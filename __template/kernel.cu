extern "C" __global__ void hello() {
  printf("Hello from block %d, thread %d\n", blockIdx.x, threadIdx.x);
}
