#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "comp.h"
#include "qsort.h"
#include "huffman.h"

#define LOG_DETAILED   1
#define LOG_BASIC      0 || LOG_DETAILED

static void comp_prep_stats(
  uint8_t *input, 
  uint32_t size, 
  uint8_t *byte_val, 
  uint8_t *byte_dens)
{
  for(uint32_t n = 0; n < BYTE_RANGE; n++) 
    byte_val[n] = n;

  memset(byte_dens, 0, sizeof(uint8_t) * BYTE_RANGE);
  for(uint32_t n = 0; n < size; n++) 
    byte_dens[input[n]]++;

  qsort_touple(byte_dens, byte_val, BYTE_RANGE);
}

comp_data_t *comp_compress(uint8_t *input, uint32_t size) 
{ 
#if LOG_BASIC 
  printf("input[%s] size[%u]\n", input, size);  
#endif

  uint8_t byte_val[BYTE_RANGE];
  uint8_t byte_dens[BYTE_RANGE];
  comp_prep_stats(input, size, byte_val, byte_dens);

#if LOG_BASIC
  printf("character density:\n[char][hex][dens]\n");
  for(uint32_t n = 0; n < 256; n++) 
    if(byte_dens[n]) 
      printf("[%c][[%0.2x][%u]\n", byte_val[n], byte_val[n], byte_dens[n]);
#endif

  hf_node_t *hf_root = NULL;
  
  for(uint32_t n = 0; n < 256; n++)
    if(byte_dens[n])
      hf_node_ll_insert(&hf_root, hf_node_create(byte_val[n], byte_dens[n]));

  hf_node_t *node0 = hf_node_ll_get_first(&hf_root);
  hf_node_t *node1 = hf_node_ll_get_first(&hf_root);

#if LOG_DETAILED
  printf("merging nodes:\n[char][hex][dens]\n");
#endif

  while(node1)
  {
#if LOG_DETAILED
    printf("[%c][%0.2x][%u] - [%c][%0.2x][%u]\n", 
      node0->byte_val, node0->byte_val, node0->byte_dens,
      node1->byte_val, node1->byte_val, node1->byte_dens);
#endif

    hf_node_t *new_node = hf_node_bt_merge(node0, node1);
    hf_node_ll_insert(&hf_root, new_node);
    
    node0 = hf_node_ll_get_first(&hf_root);
    node1 = hf_node_ll_get_first(&hf_root);
  }
  hf_node_ll_insert(&hf_root, node0);

  hf_prefix_tab_t prefix_tab;
  hf_fill_prefix_tab(hf_root, &prefix_tab, NULL, 0);

  uint8_t tmp_data[1000];
  uint32_t tmp_data_id = 0;
  for(uint32_t n = 0; n < size; n++)
    for(uint32_t i = 0; i < prefix_tab.size[input[n]]; i++)
      tmp_data[tmp_data_id++] = prefix_tab.prefix[input[n]][i];

#if LOG_BASIC || LOG_DETAILED
  printf("compressed data:\n");
  for(uint32_t n = 0; n < tmp_data_id; n++)
    printf("%c", tmp_data[n] + '0');
  printf(
    "\ncompression ratio %0.2f\n", 
    (float)((size * 8) - tmp_data_id) / (float)(size * 8));
#endif

  comp_data_t *comp_data = (comp_data_t*)malloc(sizeof(comp_data_t));

  return comp_data;
}
