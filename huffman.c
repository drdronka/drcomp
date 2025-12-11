#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "huffman.h"

hf_node_t *hf_node_create(uint8_t byte_val, uint32_t byte_dens)
{
  hf_node_t *node = (hf_node_t*)malloc(sizeof(hf_node_t));
  memset(node, 0, sizeof(hf_node_t));
  node->byte_val = byte_val;
  node->byte_dens = byte_dens;
  return node;
}

void hf_node_destroy(hf_node_t *node)
{
  free(node);
}

// insert sorted by density, lowest to highest
void hf_node_ll_insert(hf_node_t **root, hf_node_t *new_node)
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

// get element with lowest density, returns NULL if ll empty
hf_node_t *hf_node_ll_get_first(hf_node_t **root)
{
  hf_node_t *node = *root;
  if(node)
    *root = node->next;
  return node;
}

void hf_node_ll_print(hf_node_t *root)
{
  while(root)
  {
    printf(
      "hf_node: char[%c][0x%0.2x] dens[%u]\n", 
      root->byte_val, root->byte_val, root->byte_dens);
    root = root->next;
  }
}

hf_node_t *hf_node_bt_merge(hf_node_t *node0, hf_node_t *node1)
{
  hf_node_t *new_node = hf_node_create(0, node0->byte_dens + node1->byte_dens);
  new_node->left = node0;
  new_node->right = node1;
  return new_node;
}

void hf_node_bt_print(hf_node_t *root)
{
  if(root)
  {
    if(root->left)
      hf_node_bt_print(root->left);

    if(!(root->left) && !(root->right))
    {
      printf("bt node [%c][%u]\n", root->byte_val, root->byte_dens);
    }

    if(root->right)
      hf_node_bt_print(root->right);
  }
}

void hf_fill_prefix_tab(
  hf_node_t *root, 
  hf_prefix_tab_t *tab,
  uint8_t *curr_prefix,
  uint32_t curr_prefix_size)
{
  if(curr_prefix_size == 0)
  {
    memset(tab, 0, sizeof(hf_prefix_tab_t));
  }

  if(root->left)
  { 
    uint8_t *left_prefix = (uint8_t*)malloc(sizeof(uint8_t) * (curr_prefix_size + 1));
    for(uint32_t n = 0; n < curr_prefix_size; n++)
      left_prefix[n] = curr_prefix[n];
    left_prefix[curr_prefix_size] = 0;
    hf_fill_prefix_tab(root->left, tab, left_prefix, curr_prefix_size + 1);
    free(left_prefix);
  }

  if(root->right)
  { 
    uint8_t *right_prefix = (uint8_t*)malloc(sizeof(uint8_t) * (curr_prefix_size + 1));
    for(uint32_t n = 0; n < curr_prefix_size; n++)
      right_prefix[n] = curr_prefix[n];
    right_prefix[curr_prefix_size] = 1;
    hf_fill_prefix_tab(root->right, tab, right_prefix, curr_prefix_size + 1);
    free(right_prefix);
  }
  
  if(!(root->left) && !(root->right))
  {
    tab->prefix[root->byte_val] = (uint8_t*)malloc(sizeof(uint8_t) * curr_prefix_size);
    for(uint32_t n = 0; n < curr_prefix_size; n++)
      tab->prefix[root->byte_val][n] = curr_prefix[n];
    tab->size[root->byte_val] = curr_prefix_size;
  }
}
