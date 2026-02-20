int __shared__ counter;

extern "C" __global__ void kernel(const unsigned char* input, unsigned char* output, int width, int height) {
  int cx = blockDim.x * blockIdx.x + threadIdx.x;
  int cy = blockDim.y * blockIdx.y + threadIdx.y;

  if (cx >= width || cy >= height) {
    return;
  }

  int value = atomicAdd(&counter, 1);

  int offset = cy * width + cx;
  int r = value % 255;
  int g = (value >> 8) % 255;
  int b = (value >> 16) % 255;

  output[offset * 3 + 0] = r;
  output[offset * 3 + 1] = g;
  output[offset * 3 + 2] = b;
}
