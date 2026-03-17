#include "lux_priv.h"
#include <stdint.h>
#include "lux.h"

lux_instruction_buffer* lux_get_instruction_buffer(lux_scene* scene) {
  return scene->get_instruction_buffer(scene);
}

void lux_dispatch(lux_scene* scene, lux_dispatch_args args, lux_color* pixel_buffer) {
  scene->dispatch(scene, args, pixel_buffer);
}

lux_instruction_buffer lux_priv_create_instruction_buffer(size_t capacity) {
  lux_instruction_buffer buffer;
  buffer.data = (lux_instruction*)malloc(capacity * sizeof(lux_instruction));
  buffer.length = 0;
  buffer.capacity = capacity;
  return buffer;
}

void lux_priv_destroy_instruction_buffer(lux_instruction_buffer* buffer) {
  free(buffer->data);
  buffer->data = NULL;
  buffer->length = 0;
  buffer->capacity = 0;
}

void lux_priv_push_instruction(lux_instruction_buffer* buffer, lux_instruction instruction) {
  if (buffer->length >= buffer->capacity) {
    size_t new_capacity = (buffer->capacity * 3) / 2;
    lux_instruction* new_data = (lux_instruction*)realloc(buffer->data, new_capacity * sizeof(lux_instruction));
    if (new_data == NULL) {
      return;
    }
    buffer->data = new_data;
    buffer->capacity = new_capacity;
  }
  buffer->data[buffer->length++] = instruction;
}

void lux_push_pixel(lux_instruction_buffer* buffer, int32_t x, int32_t y, lux_color color) {
  lux_instruction_data data = (lux_instruction_data){.Pixel.x = x, .Pixel.y = y, .Pixel.color = color};
  lux_priv_push_instruction(buffer, (lux_instruction){.kind = Pixel, .data = data});
}

void lux_push_rect(lux_instruction_buffer* buffer, int32_t x, int32_t y, int32_t w, int32_t h, lux_color color) {
  lux_instruction_data data =
      (lux_instruction_data){.Rect.x = x, .Rect.y = y, .Rect.w = w, .Rect.h = h, .Rect.color = color};
  lux_priv_push_instruction(buffer, (lux_instruction){.kind = Rect, .data = data});
}
