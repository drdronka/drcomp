#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "drc_core.h"
#include "drc_huff.h"
#include "drc_log.h"

drc_core_pack_t *drc_core_compress(uint8_t *input, uint32_t size) 
{ 
  printf("input[%s] size[%u]\n", input, size);  

  drc_huff_stats_t *stats = drc_huff_stats_calc(input, size);
  drc_huff_tab_t *tab = drc_huff_tab_calc(stats);

  uint8_t tmp_data[1000];
  uint32_t tmp_data_id = 0;
  for(uint32_t n = 0; n < size; n++)
    for(uint32_t i = 0; i < tab->size[input[n]]; i++)
      tmp_data[tmp_data_id++] = tab->code[input[n]][i];

  DRC_LOG_INFO("compressed data:\n");
  for(uint32_t n = 0; n < tmp_data_id; n++)
  {
    DRC_LOG_INFO("%c", tmp_data[n] + '0');
  }
  DRC_LOG_INFO("\ncompression ratio %0.2f\n", 
    (float)((size * 8) - tmp_data_id) / (float)(size * 8));

  drc_core_pack_t *pack = (drc_core_pack_t*)malloc(sizeof(drc_core_pack_t));

  drc_huff_tab_destroy(tab);
  drc_huff_stats_destroy(stats);

  return pack;
}

void drc_core_pack_destroy(drc_core_pack_t *pack)
{
  free(pack);
}

