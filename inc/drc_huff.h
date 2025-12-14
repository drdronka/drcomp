#ifndef DRC_HUFF_H
#define DRC_HUFF_H

#define BYTE_RANGE 256

typedef struct drc_huff_stats
{
  uint32_t weight[BYTE_RANGE];
} drc_huff_stats_t;

typedef struct drc_huff_tab
{
  uint8_t *code[BYTE_RANGE];
  uint8_t size[BYTE_RANGE];
} drc_huff_tab_t;

// hybrid bintree/linkedlist structure for Huffman tree
typedef struct drc_huff_node
{
  uint8_t byte_val;
  uint32_t byte_weight;
  struct drc_huff_node *left;
  struct drc_huff_node *right;
  struct drc_huff_node *next;
} drc_huff_node_t;

void drc_huff_code_print(uint8_t *code, uint8_t size, uint8_t newline);

drc_huff_node_t *drc_huff_node_create(uint8_t byte_val, uint32_t byte_weight);
void drc_huff_node_destroy(drc_huff_node_t *node);

drc_huff_stats_t *drc_huff_stats_calc(uint8_t *input, uint32_t size);
drc_huff_stats_t *drc_huff_stats_calc_from_file(FILE* file_in);
void drc_huff_stats_write(FILE* file_in, drc_huff_stats_t *stats);
drc_huff_stats_t *drc_huff_stats_read(FILE* file_in);
void drc_huff_stats_destroy(drc_huff_stats_t *stats);
void drc_huff_stats_print(drc_huff_stats_t *stats);

drc_huff_tab_t *drc_huff_tab_calc(drc_huff_stats_t *stats);
void drc_huff_tab_destroy(drc_huff_tab_t *tab);
void drc_huff_tab_print(drc_huff_tab_t *tab);

void drc_huff_bt_construct(drc_huff_node_t **root, drc_huff_stats_t *stats);
void drc_huff_bt_destroy(drc_huff_node_t *root);
void drc_huff_bt_print(drc_huff_node_t *root);

void drc_huff_ll_print(drc_huff_node_t *root);

#endif
