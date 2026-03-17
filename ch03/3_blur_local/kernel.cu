// Size of local mem region should be
// (blur_radius * 2 + width) * (blur_radius * 2 + height)
extern __shared__ unsigned char local_mem[];

// Convert global coordinates to an offset into shared memory.
int __device__ translate_offset(int x, int y, int blur_radius) {
  int offset_x = x - (int)(blockDim.x * blockIdx.x) + blur_radius;
  int offset_y = y - (int)(blockDim.y * blockIdx.y) + blur_radius;
  int width_with_radius = (int)blockDim.x + 2 * blur_radius;
  return (offset_y * width_with_radius + offset_x) * 3;
}

// This one works
void __device__ copy_in(const unsigned char* input, int width, int height, int blur_radius) {
  int cx = (int)(blockDim.x * blockIdx.x + threadIdx.x);
  int cy = (int)(blockDim.y * blockIdx.y + threadIdx.y);
  int local_offset = translate_offset(cx, cy, blur_radius);
  int global_offset = (cy * width + cx) * 3;
  local_mem[local_offset + 0] = input[global_offset + 0];
  local_mem[local_offset + 1] = input[global_offset + 1];
  local_mem[local_offset + 2] = input[global_offset + 2];
}

// This one is broken
void __device__ copy_in_2(const unsigned char* input, int width, int height, int blur_radius) {
  if (threadIdx.x == 0 && threadIdx.y == 0) {
    int cx = (int)(blockDim.x * blockIdx.x) + (int)threadIdx.x;
    int cy = (int)(blockDim.y * blockIdx.y) + (int)threadIdx.y;

    for (int y = cy - blur_radius; y < cy + (int)blockDim.y + blur_radius; y++) {
      for (int x = cx - blur_radius; x < cx + (int)blockDim.x + blur_radius; x++) {
        if ((x > cx && x < cx + (int)blockDim.x) &&
            (y > cy && y < cy + (int)blockDim.y)) {
            continue;
        }
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
        int offset = translate_offset(x, y, blur_radius);

        int r = local_mem[offset + 0];
        int g = local_mem[offset + 1];
        int b = local_mem[offset + 2];

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

extern "C" __global__ void blur(const unsigned char* input,
                                unsigned char* output,
                                int width,
                                int height,
                                int blur_radius) {
  copy_in(input, width, height, blur_radius);

  if (threadIdx.x == 0 && threadIdx.y == 0)  {
      copy_in_2(input, width, height, blur_radius);
  }

  __syncthreads();

  perform_blur(output, width, height, blur_radius);
}
