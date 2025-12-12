#ifndef DRC_CODE_H
#define DRC_CODE_H

#include <stdint.h>

#include "drc_huff.h"

typedef struct drc_code_tab
{
  uint8_t *code[BYTE_RANGE];
  uint32_t size[BYTE_RANGE];
} drc_code_tab_t;

drc_code_tab_t   *drc_code_tab_calc(drc_huff_stats_t *stats);
void              drc_code_tab_destroy(drc_code_tab_t *tab);

#endif
