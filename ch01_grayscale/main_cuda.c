#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

#include "../shared/cuda_helpers.h"

int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <kernel.cu> <input.png> <output.png>\n", argv[0]);
    return 1;
  }

  const char* kernel_path = argv[1];
  const char* input_path = argv[2];
  const char* output_path = argv[3];

  cuda_state s = cuda_init();

  // Load and compile kernel
  char* source = read_file(kernel_path);
  if (!source) {
    fprintf(stderr, "Failed to read %s\n", kernel_path);
    return 1;
  }

  CUmodule module;
  if (cuda_compile_kernel(&s, source, &module)) {
    return 1;
  }
  free(source);

  CUfunction kernel;
  cuModuleGetFunction(&kernel, module, "rgb_to_grayscale");

  // Load image
  int width, height, channels;
  unsigned char* img = stbi_load(input_path, &width, &height, &channels, 3);
  if (!img) {
    fprintf(stderr, "Failed to load image: %s\n", input_path);
    return 1;
  }

  printf("Loaded %s (%dx%d, %d channels)\n", input_path, width, height, channels);

  int num_pixels = width * height;

  // Allocate device memory
  CUdeviceptr d_rgb, d_gray;
  cuMemAlloc(&d_rgb, 3 * num_pixels);
  cuMemAlloc(&d_gray, num_pixels);

  // Copy input to device
  cuMemcpyHtoD(d_rgb, img, 3 * num_pixels);
  stbi_image_free(img);

  // Launch kernel
  int threads_per_block = 256;
  int num_blocks = (num_pixels + threads_per_block - 1) / threads_per_block;
  void* args[] = {&d_rgb, &d_gray, &width, &height};
  cuLaunchKernel(kernel, num_blocks, 1, 1, threads_per_block, 1, 1, 0, NULL, args, NULL);
  cuCtxSynchronize();

  // Copy result back to host
  unsigned char* gray = malloc(num_pixels);
  cuMemcpyDtoH(gray, d_gray, num_pixels);

  cuMemFree(d_rgb);
  cuMemFree(d_gray);

  if (!stbi_write_png(output_path, width, height, 1, gray, width)) {
    fprintf(stderr, "Failed to write image: %s\n", output_path);
    free(gray);
    return 1;
  }

  printf("Wrote grayscale image to %s\n", output_path);
  free(gray);
  cuda_cleanup(&s, module);
  return 0;
}
