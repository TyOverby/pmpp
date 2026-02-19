#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

#include <cstdio>
#include <cstdlib>

__global__ void rgb_to_grayscale(const unsigned char *rgb, unsigned char *gray,
                                 int width, int height) {
  // TODO: compute pixel index from block/thread indices
  // TODO: convert rgb[3*i + 0..2] to grayscale and write to gray[i]
}

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input.png> <output.png>\n", argv[0]);
    return 1;
  }

  const char *input_path = argv[1];
  const char *output_path = argv[2];

  int width, height, channels;
  unsigned char *img = stbi_load(input_path, &width, &height, &channels, 3);
  if (!img) {
    fprintf(stderr, "Failed to load image: %s\n", input_path);
    return 1;
  }

  printf("Loaded %s (%dx%d, %d channels)\n", input_path, width, height,
         channels);

  int num_pixels = width * height;

  // Allocate device memory
  unsigned char *d_rgb, *d_gray;
  cudaMalloc(&d_rgb, 3 * num_pixels);
  cudaMalloc(&d_gray, num_pixels);

  // Copy input to device
  cudaMemcpy(d_rgb, img, 3 * num_pixels, cudaMemcpyHostToDevice);
  stbi_image_free(img);

  // Launch kernel
  int threads_per_block = 256;
  int num_blocks = (num_pixels + threads_per_block - 1) / threads_per_block;
  rgb_to_grayscale<<<num_blocks, threads_per_block>>>(d_rgb, d_gray, width,
                                                      height);

  cudaError_t err = cudaGetLastError();
  if (err != cudaSuccess) {
    fprintf(stderr, "Kernel launch failed: %s\n", cudaGetErrorString(err));
    return 1;
  }
  cudaDeviceSynchronize();

  // Copy result back to host
  unsigned char *gray = (unsigned char *)malloc(num_pixels);
  cudaMemcpy(gray, d_gray, num_pixels, cudaMemcpyDeviceToHost);

  cudaFree(d_rgb);
  cudaFree(d_gray);

  if (!stbi_write_png(output_path, width, height, 1, gray, width)) {
    fprintf(stderr, "Failed to write image: %s\n", output_path);
    free(gray);
    return 1;
  }

  printf("Wrote grayscale image to %s\n", output_path);
  free(gray);
  return 0;
}
