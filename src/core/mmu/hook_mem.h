#include <stdio.h>

char hook_get_mem(uint16_t addr){
  return 1;
}
uint8_t do_hook_get_mem(uint16_t addr) {
  printf("get %x\n",addr);
  return 0;
}

char hook_set_mem(uint16_t addr){
  return 1;
}
void do_hook_set_mem(uint16_t addr,uint8_t value) {
  printf("set %x %x\n",addr,value);
  return;
}
