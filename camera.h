//
// Created by vivek on 2/7/2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "pdf.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
// #include <omp.h>
#include <sstream>

using namespace std::chrono;

class camera {
public:
  double aspect_ratio = 1.0;
  int image_width = 100;
  int samples_per_pixel = 10;
  int max_depth = 10;
  color background;
  int num_threads = 0; // 0 means use OpenMP default

  double vfov = 90;                  // Vertical view angle (field of view)
  point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
  point3 lookat = point3(0, 0, -1);  // Point camera is looking at
  vec3 vup = vec3(0, 1, 0);          // Camrea-relative "up" direction

  double defocus_angle = 0; // Variation angle of rays through each pixel
  double focus_dist = 10;

  void render(const hittable &world, const hittable &lights) {
    initialize();

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    int chunk_size = 16;

    // Use a thread-safe vector with pre-allocated size
    std::vector<std::string> output_buffer(image_height * image_width);
    std::mutex output_mutex;

    std::atomic<int> scanlines_completed(0);
    std::atomic<bool> rendering_complete(false);
    std::condition_variable cv;
    std::mutex cv_mutex;

    std::thread progress_thread(progress_tracker, std::ref(scanlines_completed),
                                std::ref(rendering_complete), std::ref(cv),
                                std::ref(cv_mutex), image_height);

#pragma omp parallel
    {
      // Thread-local temporary storage
      std::vector<std::string> local_buffer(image_width);

#pragma omp for schedule(dynamic, chunk_size)
      // Iterate over image dimensions
      for (int j = 0; j < image_height; j++) {
        local_buffer.clear();
        local_buffer.reserve(image_width);
        for (int i = 0; i < image_width; i++) {
          color pixel_color(0, 0, 0);
          // stratified sampling
          for (int s_j = 0; s_j < sqrt_spp; s_j++) {
            for (int s_i = 0; s_i < sqrt_spp; s_i++) {
              ray r = get_ray(i, j, s_i, s_j);
              pixel_color += ray_color(r, max_depth, world, lights);
            }
          }

          // Convert pixel color to string
          std::ostringstream pixel_stream;
          auto scaled_color = pixel_samples_scale * pixel_color;
          write_color(pixel_stream, scaled_color);

          local_buffer.push_back(pixel_stream.str());
        }

        {
          std::lock_guard<std::mutex> lock(output_mutex);
          std::copy(local_buffer.begin(), local_buffer.end(),
                    output_buffer.begin() + j * image_width);
        }
        ++scanlines_completed;

        {
          std::lock_guard<std::mutex> lock(cv_mutex);
          cv.notify_one();
        }
      }
    }

    rendering_complete = true;
    {
      std::lock_guard<std::mutex> lock(cv_mutex);
      cv.notify_one();
    }

    progress_thread.join();

    // Write all pixels in the correct order after parallel processing
    for (const auto &pixel_str : output_buffer) {
      std::cout << pixel_str;
    }
  }

private:
  int image_height;           // Rendered image height
  double pixel_samples_scale; // Color scale factor for a sum of pixel samples
  int sqrt_spp;
  double recip_sqrt_spp;
  point3 center;       // Camera center
  point3 pixel00_loc;  // Location of pixel 0,0
  vec3 pixel_delta_u;  // Offset to pixel to the right
  vec3 pixel_delta_v;  // Offset to pixel below
  vec3 u, v, w;        // Camera frame basis vectors
  vec3 defocus_disk_u; // Defocus disk horizontal radius
  vec3 defocus_disk_v; // Defocus disk vertical radius

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    sqrt_spp = int(std::sqrt(samples_per_pixel));
    pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
    recip_sqrt_spp = 1 / sqrt_spp;

    center = lookfrom;

    // Determine viewport dimensions
    // auto focal_length = (lookfrom - lookat).length();
    auto theta = degrees_2_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2.0 * h * focus_dist;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    // Calculate the unit basis vectors for the camera coordinate frame
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges.
    vec3 viewport_u =
        viewport_width * u; // Vector across viewport horizontal edge
    vec3 viewport_v =
        viewport_height * -v; // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left =
        center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors
    auto defocus_radius =
        focus_dist * std::tan(degrees_2_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  // Static method for progress tracking

  static void progress_tracker(std::atomic<int> &scanlines_completed,
                               std::atomic<bool> &rendering_complete,
                               std::condition_variable &cv,
                               std::mutex &cv_mutex, int image_height) {

    int last_count = 0;

    auto start_time = steady_clock::now();

    while (scanlines_completed.load() < image_height) {

      std::this_thread::sleep_for(milliseconds(200)); // update every 200ms

      int current = scanlines_completed.load();

      auto now = steady_clock::now();

      auto elapsed = duration_cast<seconds>(now - start_time).count();

      if (current > 0 && elapsed > 0) {

        double scanlines_per_second = static_cast<double>(current) / elapsed;

        int remaining_scanlines = image_height - current;

        int eta_seconds =
            static_cast<int>(remaining_scanlines / scanlines_per_second);

        std::clog << "\rScanlines remaining: " << remaining_scanlines << " ("

                  << static_cast<int>((current * 100.0) / image_height) << "%)"

                  << " ETA: " << eta_seconds / 60 << "m" << eta_seconds % 60

                  << "s " << std::flush;

      } else {

        std::clog << "\rScanlines remaining: " << (image_height - current)

                  << ' ' << std::flush;
      }

      last_count = current;
    }

    std::clog << "\rRendering completed";
  }

  color ray_color(const ray &r, int depth, const hittable &world,
                  const hittable &lights) const {
    if (depth <= 0)
      return color(0, 0, 0);

    hit_record rec;

    // If the ray htis nothing, return the background color
    if (!world.hit(r, interval(0.001, infinity), rec))
      return background;

    scatter_record srec;
    color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.mat->scatter(r, rec, srec))
      return color_from_emission;
    if (srec.skip_pdf) {
            return srec.attenuation * ray_color(srec.skip_pdf_ray, depth-1, world, lights);
        }

    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
        mixture_pdf p(light_ptr, srec.pdf_ptr);

        ray scattered = ray(rec.p, p.generate(), r.time());
        auto pdf_value = p.value(scattered.direction());

    
    double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

    color sample_color = ray_color(scattered, depth - 1, world, lights);
    color color_from_scatter =
        (srec.attenuation * scattering_pdf * sample_color) / pdf_value;

    return color_from_emission + color_from_scatter;
  }

  ray get_ray(int i, int j) const {
    // Construct a camera ray originating from the origin and directed at
    // randomly sampled point around the pixel location i, j.

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();

    return ray(ray_origin, ray_direction, ray_time);
  }

  ray get_ray(int i, int j, int s_i, int s_j) const {
    // Construct a camera ray originating from the defocus disk and directed at
    // a randomly sample point around the pixel location i, j for stratefied
    // sample quare s_i, s_j

    auto offset = sample_square_stratified(s_i, s_j);
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();

    return ray(ray_origin, ray_direction, ray_time);
  }

  vec3 sample_square_stratified(int s_i, int s_j) const {
    // Returns the vector to a random point in the square sub-pixel specified by
    // grid indices s_i and s_j, for an idealized unit square pixel [-.5, -.5]
    // to [+.5, +.5]

    auto px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5;
    auto py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5;

    return vec3(px, py, 0);
  }

  vec3 sample_square() const {
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  point3 defocus_disk_sample() const {
    // Returns a random point the camera defocus disk
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }
};

#endif // CAMERA_H
