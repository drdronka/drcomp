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
  //drc_core_file_compress(test_path_in, test_path_comp);
  //drc_core_file_decompress(test_path_comp, test_path_decomp);
 
  if(argc > 3)
  {
    drc_core_file_compress(argv[1], argv[2]);
    drc_core_file_decompress(argv[2], argv[3]);
  }
  else
  {
    DRC_LOG_ERROR("need args\n");
  }

  return 0;
}
