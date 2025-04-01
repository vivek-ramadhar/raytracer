# Ray Tracer

A C++ ray tracer implementation based on "Ray Tracing in One Weekend" series, with various optimizations including BVH acceleration structure and OpenMP parallelization.

## Features

- Physically-based rendering
- Bounding Volume Hierarchy (BVH) for accelerated ray-scene intersection
- Texture mapping support
- Perlin noise for procedural textures
- Motion blur support
- OpenMP parallelization for faster rendering
- Support for various materials (Lambertian, Metal, Dielectric, etc.)
- Support for various primitives (Spheres, Quads, Boxes)
- Volumetric rendering with constant medium

## Development in GitHub Codespaces

This project is configured to work with GitHub Codespaces, allowing you to develop directly in your browser with all dependencies pre-installed.

### Getting Started with Codespaces

1. Click the "Code" button on the GitHub repository
2. Select the "Codespaces" tab
3. Click "Create codespace on main"
4. Wait for the environment to set up

### Building the Project

The project is set up with CMake, making it easy to build:

```bash
mkdir -p build
cd build
cmake ..
make
```

### Running the Ray Tracer

Once built, you can run the ray tracer:

```bash
cd build
./raytracer
```

This will generate a PPM image file that you can download and view. The default scene is set in the `main.cpp` file.

### Viewing PPM Images

Since PPM images aren't directly viewable in most image viewers, you can convert them using the included ImageMagick:

```bash
convert output.ppm output.png
```

Or view directly in the terminal with:

```bash
# For small images
head -n 20 output.ppm
```

### Modifying the Code

The project is structured as follows:

- Geometry classes (`sphere.h`, `quad.h`, etc.)
- Material classes (`material.h`)
- Texture classes (`texture.h`)
- Acceleration structures (`bvh.h`, `aabb.h`)
- Utilities (`vec3.h`, `ray.h`, etc.)

To change the rendered scene, modify the `main.cpp` file and select a different scene function.

## Project Structure

- `aabb.h` - Axis-aligned bounding box implementation
- `bvh.h` - Bounding volume hierarchy acceleration structure
- `camera.h` - Camera implementation with defocus blur and motion blur
- `color.h` - Color representation and output
- `constant_medium.h` - Volumetric rendering support
- `hittable.h` - Base interface for ray-hittable objects
- `hittable_list.h` - Collection of hittable objects
- `interval.h` - Utility for interval representations
- `material.h` - Material system (diffuse, metal, dielectric, etc.)
- `perlin.h` - Perlin noise implementation for textures
- `quad.h` - Quad primitive implementation
- `ray.h` - Ray representation
- `rtw_stb_image.h` - Image loading wrapper
- `rtweekend.h` - Common utilities
- `sphere.h` - Sphere primitive implementation
- `texture.h` - Texture system
- `vec3.h` - Vector math library

## Local Development (Outside of Codespaces)

If you prefer to develop locally, you'll need:

- A C++ compiler with C++17 support
- CMake (version 3.10 or higher)
- OpenMP for parallelization

Follow the same build instructions as above.

## License

This project is available under the MIT License. See the LICENSE file for more details.
