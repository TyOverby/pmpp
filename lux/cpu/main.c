#include <stdlib.h>
#include "../../stb/stb_image_write.h"
#include "../shared/lux.h"
#include "lux_cpu.h"

int main(void) {
  lux_scene* scene = lux_cpu_create_scene();
  lux_instruction_buffer* buffer = lux_get_instruction_buffer(scene);
  lux_push_pixel(buffer, 0, 0, (lux_color){.r = 255, .g = 0, .b = 0, .a = 255});
  lux_color* output = (lux_color*)malloc(256 * 256 * sizeof(lux_color));
  lux_dispatch(scene, (lux_dispatch_args){.x = 0, .y = 0, .width = 256, .height = 256}, output);
  stbi_write_png("output.png", 256, 256, 4, output, 4);
}
