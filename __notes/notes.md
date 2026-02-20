# Memory management

`cudaErrorEnum cuMemAlloc(unsigned long long* ptr, size_t bytes)`
Returns an enum and writes the device pointer into the location provided by the first arg.

# Built-in variables

- `blockDim`: number of threads in a block
- `blockIdx`: currently running block index
- `threadIdx`: currently running thread index (inside a block)
