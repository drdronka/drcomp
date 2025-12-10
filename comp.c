#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "qsort.h"
#include "bt.h"

typedef struct comp_data
{
  uint8_t *raw;
  uint32_t size_bits;
} comp_data_t;

comp_data_t *comp_run(uint8_t *input, uint32_t size) 
{ 
  uint8_t byte_dens[256] = {0};
  uint8_t byte_val[256] = {0};
  comp_data_t *comp_data = (comp_data_t*)malloc(sizeof(comp_data_t));
 
  printf("input[%s] size[%u]\n", input, size);  
 
  for(uint32_t n = 0; n < 0256; n++) 
    byte_val[n] = n;

  for(uint32_t n = 0; n < size; n++) 
    byte_dens[input[n]]++;

  qsort_touple(byte_dens, byte_val, 0256);

  for(uint32_t n = 0; n < 256; n++) 
    if(byte_dens[n]) 
      printf("%c : %u\n", byte_val[n], byte_dens[n]);

  return comp_data;
}

uint8_t *test_str = "Toady, zlap mi te gumisie";

void main()
{
  comp_run(test_str, strlen(test_str));
}
