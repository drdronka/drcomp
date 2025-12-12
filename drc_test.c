#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "drc_core.h"

uint8_t *test_str = "Toady, zlap mi te gumisieeeeeeeeeeeeeeeeeeeeeee";

int main(int argc, char* argv[])
{
  drc_core_pack_t *pack;

  //printf("test string:\n[%s]\nsize: %u\n", test_str, strlen(test_str));

  pack = drc_core_compress(test_str, strlen(test_str));

  //printf("compressed data (hex):\n");
  //for(uint32_t n = 0; n < pack->size; n++)
  //{
  //  printf("%0.2X", pack->data[n]);
  //}
  //printf("\nsize bytes: %u bits: %u padding: %u", pack->size, pack->size_bits, (pack->size * 8) - pack->bits);

  drc_core_pack_destroy(pack);

  return 0;
}
