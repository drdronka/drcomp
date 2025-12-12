#ifndef DRC_CODE_H
#define DRC_CODE_H

#include <stdint.h>

#define BYTE_RANGE 256

typedef struct drc_code_stats
{
  uint8_t weight[BYTE_RANGE];
} drc_code_stats_t;

typedef struct drc_code_tab
{
  uint8_t *code[BYTE_RANGE];
  uint32_t size[BYTE_RANGE];
} drc_code_tab_t;

drc_code_stats_t *drc_code_stats_calc(uint8_t *input, uint32_t size);
void              drc_code_stats_destroy(drc_code_stats_t *stats);

drc_code_tab_t   *drc_code_tab_calc(drc_code_stats_t *stats);
void              drc_code_tab_destroy(drc_code_tab_t *tab);

#endif
