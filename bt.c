#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "bt.h"

bt_node_t *bt_node_create(uint8_t data)
{
  bt_node_t *node = (bt_node_t*)malloc(sizeof(bt_node_t));
  memset(node, 0, sizeof(bt_node_t));
  node->data = data;
  return node;
}

void bt_node_destroy(bt_node_t *node)
{
  free(node);
}
