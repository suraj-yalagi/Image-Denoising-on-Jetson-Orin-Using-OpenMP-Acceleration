# Image Denoising on NVIDIA Jetson Orin using OpenMP Acceleration

Image denoising pipeline implemented in C++ and accelerated with OpenMP for NVIDIA Jetson Orin platforms. The pipeline targets grayscale images with heavy salt-and-pepper style noise and combines median filtering, min-max normalization, and sharpening. Parallelization with OpenMP improves CPU core utilization and reduces end-to-end processing time.

Key result: OpenMP acceleration achieves ~1.89× speedup — total time reduced from 2805.85 ms (serial) to 1478.46 ms (parallel) on the tested Jetson Orin setup.

---

## Features
- Median filtering to remove impulse noise (salt-and-pepper).
- Min-max normalization for contrast adjustment.
- Image sharpening to restore edge clarity.
- OpenMP parallelization to utilize multiple CPU cores on Jetson Orin.
- Simple C++ implementation suitable for embedded deployments.

---

## Repository structure (example)
(The repository structure below is a suggested/expected layout; actual files may differ.)
- `src/` — C++ source files
- `include/` — header files
- `images/` — sample inputs and outputs
- `build/` — build directory (ignored by .gitignore)
- `CMakeLists.txt` or a Makefile to build the project
- `README.md`, `LICENSE`

---

## Requirements
- NVIDIA Jetson Orin device (or any Linux system with comparable CPU/OpenMP support for development).
- A C++17-capable compiler (g++ recommended) with OpenMP support.
- OpenCV (>= 3.4 or 4.x) for image I/O and basic image operations.
- CMake (optional, if the repo provides CMake build files) or a working Make toolchain.

Recommended packages (Ubuntu / JetPack environments):
- build-essential (g++/make)
- cmake
- libopencv-dev (or install OpenCV from source if needed)

---

## Build (examples)

Using CMake (recommended if CMakeLists.txt is provided):
1. Clone repository:
   git clone https://github.com/suraj-yalagi/Image-Denoising-on-Jetson-Orin-Using-OpenMP-Acceleration.git
2. Create and build:
   mkdir -p build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . -- -j$(nproc)

Using a direct g++ compile (if there are a few source files):
- Example:
  g++ -O3 -std=c++17 -fopenmp -o denoise src/*.cpp `pkg-config --cflags --libs opencv4`

Notes:
- Ensure `-fopenmp` is present to enable OpenMP.
- Use `-O2`/`-O3` for optimization on embedded platforms.
- On Jetson, the system compiler is typically `aarch64-linux-gnu-g++` in cross-build setups — adapt accordingly.

---

## Run / Usage (examples)
Because command-line arguments vary by implementation, replace the example binary and flags with those used in this repository.

Example:
- Single image:
  ./denoise -i images/noisy.png -o results/denoised.png -t 4
- Batch processing:
  ./denoise -input_dir images/noisy/ -output_dir results/ -threads 6

Common flags:
- `-i` / `--input` : input image path
- `-o` / `--output`: output image path
- `-t` / `--threads`: number of OpenMP threads to use (defaults to number of logical cores)

If the project uses a different CLI, consult the source (main.cpp or README in the repo) for exact invocation.

---

## Tuning and performance tips
- Threads: Test different thread counts (e.g., 1, 2, 4, 8) and pick the one giving the best throughput on your Orin configuration. Use environment variable OMP_NUM_THREADS or pass thread count programmatically.
- Affinity: Set thread affinity to avoid thread migration overhead (e.g., `export OMP_PROC_BIND=TRUE`).
- Compiler flags: Use `-O3`, enable loop vectorization and tune architecture-specific flags if cross-compiling.
- Memory: Minimize costly memory allocations inside hot loops; reuse buffers when possible.
- OpenCV: Use optimized builds of OpenCV (with NEON/TBB support when available) for best performance.
- Profiling: Use `perf`, `nvprof` (if applicable), or Linux perf tools to identify hotspots.

---

## Results (reported)
- Serial total time: 2805.85 ms
- Parallel total time (OpenMP): 1478.46 ms
- Speedup: ≈ 1.89×
These results were obtained on the described Jetson Orin target using the implemented pipeline (median filter → min-max normalization → sharpening).

---

## Troubleshooting
- Build failures: Ensure OpenCV development headers are installed and pkg-config finds opencv4. Adjust include/library paths if necessary.
- OpenMP not active: Verify compiler supports OpenMP and that `-fopenmp` was used during compilation.
- Performance lower than expected: Check the number of threads, affinity, and whether image I/O is dominating runtime. Use warm-up runs when measuring.
- Image color issues: The implementation targets grayscale images; ensure inputs are single-channel or convert them before processing.

---

## Contributing
Contributions, issues, and feature requests are welcome. Typical contributions:
- Bug fixes and reproducible issues
- Performance tuning and profiling reports
- Support for color images (per-channel or alternative denoising strategies)
- CI integration and packaging

Please open issues or pull requests on the repository.

---

## License
Check the repository `LICENSE` file for the exact license. If no license file is present, assume "All rights reserved" and contact the repository owner for permission to reuse code.

---

## Acknowledgements
- OpenCV for providing image I/O and basic image operations.
- NVIDIA Jetson platform for the target hardware.

---

## Contact
Repository owner: suraj-yalagi (https://github.com/suraj-yalagi)
For questions related to benchmarking or implementation details, open an issue on the repository or contact the owner via their GitHub profile.

```
