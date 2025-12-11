#include <stdint.h>

// hybrid bintree/linkedlist structure for Huffman tree
typedef struct hf_node
{
  uint8_t char_val;
  uint32_t char_dens;
  struct hf_node *left;
  struct hf_node *right;
  struct hf_node *next;
} hf_node_t;

typedef struct hf_prefix_tab
{
  uint8_t *prefix[256];
  uint32_t size[256];
} hf_prefix_tab_t;

hf_node_t *hf_node_create(uint8_t char_val, uint32_t char_dens);
void hf_node_destroy(hf_node_t *node);
void hf_node_ll_insert(hf_node_t **root, hf_node_t *new_node);
hf_node_t *hf_node_ll_get_first(hf_node_t **root);
void hf_node_ll_print(hf_node_t *root);
hf_node_t *hf_node_bt_merge(hf_node_t *node0, hf_node_t *node1);
void hf_node_bt_print(hf_node_t *root);

void hf_fill_prefix_tab(
  hf_node_t *root, 
  hf_prefix_tab_t *tab,
  uint8_t *curr_prefix,
  uint32_t curr_prefix_size);
