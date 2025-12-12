#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "drc_code.h"
#include "drc_huff.h"
#include "drc_log.h"

static void tab_fill(
  drc_huff_node_t *root, drc_code_tab_t *tab, uint8_t *curr_code, uint32_t curr_code_size);

/// LOCAL FUNC ///

static void tab_fill(
  drc_huff_node_t *root, 
  drc_code_tab_t *tab,
  uint8_t *curr_code,
  uint32_t curr_code_size)
{
  if(root->left)
  { 
    uint8_t *left_code = (uint8_t*)malloc(curr_code_size + 1);
    memcpy(left_code, curr_code, curr_code_size);
    left_code[curr_code_size] = 0;
    tab_fill(root->left, tab, left_code, curr_code_size + 1);
    free(left_code);
  }

  if(root->right)
  { 
    uint8_t *right_code = (uint8_t*)malloc(curr_code_size + 1);
    memcpy(right_code, curr_code, curr_code_size);
    right_code[curr_code_size] = 1;
    tab_fill(root->right, tab, right_code, curr_code_size + 1);
    free(right_code);
  }
  
  if(!(root->left) && !(root->right))
  {
    tab->code[root->byte_val] = (uint8_t*)malloc(curr_code_size);
    memcpy(tab->code[root->byte_val], curr_code, curr_code_size);
    tab->size[root->byte_val] = curr_code_size;
  }
}

/// GLOBAL FUNC ///

drc_code_tab_t *drc_code_tab_calc(drc_huff_stats_t *stats)
{
  drc_huff_node_t *root = NULL;
  drc_huff_bt_construct(&root, stats);

  drc_code_tab_t *tab = (drc_code_tab_t*)malloc(sizeof(drc_code_tab_t));
  memset(tab, 0, sizeof(drc_code_tab_t));
  tab_fill(root, tab, NULL, 0);

  drc_huff_bt_destroy(root);

  return tab;
}

void drc_code_tab_destroy(drc_code_tab_t *tab)
{
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    free(tab->code[n]);
  }
  free(tab);
}

