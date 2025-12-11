#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "comp.h"

uint8_t *test_str = "Toady, zlap mi te gumisieeeeeeeeeeeeeeeeeeeeeee";

int main(int argc, char* argv[])
{
  comp_compress(test_str, strlen(test_str));

  return 0;
}
