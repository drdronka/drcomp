#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "drc_huff.h"
#include "drc_log.h"

static drc_huff_node_t *node_create(uint8_t byte_val, uint32_t byte_weight);
static void node_destroy(drc_huff_node_t *node);
static void ll_insert(drc_huff_node_t **root, drc_huff_node_t *new_node);
static drc_huff_node_t *ll_get_first(drc_huff_node_t **root);
static void ll_print(drc_huff_node_t *root);
static drc_huff_node_t *bt_merge(drc_huff_node_t *node0, drc_huff_node_t *node1);
static void bt_construct(drc_huff_node_t **root, drc_huff_stats_t *stats);
static void bt_destroy(drc_huff_node_t *root);
static void bt_print(drc_huff_node_t *root);
static void tab_fill(
  drc_huff_node_t *root, 
  drc_huff_tab_t *tab, 
  uint8_t *curr_code, 
  uint32_t curr_code_size);

  /// LOCAL FUNC ///

static drc_huff_node_t *node_create(uint8_t byte_val, uint32_t byte_weight)
{
  drc_huff_node_t *node = (drc_huff_node_t*)malloc(sizeof(drc_huff_node_t));
  memset(node, 0, sizeof(drc_huff_node_t));
  node->byte_val = byte_val;
  node->byte_weight = byte_weight;
  return node;
}

static void node_destroy(drc_huff_node_t *node)
{
  free(node);
}

// automatically sorted
static void ll_insert(drc_huff_node_t **root, drc_huff_node_t *new_node)
{
  if(!(*root))
  {
    *root = new_node;
  }
  else if(new_node->byte_weight < (*root)->byte_weight)
  {
    new_node->next = *root;
    *root = new_node;
  }
  else
  {
    drc_huff_node_t *tmp_node = *root;
  
    while(tmp_node->next)
    {
      if(tmp_node->next->byte_weight < new_node->byte_weight)
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

static drc_huff_node_t *ll_get_first(drc_huff_node_t **root)
{
  drc_huff_node_t *node = *root;
  if(node)
    *root = node->next;
  return node;
}

static drc_huff_node_t *bt_merge(drc_huff_node_t *node0, drc_huff_node_t *node1)
{
  drc_huff_node_t *new_node = node_create(0, node0->byte_weight + node1->byte_weight);
  new_node->left = node0;
  new_node->right = node1;
  return new_node;
}

static void tab_fill(
  drc_huff_node_t *root, 
  drc_huff_tab_t *tab,
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

static void bt_construct(drc_huff_node_t **root, drc_huff_stats_t *stats)
{
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    if(stats->weight[n])
    {
      ll_insert(root, node_create(n, stats->weight[n]));
    }
  }

  drc_huff_node_t *node0 = ll_get_first(root);
  drc_huff_node_t *node1 = ll_get_first(root);

  DRC_LOG_DEBUG("merging nodes:\n[char][hex][weight]\n");

  while(node1)
  {
    DRC_LOG_DEBUG("[%c][%0.2x][%u] - [%c][%0.2x][%u]\n", 
      node0->byte_val, node0->byte_val, node0->byte_weight,
      node1->byte_val, node1->byte_val, node1->byte_weight);

    drc_huff_node_t *new_node = bt_merge(node0, node1);
    ll_insert(root, new_node);
    
    node0 = ll_get_first(root);
    node1 = ll_get_first(root);
  }

  ll_insert(root, node0);
}

static void bt_destroy(drc_huff_node_t *root)
{
  if(root)
  {
    if(root->left)
    {
      bt_destroy(root->left);
    }

    if(root->right)
    {
      bt_destroy(root->right);
    }

    node_destroy(root);
  }
}

// GLOBAL FUNC ///

drc_huff_stats_t *drc_huff_stats_calc(uint8_t *input, uint32_t size)
{
  drc_huff_stats_t *stats = (drc_huff_stats_t*)malloc(sizeof(drc_huff_stats_t));
  memset(stats, 0, sizeof(drc_huff_stats_t));

  for(uint32_t n = 0; n < size; n++)
  {
    stats->weight[input[n]]++;
  }

  return stats;
}

void drc_huff_stats_destroy(drc_huff_stats_t *stats)
{
  free(stats);
}

void drc_huff_bt_print(drc_huff_node_t *root)
{
  if(root)
  {
    if(root->left)
      drc_huff_bt_print(root->left);

    if(!(root->left) && !(root->right))
    {
      printf(
        "huff bt node [%c][0x%0.2x] weight[%u]\n", 
        root->byte_val, root->byte_val, root->byte_weight);
    }

    if(root->right)
      drc_huff_bt_print(root->right);
  }
}

void drc_huff_ll_print(drc_huff_node_t *root)
{
  while(root)
  {
    printf(
      "huff ll node [%c][0x%0.2x] weight[%u]\n", 
      root->byte_val, root->byte_val, root->byte_weight);
    root = root->next;
  }
}

drc_huff_tab_t *drc_huff_tab_calc(drc_huff_stats_t *stats)
{
  drc_huff_node_t *root = NULL;
  bt_construct(&root, stats);

  drc_huff_tab_t *tab = (drc_huff_tab_t*)malloc(sizeof(drc_huff_tab_t));
  memset(tab, 0, sizeof(drc_huff_tab_t));
  tab_fill(root, tab, NULL, 0);

  bt_destroy(root);

  return tab;
}

void drc_huff_tab_destroy(drc_huff_tab_t *tab)
{
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    free(tab->code[n]);
  }
  free(tab);
}
