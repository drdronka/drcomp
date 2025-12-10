#include <stdint.h>

// sorts topuples based on tab0 values
void qsort_touple(uint8_t* tab0, uint8_t* tab1, uint32_t size)
{
  if(size > 1)
  {
    uint32_t mean = tab0[size/2];
    int32_t back = -1;
    int32_t front = size;

    while(back < front)
    {
      while(tab0[++back] < mean);
      while(tab0[--front] > mean);

      if(back < front)
      {
        uint32_t tmp = tab0[back];
        tab0[back] = tab0[front];
        tab0[front] = tmp;
        tmp = tab1[back];
        tab1[back] = tab1[front];
        tab1[front] = tmp;
      }
    }

    qsort_touple(tab0, tab1, back);
    qsort_touple(&tab0[back], &tab1[back], size-back);
  }
}


