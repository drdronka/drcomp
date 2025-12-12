#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "drc_code.h"
#include "drc_log.h"

// hybrid bintree/linkedlist structure for Huffman tree
typedef struct hf_node
{
  uint8_t byte_val;
  uint32_t byte_dens;
  struct hf_node *left;
  struct hf_node *right;
  struct hf_node *next;
} hf_node_t;

static hf_node_t *hf_node_create(uint8_t byte_val, uint32_t byte_dens);
static void       hf_node_destroy(hf_node_t *node);

static void       hf_ll_insert(hf_node_t **root, hf_node_t *new_node);
static            hf_node_t *hf_ll_get_first(hf_node_t **root);
static void       hf_ll_print(hf_node_t *root);

static hf_node_t *hf_bt_merge(hf_node_t *node0, hf_node_t *node1);
static void       hf_bt_print(hf_node_t *root);
static void       hf_bt_construct(hf_node_t **root, drc_code_stats_t *stats);
static void       hf_bt_destroy(hf_node_t *root);

static void       hf_code_tab_fill(
  hf_node_t *root, drc_code_tab_t *tab, uint8_t *curr_code, uint32_t curr_code_size);

/// LOCAL FUNC ///

static hf_node_t *hf_node_create(uint8_t byte_val, uint32_t byte_dens)
{
  hf_node_t *node = (hf_node_t*)malloc(sizeof(hf_node_t));
  memset(node, 0, sizeof(hf_node_t));
  node->byte_val = byte_val;
  node->byte_dens = byte_dens;
  return node;
}

static void hf_node_destroy(hf_node_t *node)
{
  free(node);
}

// automatically sorted
static void hf_ll_insert(hf_node_t **root, hf_node_t *new_node)
{
  if(!(*root))
  {
    *root = new_node;
  }
  else if(new_node->byte_dens < (*root)->byte_dens)
  {
    new_node->next = *root;
    *root = new_node;
  }
  else
  {
    hf_node_t *tmp_node = *root;
  
    while(tmp_node->next)
    {
      if(tmp_node->next->byte_dens < new_node->byte_dens)
      {
        tmp_node = tmp_node->next;
      }
      else
      {
        new_node->next = tmp_node->next;
        tmp_node->next = new_node;
        return;
      }
    }

    tmp_node->next = new_node;
  }
}

static hf_node_t *hf_ll_get_first(hf_node_t **root)
{
  hf_node_t *node = *root;
  if(node)
    *root = node->next;
  return node;
}

static void hf_ll_print(hf_node_t *root)
{
  while(root)
  {
    printf(
      "hf_node: char[%c][0x%0.2x] dens[%u]\n", 
      root->byte_val, root->byte_val, root->byte_dens);
    root = root->next;
  }
}

static hf_node_t *hf_bt_merge(hf_node_t *node0, hf_node_t *node1)
{
  hf_node_t *new_node = hf_node_create(0, node0->byte_dens + node1->byte_dens);
  new_node->left = node0;
  new_node->right = node1;
  return new_node;
}

static void hf_bt_print(hf_node_t *root)
{
  if(root)
  {
    if(root->left)
      hf_bt_print(root->left);

    if(!(root->left) && !(root->right))
    {
      printf("bt node [%c][%u]\n", root->byte_val, root->byte_dens);
    }

    if(root->right)
      hf_bt_print(root->right);
  }
}

void hf_bt_construct(hf_node_t **root, drc_code_stats_t *stats)
{
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    if(stats->weight[n])
    {
      hf_ll_insert(root, hf_node_create(n, stats->weight[n]));
    }
  }

  hf_node_t *node0 = hf_ll_get_first(root);
  hf_node_t *node1 = hf_ll_get_first(root);

  DRC_LOG_DEBUG("merging nodes:\n[char][hex][dens]\n");

  while(node1)
  {
    DRC_LOG_DEBUG("[%c][%0.2x][%u] - [%c][%0.2x][%u]\n", 
      node0->byte_val, node0->byte_val, node0->byte_dens,
      node1->byte_val, node1->byte_val, node1->byte_dens);

    hf_node_t *new_node = hf_bt_merge(node0, node1);
    hf_ll_insert(root, new_node);
    
    node0 = hf_ll_get_first(root);
    node1 = hf_ll_get_first(root);
  }

  hf_ll_insert(root, node0);
}

static void hf_bt_destroy(hf_node_t *root)
{
  if(root)
  {
    if(root->left)
    {
      hf_bt_destroy(root->left);
    }

    if(root->right)
    {
      hf_bt_destroy(root->right);
    }

    hf_node_destroy(root);
  }
}

static void hf_code_tab_fill(
  hf_node_t *root, 
  drc_code_tab_t *tab,
  uint8_t *curr_code,
  uint32_t curr_code_size)
{
  if(root->left)
  { 
    uint8_t *left_code = (uint8_t*)malloc(sizeof(uint8_t) * (curr_code_size + 1));
    for(uint32_t n = 0; n < curr_code_size; n++)
    {
      left_code[n] = curr_code[n];
    }
    left_code[curr_code_size] = 0;
    hf_code_tab_fill(root->left, tab, left_code, curr_code_size + 1);
    free(left_code);
  }

  if(root->right)
  { 
    uint8_t *right_code = (uint8_t*)malloc(sizeof(uint8_t) * (curr_code_size + 1));
    for(uint32_t n = 0; n < curr_code_size; n++)
    {
      right_code[n] = curr_code[n];
    }
    right_code[curr_code_size] = 1;
    hf_code_tab_fill(root->right, tab, right_code, curr_code_size + 1);
    free(right_code);
  }
  
  if(!(root->left) && !(root->right))
  {
    tab->code[root->byte_val] = (uint8_t*)malloc(sizeof(uint8_t) * curr_code_size);
    for(uint32_t n = 0; n < curr_code_size; n++)
    {
      tab->code[root->byte_val][n] = curr_code[n];
    }
    tab->size[root->byte_val] = curr_code_size;
  }
}

/// GLOBAL FUNC ///

drc_code_stats_t *drc_code_stats_calc(uint8_t *input, uint32_t size)
{
  drc_code_stats_t *stats = (drc_code_stats_t*)malloc(sizeof(drc_code_stats_t));
  memset(stats, 0, sizeof(drc_code_stats_t));

  for(uint32_t n = 0; n < size; n++)
  {
    stats->weight[input[n]]++;
  }

  return stats;
}

void drc_code_stats_destroy(drc_code_stats_t *stats)
{
  free(stats);
}

drc_code_tab_t *drc_code_tab_calc(drc_code_stats_t *stats)
{
  hf_node_t *root = NULL;
  hf_bt_construct(&root, stats);

  drc_code_tab_t *tab = (drc_code_tab_t*)malloc(sizeof(drc_code_tab_t));
  memset(tab, 0, sizeof(drc_code_tab_t));
  hf_code_tab_fill(root, tab, NULL, 0);

  hf_bt_destroy(root);

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

