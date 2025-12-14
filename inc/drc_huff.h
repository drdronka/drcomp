#ifndef DRC_HUFF_H
#define DRC_HUFF_H

#define BYTE_RANGE 256

typedef struct drc_huff_stats
{
  uint8_t weight[BYTE_RANGE];
} drc_huff_stats_t;

typedef struct drc_huff_tab
{
  uint8_t *code[BYTE_RANGE];
  uint8_t size[BYTE_RANGE];
} drc_huff_tab_t;

drc_huff_stats_t *drc_huff_stats_calc(uint8_t *input, uint32_t size);
drc_huff_stats_t *drc_huff_stats_calc_from_file(FILE* file_in);
void drc_huff_stats_print(drc_huff_stats_t *stats);
void drc_huff_stats_destroy(drc_huff_stats_t *stats);
drc_huff_tab_t *drc_huff_tab_calc(drc_huff_stats_t *stats);
void drc_huff_tab_write(FILE* file_out, drc_huff_tab_t *tab);
drc_huff_tab_t *drc_huff_tab_read(FILE* file_in);
void drc_huff_tab_print(drc_huff_tab_t *tab);
void drc_huff_tab_destroy(drc_huff_tab_t *tab);

#endif
