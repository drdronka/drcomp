#ifndef DRC_CORE_H
#define DRC_CORE_H

#include <stdint.h>

#include "drc_huff.h"

typedef struct drc_core_pack
{
  uint8_t *data;
  uint32_t size_bits;
  drc_huff_tab_t codes;
  float ratio;
} drc_core_pack_t;

void drc_core_file_compress(uint8_t *path_in, uint8_t *path_out);
void drc_core_file_decompress(uint8_t *path_in, uint8_t *path_out);
//void*drc_core_compress(uint8_t *input, uint32_t size);

#endif
