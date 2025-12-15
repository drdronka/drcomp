#ifndef DRC_CORE_H
#define DRC_CORE_H

#include <stdint.h>

#include "drc_huff.h"

void drc_core_file_compress(uint8_t *path_in, uint8_t *path_out);
void drc_core_file_decompress(uint8_t *path_in, uint8_t *path_out);
//void*drc_core_compress(uint8_t *input, uint32_t size);

#endif
