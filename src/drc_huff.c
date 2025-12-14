#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "drc_huff.h"
#include "drc_log.h"

#define READ_BLOCK_SIZE 4096

#define FILTER_SPEC_CHARS(x) (((x) == 0x0A || (x) == 0x0D) ? '\\' : (x))

static void ll_insert(drc_huff_node_t **root, drc_huff_node_t *new_node);
static drc_huff_node_t *ll_get_first(drc_huff_node_t **root);
static void ll_print(drc_huff_node_t *root);
static drc_huff_node_t *bt_merge(drc_huff_node_t *node0, drc_huff_node_t *node1);
static void bt_print(drc_huff_node_t *root);
static void tab_fill(
  drc_huff_node_t *root, 
  drc_huff_tab_t *tab, 
  uint8_t *curr_code, 
  uint32_t curr_code_size);

/// LOCAL FUNC ///

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
  drc_huff_node_t *new_node = drc_huff_node_create(0, node0->byte_weight + node1->byte_weight);
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

///  GLOBAL FUNC ///

void drc_huff_code_print(uint8_t *code, uint8_t size, uint8_t newline)
{
  for(uint32_t n = 0; n < size; n++)
  {
    DRC_LOG("%c", code[n] + '0');
  }
  if(newline)
  {
    DRC_LOG("\n");
  }
}

drc_huff_node_t *drc_huff_node_create(uint8_t byte_val, uint32_t byte_weight)
{
  drc_huff_node_t *node = (drc_huff_node_t*)malloc(sizeof(drc_huff_node_t));
  memset(node, 0, sizeof(drc_huff_node_t));
  node->byte_val = byte_val;
  node->byte_weight = byte_weight;
  return node;
}

void drc_huff_node_destroy(drc_huff_node_t *node)
{
  free(node);
}

drc_huff_stats_t *drc_huff_stats_calc(uint8_t *input, uint32_t size)
{
  DRC_LOG_INFO("calculating character stats\n");

  drc_huff_stats_t *stats = (drc_huff_stats_t*)malloc(sizeof(drc_huff_stats_t));
  memset(stats, 0, sizeof(drc_huff_stats_t));

  for(uint32_t n = 0; n < size; n++)
  {
    stats->weight[input[n]]++;
  }

  return stats;
}

drc_huff_stats_t *drc_huff_stats_calc_from_file(FILE* file_in)
{
  DRC_LOG_INFO("calculating character stats\n");

  drc_huff_stats_t *stats = (drc_huff_stats_t*)malloc(sizeof(drc_huff_stats_t));
  memset(stats, 0, sizeof(drc_huff_stats_t));

  fseek(file_in, 0, SEEK_SET);

  uint32_t size = 0;
  do
  {
    char buf[READ_BLOCK_SIZE];
    size = fread(buf, 1, READ_BLOCK_SIZE, file_in);
    DRC_LOG_DEBUG("read block: size[%u]\n", size);
    
    for(uint32_t n = 0; n < size; n++)
    {
      stats->weight[buf[n]]++;  
    }
  }
  while(size);
  
  return stats;
}

void drc_huff_stats_write(FILE* file_in, drc_huff_stats_t *stats)
{
  DRC_LOG_INFO("writing character stats to file\n");

  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    uint8_t weight_size = sizeof(stats->weight[0]);
    uint8_t buf[sizeof(stats->weight[0])];

    for(uint8_t i = 0; i < weight_size; i++)
    {
      buf[weight_size - i - 1] = (uint8_t)((stats->weight[n] >> (i * 8)) & 0xFF);
    }

    fwrite(buf, weight_size, 1, file_in);
  }
}

drc_huff_stats_t *drc_huff_stats_read(FILE* file_in)
{
  DRC_LOG_INFO("reading character stats from file\n");
  
  drc_huff_stats_t *stats = (drc_huff_stats_t*)malloc(sizeof(drc_huff_stats_t));
  memset(stats, 0, sizeof(drc_huff_stats_t));

  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    uint8_t weight_size = sizeof(stats->weight[0]);
    uint8_t buf[sizeof(stats->weight[0])];

    fread(buf, weight_size, 1, file_in);

    for(uint8_t i = 0; i < weight_size; i++)
    {
      stats->weight[n] |= ((uint32_t)buf[weight_size - i - 1]) << (i * 8);
    }
  }

  return stats;
}

void drc_huff_stats_destroy(drc_huff_stats_t *stats)
{
  free(stats);
}

void drc_huff_stats_print(drc_huff_stats_t *stats)
{
  DRC_LOG("character stats:\n");
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    if(stats->weight[n])
    {
      DRC_LOG("char[%c][0x%0.2x] weight[%u]\n", FILTER_SPEC_CHARS(n), n, stats->weight[n]);
    }
  }
}

drc_huff_tab_t *drc_huff_tab_calc(drc_huff_stats_t *stats)
{
  DRC_LOG_INFO("calculating coding table\n");
  
  drc_huff_node_t *root = NULL;
  drc_huff_bt_construct(&root, stats);

  drc_huff_tab_t *tab = (drc_huff_tab_t*)malloc(sizeof(drc_huff_tab_t));
  memset(tab, 0, sizeof(drc_huff_tab_t));
  tab_fill(root, tab, NULL, 0);

  drc_huff_bt_destroy(root);

  return tab;
}

void drc_huff_tab_print(drc_huff_tab_t *tab)
{
  DRC_LOG("coding table:\n");
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    if(tab->size[n])
    {
      DRC_LOG("char[%c][0x%0.2x]: ", FILTER_SPEC_CHARS(n), n);
      drc_huff_code_print(tab->code[n], tab->size[n], 1);
    }
  }
}

void drc_huff_tab_destroy(drc_huff_tab_t *tab)
{
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    if(tab->size[n])
    {
      free(tab->code[n]);
    }
  }
  free(tab);
}

void drc_huff_bt_construct(drc_huff_node_t **root, drc_huff_stats_t *stats)
{
  DRC_LOG_INFO("constructing Huffman tree\n");

  DRC_LOG_DEBUG("adding nodes:\n");
  for(uint32_t n = 0; n < BYTE_RANGE; n++)
  {
    if(stats->weight[n])
    {
      DRC_LOG_DEBUG("char[%c][%u] weight[%u]\n", FILTER_SPEC_CHARS(n), n, stats->weight[n]);
      ll_insert(root, drc_huff_node_create(n, stats->weight[n]));
    }
  }

  drc_huff_node_t *node0 = ll_get_first(root);
  drc_huff_node_t *node1 = ll_get_first(root);

  DRC_LOG_DEBUG("merging nodes:\n");

  while(node1)
  {
    DRC_LOG_DEBUG("[%c][%0.2x][%u] - [%c][%0.2x][%u]\n", 
      FILTER_SPEC_CHARS(node0->byte_val), node0->byte_val, node0->byte_weight,
      FILTER_SPEC_CHARS(node1->byte_val), node1->byte_val, node1->byte_weight);

    drc_huff_node_t *new_node = bt_merge(node0, node1);
    ll_insert(root, new_node);
    
    node0 = ll_get_first(root);
    node1 = ll_get_first(root);
  }

  ll_insert(root, node0);
}

void drc_huff_bt_destroy(drc_huff_node_t *root)
{
  if(root)
  {
    if(root->left)
    {
      drc_huff_bt_destroy(root->left);
    }

    if(root->right)
    {
      drc_huff_bt_destroy(root->right);
    }

    drc_huff_node_destroy(root);
  }
}

void drc_huff_bt_print(drc_huff_node_t *root)
{
  if(root)
  {
    if(root->left)
      drc_huff_bt_print(root->left);

    if(!(root->left) && !(root->right))
    {
      DRC_LOG(
        "huff bt node [%c][0x%0.2x] weight[%u]\n", 
        FILTER_SPEC_CHARS(root->byte_val), root->byte_val, root->byte_weight);
    }

    if(root->right)
      drc_huff_bt_print(root->right);
  }
}

void drc_huff_ll_print(drc_huff_node_t *root)
{
  while(root)
  {
    DRC_LOG(
      "huff ll node [%c][0x%0.2x] weight[%u]\n", 
      FILTER_SPEC_CHARS(root->byte_val), root->byte_val, root->byte_weight);
    root = root->next;
  }
}









