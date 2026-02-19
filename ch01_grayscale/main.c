#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>

void rgb_to_grayscale(const unsigned char *rgb, unsigned char *gray, int width,
                      int height) {
  for (int i = 0; i < width * height; i++) {
    int r = rgb[3 * i + 0];
    int g = rgb[3 * i + 1];
    int b = rgb[3 * i + 2];
    gray[i] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
  }
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

  unsigned char *gray = malloc(width * height);
  rgb_to_grayscale(img, gray, width, height);
  stbi_image_free(img);

  if (!stbi_write_png(output_path, width, height, 1, gray, width)) {
    fprintf(stderr, "Failed to write image: %s\n", output_path);
    free(gray);
    return 1;
  }

  printf("Wrote grayscale image to %s\n", output_path);
  free(gray);
  return 0;
}
