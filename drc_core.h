#ifndef DRC_CORE_H
#define DRC_CORE_H

#include <stdint.h>
#include "drc_code.h"

typedef struct drc_core_pack
{
  uint8_t *data;
  uint32_t size_bits;
  drc_code_tab_t codes;
  float ratio;
} drc_core_pack_t;

drc_core_pack_t *drc_core_compress(uint8_t *input, uint32_t size);
void             drc_core_pack_destroy(drc_core_pack_t *pack);

#endif
