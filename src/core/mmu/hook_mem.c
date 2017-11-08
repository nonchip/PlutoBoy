#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include "hook_mem.h"

size_t psize = sizeof(packet);
int socket_fd = -255;
struct sockaddr_un socket_addr;

void connect_socket(){
  socket_path = "\0de.nonchip.PlutoBoy.hook_mem";
  if(socket_fd!=-255)
    return;
  if ( (socket_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&socket_addr, 0, sizeof(socket_addr));
  socket_addr.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *socket_addr.sun_path = '\0';
    strncpy(socket_addr.sun_path+1, socket_path+1, sizeof(socket_addr.sun_path)-2);
  } else {
    strncpy(socket_addr.sun_path, socket_path, sizeof(socket_addr.sun_path)-1);
  }

  if(-1 == setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &psize, sizeof(psize)))
    printf("Error setsockopt");

  if (connect(socket_fd, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) == -1) {
    perror("connect error");
    exit(-1);
  }

  packet p;
  p.type=CONNECT;
  send(socket_fd, &p, psize,MSG_EOR);
  recv(socket_fd, &p, psize,MSG_WAITALL);
  if(p.type != CONNECT_RESP){
    perror("connect handshake error");
    exit(-1);
  }
}

static inline char hook_both_mem(uint16_t addr){
  return addr < 0x8000 // ROM*
    || (addr >= 0xA000 && addr < 0xC000); // RAM*
}

char hook_get_mem(uint16_t addr){
  if(hook_both_mem(addr))
    return 1;
  return 0;
}
uint8_t do_hook_get_mem(uint16_t addr) {
  connect_socket();
  packet p;
  p.type=GET_MEM;
  p.addr=addr;
  send(socket_fd, &p, psize,MSG_EOR);
  recv(socket_fd, &p, psize,MSG_WAITALL);
  if(p.type != GET_MEM_RESP){
    perror("get mem error");
    exit(-1);
  }
  return p.value;
}

char hook_set_mem(uint16_t addr){
  if(hook_both_mem(addr))
    return 1;
  return 0;
}
void do_hook_set_mem(uint16_t addr,uint8_t value) {
  connect_socket();
  packet p;
  p.type=SET_MEM;
  p.addr=addr;
  p.value=value;
  send(socket_fd, &p, sizeof(p),MSG_EOR);
  recv(socket_fd, &p, sizeof(p),MSG_WAITALL);
  if(p.type != SET_MEM_RESP){
    perror("set mem error");
    exit(-1);
  }
  return;
}

char hook_dma(uint16_t addr,uint16_t len){
  for (uint16_t i = addr; i < addr+len; ++i)
  {
    if(hook_get_mem(addr))
      return 1;
  }
  return 0;
}

void do_hook_dma(uint16_t addr,uint16_t len,uint8_t *target){
  connect_socket();
  packet p;
  p.type=DMA_MEM;
  p.addr=addr;
  p.len=len;
  send(socket_fd, &p, psize,MSG_EOR);
  recv(socket_fd, &p, psize,MSG_WAITALL);
  if(p.type != DMA_MEM_RESP){
    perror("dma mem error");
    exit(-1);
  }
  recv(socket_fd,target,sizeof(uint8_t)*p.len,MSG_WAITALL);
}