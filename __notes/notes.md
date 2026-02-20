# Memory management

- `cudaErrorEnum cuMemAlloc(unsigned long long* ptr, size_t bytes)` returns an error enum and writes the device pointer into the location provided by the first arg.

# Built-in variables

- `blockDim`: number of threads in a block
- `blockIdx`: currently running block index
- `threadIdx`: currently running thread index (inside a block)

# Memory types

```
┌──────────┬──────────┬────────┬───────────────────────────────────┐
│  Memory  │ Location │ Scope  │              Latency              │
├──────────┼──────────┼────────┼───────────────────────────────────┤
│ Register │ On-chip  │ Thread │ ~1 cycle                          │
├──────────┼──────────┼────────┼───────────────────────────────────┤
│ Shared   │ On-chip  │ Block  │ ~1 cycle                          │
├──────────┼──────────┼────────┼───────────────────────────────────┤
│ Constant │ Cached   │ Grid   │ ~1 cycle (broadcast) to ~hundreds │
├──────────┼──────────┼────────┼───────────────────────────────────┤
│ Local    │ Off-chip │ Thread │ ~hundreds of cycles               │
├──────────┼──────────┼────────┼───────────────────────────────────┤
│ Global   │ Off-chip │ Grid   │ ~hundreds of cycles               │
└──────────┴──────────┴────────┴───────────────────────────────────┘
```

## Shared memory

Shared memory isn't automatically populated, you need to get the threads to copy data from global memory to shared memory and back again. This means that it's not worth it if you're only doing one read / write. After copying, synchronize the threads in the block
before reading from shared memory.

If you know the size of a shared memory array, you can declare it like so: `__shared__ float weights[1024]`. But if you don't know the size at compile time, then you declare it like `extern __shared__ float weights[]` and then pass in the size of shared memory in bytes to
`cuLaunchKernel` like so:

```c
cuLaunchKernel(kernel, blocks_x, blocks_y, 1,
               threads_per_dimension, threads_per_dimension, 1,
               shared_mem_bytes, // <-- shared memory bytes go here
               NULL, args, NULL);
```

**IMPORTANT**: if you have multiple dynamically sized shared arrays, they all alias one another, so you need to manually figure out where one starts and another ends. This is wild.
