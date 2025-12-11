#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "qsort.h"
#include "hf.h"

typedef struct comp_data
{
  uint8_t *raw;
  uint32_t size_bits;
} comp_data_t;

comp_data_t *comp_run(uint8_t *input, uint32_t size) 
{ 
  uint8_t char_dens[256] = {0};
  uint8_t char_val[256] = {0};

  printf("input[%s] size[%u]\n", input, size);  
 
  for(uint32_t n = 0; n < 256; n++) 
    char_val[n] = n;

  for(uint32_t n = 0; n < size; n++) 
    char_dens[input[n]]++;

  qsort_touple(char_dens, char_val, 256);

#if 0
  printf("character density:\n");
  for(uint32_t n = 0; n < 256; n++) 
    if(char_dens[n]) 
      printf("[%c][%u]\n", char_val[n], char_dens[n]);
#endif

  hf_node_t *hf_root = NULL;
  
  for(uint32_t n = 0; n < 256; n++)
    if(char_dens[n])
      hf_node_ll_insert(&hf_root, hf_node_create(char_val[n], char_dens[n]));

  //hf_node_ll_print(hf_root);

  hf_node_t *node0 = hf_node_ll_get_first(&hf_root);
  hf_node_t *node1 = hf_node_ll_get_first(&hf_root);

  while(node1)
  {
#if 0
    printf("merging [%c][%u] - [%c][%u]\n", 
      node0->char_val, node0->char_dens,
      node1->char_val, node1->char_dens);
#endif

    hf_node_t *new_node = hf_node_bt_merge(node0, node1);
    hf_node_ll_insert(&hf_root, new_node);
    
    node0 = hf_node_ll_get_first(&hf_root);
    node1 = hf_node_ll_get_first(&hf_root);
  }
  hf_node_ll_insert(&hf_root, node0);

  //hf_node_bt_print(hf_root);

  hf_prefix_tab_t prefix_tab;
  memset(&prefix_tab, 0, sizeof(prefix_tab));

  hf_fill_prefix_tab(hf_root, &prefix_tab, NULL, 0);

  comp_data_t *comp_data = (comp_data_t*)malloc(sizeof(comp_data_t));

  return comp_data;
}

uint8_t *test_str = "Toady, zlap mi te gumisie";

void main()
{
  comp_run(test_str, strlen(test_str));
}
