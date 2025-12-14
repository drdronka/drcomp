#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "drc_core.h"
#include "drc_log.h"

uint8_t *test_str = "Toady, zlap mi te gumisieeeeeeeeeeeeeeeeeeeeeee";
uint8_t *test_path_in = "./data_in";
uint8_t *test_path_comp = "./data_comp";
uint8_t *test_path_decomp = "./data_decomp";

int main(int argc, char* argv[])
{
  drc_core_pack_t *pack;

  drc_core_file_compress(test_path_in, test_path_comp);
  drc_core_file_decompress(test_path_comp, test_path_decomp);
  
  //printf("test string:\n[%s]\nsize: %u\n", test_str, strlen(test_str));

  //pack = drc_core_compress(test_str, strlen(test_str));

  //printf("compressed data (hex):\n");
  //for(uint32_t n = 0; n < pack->size; n++)
  //{
  //  printf("%0.2X", pack->data[n]);
  //}
  //printf("\nsize bytes: %u bits: %u padding: %u", pack->size, pack->size_bits, (pack->size * 8) - pack->bits);

  //drc_core_pack_destroy(pack);

  return 0;
}
