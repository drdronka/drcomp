#include <stdint.h>

typedef struct bt_node
{
  uint8_t data;
  struct bt_node *left;
  struct bt_node *right;
} bt_node_t;

bt_node_t *bt_node_create(uint8_t data);
void bt_node_destroy(bt_node_t *node);

