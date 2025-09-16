#include <stdio.h>
#include <unistd.h>

void *memory_pool;

void init_memory_pool() { memory_pool = sbrk(0); }

void reset_memory_pool() { brk(memory_pool); }
void *myallocate(int size) {
  void *ptr = sbrk(size);
  if (ptr == (void *)-1) {
    return NULL;
  }
  return ptr;
}
int main() {
  init_memory_pool();

  int *array = (int *)myallocate(100 * sizeof(int));
  if (array == NULL) {
    printf("Erro na alocação de memória.\n");
    return 1;
  }

  for (int i = 0; i < 100; i++) {
    array[i] = i;
    printf("array[%d] = %d\n", i, array[i]);
  }

  reset_memory_pool();

  return 0;
}