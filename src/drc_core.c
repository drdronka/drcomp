#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "drc_core.h"
#include "drc_huff.h"
#include "drc_log.h"

#define COMP_READ_BLOCK_SIZE 4096
#define COMP_WRITE_BLOCK_SIZE 4096
#define BIT_ARRAY_SIZE 1024
#define BIT_ARRAY_MARGIN 8

typedef struct bit_array
{
  uint8_t data[BIT_ARRAY_SIZE];
  uint32_t size_bits;
  uint32_t size_bytes;
} bit_array_t;

static void bit_array_add(bit_array_t *array, uint8_t *code, uint8_t code_size);
static void bit_array_truncate(bit_array_t *array);
static void bit_array_finish(bit_array_t *array);
static void bit_array_print(bit_array_t *array);
static void encode_and_write(FILE* file_in, FILE* file_out, drc_huff_tab_t *tab);
static void decode_and_write(
  FILE* file_in, FILE* file_out, drc_huff_node_t *root, uint32_t size, uint8_t remainder_bits);

/// LOCAL FUNC ///

static inline void bit_array_add(bit_array_t *array, uint8_t *code, uint8_t code_size)
{
#if DRC_LOG_DEBUG_EN
  DRC_LOG("bit_array_add: ");
  drc_huff_code_print(code, code_size, 1);
#endif

  for(uint32_t bit_n = 0; bit_n < code_size; bit_n++)
  {
    array->data[array->size_bytes] |= code[bit_n] << (array->size_bits % 8);
    array->size_bits++;
    if(array->size_bits % 8 == 0)
    {
      array->size_bytes++;
    }
  }
}

static void bit_array_truncate(bit_array_t *array)
{
  DRC_LOG_DEBUG("bit_array_truncate: size_bytes[%u] size_bits[%u] remainder[%u]\n", 
    array->size_bytes, array->size_bits, array->size_bits % 8);

  uint8_t reminder = array->data[array->size_bytes];
  memset(array->data, 0, sizeof(array->data));
  if(array->size_bits % 8)
  {
    array->data[0] = reminder;
  }
  array->size_bits = array->size_bits % 8;
  array->size_bytes = 0;
}

static void bit_array_finish(bit_array_t *array)
{
  DRC_LOG_DEBUG("bit_array_finish: size_bytes[%u] size_bits[%u] remainder[%u]\n",
    array->size_bytes, array->size_bits, array->size_bits % 8);

    array->data[array->size_bytes + 1] = array->size_bits % 8;
    array->size_bytes += 2;
}

static void bit_array_print(bit_array_t *array)
{
  for(uint32_t nd = 0; nd < array->size_bytes; nd++)
  {
    for(uint32_t ni = 0; ni < 8; ni++)
    {
    DRC_LOG("%c", ((array->data[nd] >> ni) & 1) + '0');
    }
    DRC_LOG(" ");
  }
  DRC_LOG("\n");
}

static void encode_and_write(FILE* file_in, FILE* file_out, drc_huff_tab_t *tab)
{
  DRC_LOG_INFO("encoding data\n");

  fseek(file_in, 0L, SEEK_SET);

  bit_array_t array = {0};
  uint32_t size;
  do
  {
    uint8_t buf_in[COMP_READ_BLOCK_SIZE];

    size = fread(buf_in, 1, COMP_READ_BLOCK_SIZE, file_in);
    DRC_LOG_DEBUG("bytes read: [%u]\n", size);

    if(size)
    {
      for(uint32_t n = 0; n < size; n++)
      {
        bit_array_add(&array, tab->code[buf_in[n]], tab->size[buf_in[n]]);
        if(array.size_bytes > (BIT_ARRAY_SIZE - BIT_ARRAY_MARGIN))
        {
#if DRC_LOG_DEBUG_EN
          DRC_LOG_DEBUG("compressed data write: ");
          bit_array_print(&array);
#endif  
          fwrite(array.data, array.size_bytes, 1, file_out);
          bit_array_truncate(&array);
        }
      }
    }
    else
    {
      bit_array_finish(&array);
#if DRC_LOG_DEBUG_EN
          DRC_LOG_DEBUG("compressed data write: ");
          bit_array_print(&array);
#endif
      fwrite(array.data, array.size_bytes, 1, file_out);
    }
  } while(size);
}

static void decode_and_write(
  FILE* file_in, FILE* file_out, drc_huff_node_t *root, uint32_t data_size, uint8_t remainder_bits)
{
  DRC_LOG_INFO("decoding data\n");

  uint8_t buf_out[COMP_WRITE_BLOCK_SIZE];
  uint32_t buf_out_idx = 0;
  uint32_t read_total = 0;
  uint32_t read_size;
  uint8_t bit_id;
  drc_huff_node_t *node = root;

  do
  {
    uint8_t buf_in[COMP_READ_BLOCK_SIZE];
    read_size = fread(buf_in, 1, COMP_READ_BLOCK_SIZE, file_in);
    DRC_LOG_DEBUG("bytes read: [%u]\n", read_size);

    for(uint32_t n = 0; n < read_size; n++)
    {
      DRC_LOG_DEBUG("coded[%0.2x]: ", buf_in[n]);

      uint8_t bit_limit = 8;
      if(read_total + n == data_size - 1)
      {
        bit_limit = remainder_bits;
      }

      for(uint8_t i = 0; i < bit_limit; i++)
      {
        uint8_t buf_bit = (buf_in[n] >> i) & 1;
        node = buf_bit == 0 ? node->left : node->right;

        if(node->val_node)
        {
          DRC_LOG_DEBUG("decoded[%0.2x] ", node->byte_val);
          buf_out[buf_out_idx] = node->byte_val;
          buf_out_idx++;
          node = root;
        }

        if(buf_out_idx == COMP_READ_BLOCK_SIZE)
        {
          fwrite(buf_out, 1, buf_out_idx, file_out);
          buf_out_idx = 0;
        }
      }

      if(read_total + n == data_size - 1)
      {
        break;
      }
      DRC_LOG_DEBUG("\n");
    }

    read_total += read_size;
  } while(read_size);

  if(buf_out_idx)
  {
    fwrite(buf_out, 1, buf_out_idx, file_out);
  }
}

/// GLOBAL FUNC ///

void drc_core_compress(uint8_t *path_in, uint8_t *path_out)
{
  DRC_LOG_INFO("compress: input[%s] output[%s]\n", path_in, path_out);

  FILE* file_in = fopen(path_in, "rb");
  FILE* file_out = fopen(path_out, "wb");

  if(!file_in || !file_out)
  {
    DRC_LOG_ERROR("failed to open file: %s\n", !file_in ? path_in : path_out);
    return;
  }
  
  drc_huff_stats_t *stats = drc_huff_stats_calc_from_file(file_in);
  drc_huff_tab_t *tab = drc_huff_tab_calc(stats);

#if DRC_LOG_DEBUG_EN
  drc_huff_stats_print(stats);
  drc_huff_tab_print(tab);
#endif

  drc_huff_stats_write(file_out, stats);
  encode_and_write(file_in, file_out, tab);

  drc_huff_tab_destroy(tab);
  drc_huff_stats_destroy(stats);

  fclose(file_out);
  fclose(file_in);

  DRC_LOG_INFO("finished\n");
}

void drc_core_decompress(uint8_t *path_in, uint8_t *path_out)
{
  DRC_LOG_INFO("decompress: input[%s] output[%s]\n", path_in, path_out);

  FILE* file_in = fopen(path_in, "rb");
  FILE* file_out = fopen(path_out, "wb");

  if(!file_in || !file_out)
  {
    DRC_LOG_ERROR("failed to open file: %s\n", !file_in ? path_in : path_out);
    return;
  }

  uint32_t coding_tab_size;  
  uint32_t data_size;
  uint8_t remainder_bits;

  drc_huff_stats_t *stats = drc_huff_stats_read(file_in);
  coding_tab_size = sizeof(stats->weight[0]) * BYTE_RANGE;

  fseek(file_in, -1, SEEK_END);
  data_size = ftell(file_in) - coding_tab_size;

  fread(&remainder_bits, 1, 1, file_in);

  DRC_LOG_INFO("coding_tab_size[%u] data_size[%u] reminder_bits[%u]\n",
    coding_tab_size, data_size, remainder_bits);

 #if DRC_LOG_DEBUG_EN
  drc_huff_stats_print(stats);
#endif

  drc_huff_node_t *root = NULL;
  drc_huff_bt_construct(&root, stats);

  fseek(file_in, coding_tab_size, SEEK_SET);
  decode_and_write(file_in, file_out, root, data_size, remainder_bits);

  drc_huff_bt_destroy(root);
  drc_huff_stats_destroy(stats);

  fclose(file_out);
  fclose(file_in);

  DRC_LOG_INFO("finished\n");
}
