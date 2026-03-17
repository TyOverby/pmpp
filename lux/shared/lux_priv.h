#include <stdint.h>
#include <stdlib.h>
#include "lux.h"

/* Because there are multiple different backends for lux, we build a vtable-like
 * structure to represent the scene type.  This contains userdata for the backend to
 * use as it sees fit, as well as function pointers to the underlying implementations
 * of the functions.
 */
typedef struct lux_scene {
  void* data;
  lux_instruction_buffer* (*get_instruction_buffer)(lux_scene*);
  void (*dispatch)(lux_scene*, lux_dispatch_args, lux_color*);
} lux_scene;

typedef enum { Pixel, Rect } lux_instruction_kind;

typedef union {
  struct Pixel {
    int32_t x;
    int32_t y;
    lux_color color;
  } Pixel;

  struct Rect {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    lux_color color;
  } Rect;
} lux_instruction_data;

typedef struct {
  lux_instruction_kind kind;
  lux_instruction_data data;
} lux_instruction;

typedef struct lux_instruction_buffer {
  lux_instruction* data;
  size_t length;
  size_t capacity;
  // TODO: store frame number of last edit to buffer in order to skip rendering
} lux_instruction_buffer;

// Allocate and return a new instruction buffer with the given capacity.
lux_instruction_buffer lux_priv_create_instruction_buffer(size_t);

// Free the given instruction buffer.
void lux_priv_destroy_instruction_buffer(lux_instruction_buffer*);
