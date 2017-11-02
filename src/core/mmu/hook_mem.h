#ifndef HOOK_MEM_H
#define HOOK_MEM_H
#include <stdlib.h>
#include <stdint.h>


char *socket_path;

typedef struct {
  enum {
    CONNECT,
    GET_MEM,
    SET_MEM,
    DMA_MEM,
    CONNECT_RESP,
    GET_MEM_RESP,
    SET_MEM_RESP,
    DMA_MEM_RESP
  } type;
  uint16_t addr;
  uint16_t len;
  uint8_t value;
} packet;

char hook_get_mem(uint16_t addr);
char hook_set_mem(uint16_t addr);
char hook_dma(uint16_t addr,uint16_t len);

uint8_t do_hook_get_mem(uint16_t addr);
void do_hook_set_mem(uint16_t addr,uint8_t value);
void do_hook_dma(uint16_t addr,uint16_t len,uint8_t *target);

#endif