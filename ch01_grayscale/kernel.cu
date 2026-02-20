extern "C" __global__ void rgb_to_grayscale(const unsigned char* rgb, unsigned char* gray, int width, int height) {
  int x = blockDim.x * blockIdx.x + threadIdx.x;
  int y = blockDim.y * blockIdx.y + threadIdx.y;
  if (x >= width || y >= height) {
    return;
  }

  int offset = y * width + x;
  int r = rgb[offset * 3 + 0];
  int g = rgb[offset * 3 + 1];
  int b = rgb[offset * 3 + 2];

  gray[offset] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
}
