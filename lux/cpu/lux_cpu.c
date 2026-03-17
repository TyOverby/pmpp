#include "lux_cpu.h"
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct lux_cpu_scene {
  lux_instruction_buffer buffer;
} lux_cpu_scene;

lux_instruction_buffer* get_instruction_buffer(lux_scene* generic_scene) {
  lux_cpu_scene* scene = (lux_cpu_scene*)generic_scene->data;
  return &scene->buffer;
}

void dispatch(lux_scene* scene, lux_dispatch_args args, lux_color* output) {
  (void)scene;
  (void)output;
  for (u_int32_t y = 0; y < args.height; y++) {
    for (u_int32_t x = 0; x < args.width; x++) {
      output[y * args.width + x] = (lux_color){0, 0, 0, 255};
    }
  }
}

lux_scene* lux_cpu_create_scene(void) {
  lux_scene* scene = (lux_scene*)malloc(sizeof(lux_scene));
  lux_cpu_scene* cpu_scene = (lux_cpu_scene*)malloc(sizeof(lux_cpu_scene));
  *cpu_scene = (lux_cpu_scene){.buffer = lux_priv_create_instruction_buffer(1024)};
  *scene = (lux_scene){.data = cpu_scene, .get_instruction_buffer = &get_instruction_buffer, .dispatch = &dispatch};
  return scene;
}
