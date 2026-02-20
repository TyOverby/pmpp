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

static char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f)
    return NULL;
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buf = malloc(len + 1);
  size_t n = fread(buf, 1, len, f);
  buf[n] = '\0';
  fclose(f);
  return buf;
}

static cuda_state cuda_init(void) {
  cuda_state s;
  cuInit(0);
  cuDeviceGet(&s.device, 0);
  cuDevicePrimaryCtxRetain(&s.ctx, s.device);
  cuCtxSetCurrent(s.ctx);
  cuDeviceGetAttribute(&s.major,
                       CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, s.device);
  cuDeviceGetAttribute(&s.minor,
                       CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, s.device);
  return s;
}

static void cuda_print_device_info(const cuda_state *s) {
  char name[256];
  cuDeviceGetName(name, sizeof(name), s->device);
  printf("Device: %s\nCompute capability: %d.%d\n\n", name, s->major,
         s->minor);
}

// Compiles a .cu source string with NVRTC and loads it as a CUmodule.
// Returns 0 on success, non-zero on failure (with errors printed to stderr).
static int cuda_compile_kernel(const cuda_state *s, const char *source,
                               CUmodule *module) {
  nvrtcProgram prog;
  nvrtcCreateProgram(&prog, source, "kernel.cu", 0, NULL, NULL);

  char arch_flag[32];
  snprintf(arch_flag, sizeof(arch_flag), "--gpu-architecture=sm_%d%d", s->major,
           s->minor);
  const char *opts[] = {arch_flag};
  nvrtcResult res = nvrtcCompileProgram(prog, 1, opts);

  if (res != NVRTC_SUCCESS) {
    size_t log_size;
    nvrtcGetProgramLogSize(prog, &log_size);
    char *log = malloc(log_size);
    nvrtcGetProgramLog(prog, log);
    fprintf(stderr, "Compilation failed:\n%s\n", log);
    free(log);
    nvrtcDestroyProgram(&prog);
    return 1;
  }

  size_t ptx_size;
  nvrtcGetPTXSize(prog, &ptx_size);
  char *ptx = malloc(ptx_size);
  nvrtcGetPTX(prog, ptx);
  nvrtcDestroyProgram(&prog);

  cuModuleLoadData(module, ptx);
  free(ptx);
  return 0;
}

static void cuda_cleanup(cuda_state *s, CUmodule module) {
  cuModuleUnload(module);
  cuDevicePrimaryCtxRelease(s->device);
}

#endif
