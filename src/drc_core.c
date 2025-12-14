#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "drc_core.h"
#include "drc_huff.h"
#include "drc_log.h"

#define COMP_BLOCK_SIZE 4


void drc_core_file_compress(uint8_t *path_in, uint8_t *path_out)
{
  DRC_LOG_INFO("compress: input[%s] output[%s]\n", path_in, path_out);

  FILE* file_in = fopen(path_in, "rb");
  FILE* file_out = fopen(path_out, "wb");

  if(!file_in || !file_out)
  {
    DRC_LOG_ERROR(
      "failed to open file: %s\n", !file_in ? path_in : path_out);
    return;
  }
  
  drc_huff_stats_t *stats = drc_huff_stats_calc_from_file(file_in);
  drc_huff_tab_t *tab = drc_huff_tab_calc(stats);

#if DRC_LOG_DEBUG_EN
  drc_huff_stats_print(stats);
  drc_huff_tab_print(tab);
#endif

  drc_huff_tab_write(file_out, tab);

  drc_huff_tab_destroy(tab);
  drc_huff_stats_destroy(stats);

  fclose(file_out);
  fclose(file_in);
}

void drc_core_file_decompress(uint8_t *path_in, uint8_t *path_out)
{
  DRC_LOG_INFO("compress: input[%s] output[%s]\n", path_in, path_out);

  FILE* file_in = fopen(path_in, "rb");
  FILE* file_out = fopen(path_out, "wb");

  if(!file_in || !file_out)
  {
    DRC_LOG_ERROR(
      "failed to open file: %s\n", !file_in ? path_in : path_out);
    return;
  }

  drc_huff_tab_t* tab = drc_huff_tab_read(file_in);

#if DRC_LOG_DEBUG_EN
  drc_huff_tab_print(tab);
#endif

  drc_huff_tab_destroy(tab);

  fclose(file_out);
  fclose(file_in);
}
drc_core_pack_t *drc_core_compress(uint8_t *input, uint32_t size) 
{ 
  DRC_LOG_INFO("input[%s] size[%u]\n", input, size);  

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

