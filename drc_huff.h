#ifndef DRC_HUFF_H
#define DRC_HUFF_H

#define BYTE_RANGE 256

typedef struct drc_huff_stats
{
  uint8_t weight[BYTE_RANGE];
} drc_huff_stats_t;

// hybrid bintree/linkedlist structure for Huffman tree
typedef struct drc_huff_node
{
  uint8_t byte_val;
  uint32_t byte_dens;
  struct drc_huff_node *left;
  struct drc_huff_node *right;
  struct drc_huff_node *next;
} drc_huff_node_t;

drc_huff_stats_t *drc_huff_stats_calc(uint8_t *input, uint32_t size);
void              drc_huff_stats_destroy(drc_huff_stats_t *stats);
void              drc_huff_bt_construct(drc_huff_node_t **root, drc_huff_stats_t *stats);
void              drc_huff_bt_destroy(drc_huff_node_t *root);
void              drc_huff_bt_print(drc_huff_node_t *root);
void              drc_huff_ll_print(drc_huff_node_t *root);

#endif
