#include "../shared/cuda_helpers.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <kernel.cu>\n", argv[0]);
    return 1;
  }

  cuda_state s = cuda_init();
  cuda_print_device_info(&s);

  char *source = read_file(argv[1]);
  if (!source) {
    fprintf(stderr, "Failed to read %s\n", argv[1]);
    return 1;
  }

  CUmodule module;
  if (cuda_compile_kernel(&s, source, &module))
    return 1;
  free(source);

  CUfunction kernel;
  cuModuleGetFunction(&kernel, module, "hello");

  // Launch: 2 blocks, 4 threads
  cuLaunchKernel(kernel, 2, 1, 1, 4, 1, 1, 0, NULL, NULL, NULL);
  cuCtxSynchronize();

  printf("\nCUDA is working!\n");

  cuda_cleanup(&s, module);
  return 0;
}
