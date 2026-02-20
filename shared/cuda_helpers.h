#ifndef CUDA_HELPERS_H
#define CUDA_HELPERS_H

#include <cuda.h>
#include <nvrtc.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  CUdevice device;
  CUcontext ctx;
  int major;
  int minor;
} cuda_state;

char* read_file(const char* path);
cuda_state cuda_init(void);
void cuda_print_device_info(const cuda_state* s);
int cuda_compile_kernel(const cuda_state* s, const char* source, CUmodule* module);
void cuda_cleanup(cuda_state* s, CUmodule module);

#endif
