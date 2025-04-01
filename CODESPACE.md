# Using GitHub Codespaces with This Ray Tracer

This document provides detailed instructions on how to set up and use GitHub Codespaces with this ray tracer project.

## Setting Up Codespaces

### First-Time Setup

1. **Create a Codespace:**
   - Go to your GitHub repository.
   - Click the "Code" button.
   - Select the "Codespaces" tab.
   - Click "Create codespace on main".

2. **Wait for Initialization:**
   - The first time you launch a Codespace, it will take a few minutes to set up.
   - This includes building the Docker container with all necessary dependencies.

3. **Verify Setup:**
   - The `postCreateCommand` in the devcontainer.json file will automatically create a build directory, run CMake, and build the project.
   - You can check if this was successful by looking at the Terminal output.

## Building and Running

### Building the Project

If you need to rebuild the project:

```bash
mkdir -p build
cd build
cmake ..
make
```

### Running the Ray Tracer

To run the ray tracer:

```bash
cd build
./raytracer
```

By default, the output will be a PPM image file. 

### Viewing the Output

PPM files are not directly viewable in most browsers or applications. You have a few options:

1. **Convert to PNG:**
   ```bash
   # Install ImageMagick if not included in the container
   sudo apt-get update && sudo apt-get install -y imagemagick
   
   # Convert PPM to PNG
   convert output.ppm output.png
   ```

2. **Download the file:**
   - Right-click on the file in the Explorer and select "Download..."
   - Open the file on your local machine with a viewer that supports PPM.

3. **View in the terminal (for smaller images):**
   ```bash
   head -n 100 output.ppm
   ```

## Optimizing Rendering

### Adjust Samples Per Pixel

For faster development iterations, reduce the samples per pixel:

```cpp
cam.samples_per_pixel = 10; // Lower for faster renders during development
```

For final renders, increase it:

```cpp
cam.samples_per_pixel = 100; // Or higher for final renders
```

### Adjust Image Resolution

Similarly, you can adjust the image resolution:

```cpp
cam.image_width = 400; // Lower for faster renders
```

### Use OpenMP Parallelization

The code is already set up to use OpenMP for parallel rendering. You can adjust the number of threads:

```cpp
cam.num_threads = 4; // Set specific number of threads or 0 for default
```

## Managing Your Codespace

### Stopping and Starting

- **Stop a Codespace:** When you're not using it, you can stop your Codespace from the GitHub UI to save on usage limits.
- **Restart a Codespace:** When you're ready to work again, you can start it from the GitHub UI.

### Resource Allocation

- **Default Resources:** A Codespace typically provides 2 cores, 4GB RAM, and 32GB storage.
- **Increase Resources:** If you need more, you can configure this when creating a new Codespace.

### Port Forwarding

- If you develop a visualization tool that runs on a local server, Codespaces allows port forwarding.
- You can configure this in the `.devcontainer/devcontainer.json` file.

## Troubleshooting

### Build Errors

If you encounter build errors:

1. Check that all dependencies are properly installed.
2. Make sure the CMake configuration is correct for your system.
3. Check for compiler errors in the output.

### Performance Issues

If rendering is too slow:

1. Reduce samples per pixel or image resolution.
2. Make sure OpenMP parallelization is working correctly.
3. Consider simplifying the scene while developing.

### File Storage

Codespaces has a limit on the amount of storage available. If you generate many large output images, consider:

1. Downloading and removing them from the Codespace.
2. Using smaller resolutions for development.
3. Storing final outputs in the repository or elsewhere.
