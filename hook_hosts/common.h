#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>

const char *socket_path;

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


// to be defined by controllers:
int host_init(int argc,char* argv[]);
uint8_t host_read(int host,uint16_t addr);
void host_write(int host,uint16_t addr, uint8_t value);
void host_deinit(int host);

#endif
