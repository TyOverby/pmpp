
static int __device__ blur_radius = 5;

extern "C" __global__ void kernel(const unsigned char* input, unsigned char* output, int width, int height) {
  int cx = blockDim.x * blockIdx.x + threadIdx.x;
  int cy = blockDim.y * blockIdx.y + threadIdx.y;

  if (cx >= width || cy >= height) {
    return;
  }

  int out_r = 0;
  int out_g = 0;
  int out_b = 0;
  int num_pixels = 0;

  for (int y = cy - blur_radius; y <= cy + blur_radius; y++) {
    for (int x = cx - blur_radius; x <= cx + blur_radius; x++) {
      if (x >= 0 && x < width && y >= 0 && y < height) {
        int offset = y * width + x;

        int r = input[offset * 3 + 0];
        int g = input[offset * 3 + 1];
        int b = input[offset * 3 + 2];

        out_r += r;
        out_g += g;
        out_b += b;

        num_pixels += 1;
      }
    }
  }

  int offset = cy * width + cx;
  int r = out_r / num_pixels;
  int g = out_g / num_pixels;
  int b = out_b / num_pixels;

  output[offset * 3 + 0] = r;
  output[offset * 3 + 1] = g;
  output[offset * 3 + 2] = b;
}
