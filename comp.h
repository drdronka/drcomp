#include <stdint.h>

typedef struct comp_data
{
  uint8_t *raw;
  uint32_t size_bits;
} comp_data_t;

comp_data_t *comp_compress(uint8_t *input, uint32_t size);
