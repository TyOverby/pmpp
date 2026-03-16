// Size of local mem region should be
// (blur_radius * 2 + width) * (blur_radius * 2 + height)
extern __shared__ char local_mem[];

// Convert global coordinates to an offset into shared memory.
int __device__ translate_offset(int x, int y, int blur_radius) {
  int offset_x = x - blockDim.x * blockIdx.x + blur_radius;
  int offset_y = y - blockDim.y * blockIdx.y + blur_radius;
  int width_with_radius = blockDim.x + 2 * blur_radius;
  return (offset_y * width_with_radius + offset_x) * 3;
}

// This one works
void __device__ copy_in(const unsigned char* input, int width, int height, int blur_radius) {
  int cx = blockDim.x * blockIdx.x + threadIdx.x;
  int cy = blockDim.y * blockIdx.y + threadIdx.y;
  int local_offset = translate_offset(cx, cy, blur_radius);
  int global_offset = (cy * width + cx) * 3;
  local_mem[local_offset + 0] = input[global_offset + 0];
  local_mem[local_offset + 1] = input[global_offset + 1];
  local_mem[local_offset + 2] = input[global_offset + 2];
}

// This one is broken
void __device__ copy_in_2(const unsigned char* input, int width, int height, int blur_radius) {
  if (threadIdx.x == 0 && threadIdx.y == 0) {
    int cx = blockDim.x * blockIdx.x + threadIdx.x;
    int cy = blockDim.y * blockIdx.y + threadIdx.y;

    for (int y = max(0, cy - blur_radius); y < cy + blockDim.y + blur_radius; y++) {
      for (int x = max(0, cx - blur_radius); x < cx + blockDim.x + blur_radius; x++) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
          int local_offset = translate_offset(x, y, blur_radius);
          int global_offset = (y * width + x) * 3;
          local_mem[local_offset + 0] = input[global_offset + 0];
          local_mem[local_offset + 1] = input[global_offset + 1];
          local_mem[local_offset + 2] = input[global_offset + 2];
        } 
      }
    }
  }
}

void __device__ perform_blur(unsigned char* output, int width, int height, int blur_radius) {
  int cx = blockDim.x * blockIdx.x + threadIdx.x;
  int cy = blockDim.y * blockIdx.y + threadIdx.y;
  int local_offset = translate_offset(cx, cy, blur_radius);
  int global_offset = (cy * width + cx) * 3;
  output[global_offset + 0] = local_mem[local_offset + 0];
  output[global_offset + 1] = local_mem[local_offset + 1];
  output[global_offset + 2] = local_mem[local_offset + 2];
}

extern "C" __global__ void blur(const unsigned char* input,
                                unsigned char* output,
                                int width,
                                int height,
                                int blur_radius) {
  copy_in_2(input, width, height, blur_radius);
  __syncthreads();

  perform_blur(output, width, height, blur_radius);
}
