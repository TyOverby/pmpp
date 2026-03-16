# Programming Massively Parallel Processors — Exercises

Working through the exercises from the book using an NVIDIA GeForce RTX 4080
(compute capability 8.9), CUDA 13.2, and driver 580.

## Project Structure

```
book/
├── shared/cuda_helpers.h   — common CUDA init, NVRTC compilation, cleanup
├── stb/                    — vendored stb single-header image libraries
├── __template/             — copy this to start a new chapter
│   ├── main.c
│   ├── kernel.cu
│   └── dune
└── ch01_grayscale/
    ├── main.c              — plain C reference implementation
    ├── main_cuda.c         — CUDA version (gcc-compiled host code)
    ├── kernel.cu           — GPU kernel (compiled at runtime)
    └── dune
```

## Design Decisions

**Runtime compilation with NVRTC instead of nvcc.**  The host code (`main.c` /
`main_cuda.c`) is compiled with plain `gcc`. The CUDA kernel source is kept in a
separate `kernel.cu` file which gets loaded as a string and compiled at runtime
via NVRTC. This means the edit-run cycle for kernel code doesn't require a
recompile of the host program — just re-run the executable.

**CUDA Driver API instead of Runtime API.**  Since we're using NVRTC to compile
kernels at runtime, we use the CUDA Driver API (`cuMemAlloc`, `cuLaunchKernel`,
etc.) rather than the Runtime API (`cudaMalloc`, `<<<>>>` syntax). The Driver API
gives explicit control over contexts, modules, and kernel launches, which pairs
naturally with runtime-compiled PTX.

**dune as the build system.**  Each chapter directory has a `dune` file with
custom rules that invoke `gcc` directly. This keeps the build declarative and
incremental without needing CMake or Makefiles. Build a chapter with
`dune build ch01_grayscale/main_cuda.exe`.

**One directory per exercise.**  Each chapter/exercise gets its own directory with
both a plain C reference implementation and a CUDA version. The C version serves
as a baseline to verify correctness before porting to the GPU.

## Usage

```bash
# build and run the template
dune build @__template/run

# build a specific target
dune build ch01_grayscale/main_cuda.exe

# run it
dune exec -- ./ch01_grayscale/main_cuda.exe \
  ch01_grayscale/kernel.cu \
  ch01_grayscale/test_input.png \
  ch01_grayscale/test_output.png
```

## Starting a New Chapter

```bash
cp -r __template ch02_something
# edit kernel.cu and main.c
```
