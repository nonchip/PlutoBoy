#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "common.h"

int main(int argc, char *argv[]) {
  socket_path= "\0de.nonchip.PlutoBoy.hook_mem";
  int hosts[256];
  for (int i = 0; i < sizeof(hosts)/sizeof(int); ++i)
  {
    hosts[i]=-1;
  }
  struct sockaddr_un addr;
  int fd,cl,rc;

  if ( (fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
    unlink(socket_path);
  }

  size_t psize=sizeof(packet);
  if(-1 == setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &psize, sizeof(psize)))
    printf("Error setsockopt");

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  packet p;
  char *buf;
  struct timespec timer_old;
  struct timespec timer;
  int timer_bytes=0;
  clock_gettime(CLOCK_MONOTONIC,&timer_old);
  while (1) {
    if ( (cl = accept(fd, NULL, NULL)) == -1) {
      perror("accept error");
      continue;
    }
    if(hosts[cl] < 0)
      hosts[cl]=host_init(argc,argv);

    while ( (rc =recv(cl,&p,psize,MSG_WAITALL)) > 0) {
      switch(p.type){
        case CONNECT:
          p.type=CONNECT_RESP;
          break;
        case GET_MEM:
          p.type=GET_MEM_RESP;
          p.value=host_read(hosts[cl],p.addr);
          timer_bytes+=1;
          break;
        case DMA_MEM:
          p.type=DMA_MEM_RESP;
          buf=malloc(sizeof(uint8_t)*(p.len+1));
          timer_bytes+=p.len;
          for (int i = 0; i < p.len; ++i)
          {
            buf[i]=host_read(hosts[cl],p.addr+i);
          }
          break;
        case SET_MEM:
          p.type=SET_MEM_RESP;
          timer_bytes+=1;
          host_write(hosts[cl],p.addr,p.value);
          break;
      }
      send(cl,&p,psize,MSG_EOR);
      if (p.type==DMA_MEM_RESP)
      {
        #ifdef DEBUG
          printf("DMA %.4X %.4X\n",p.addr,p.len);
        #endif
        psize=sizeof(buf);
        if(-1 == setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &psize, sizeof(psize)))
          printf("Error setsockopt");
        send(cl,buf,sizeof(uint8_t)*p.len,MSG_EOR);
        psize=sizeof(packet);
        if(-1 == setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &psize, sizeof(psize)))
          printf("Error setsockopt");
        free(buf);
      }
      clock_gettime(CLOCK_MONOTONIC,&timer);
      struct timespec delta;
      if ((timer.tv_nsec-timer_old.tv_nsec)<0) {
        delta.tv_sec = timer.tv_sec-timer_old.tv_sec-1;
        delta.tv_nsec = 1000000000+timer.tv_nsec-timer_old.tv_nsec;
      } else {
        delta.tv_sec = timer.tv_sec-timer_old.tv_sec;
        delta.tv_nsec = timer.tv_nsec-timer_old.tv_nsec;
      }
      if(delta.tv_sec>=5){
        printf("%8d bytes transferred in %2d s = %8d B/s\n",timer_bytes,delta.tv_sec,(timer_bytes/delta.tv_sec));
        timer_bytes=0;
        timer_old=timer;
      }
    }
    if (rc == -1) {
      perror("read");
      exit(-1);
    }
    else if (rc == 0) {
      printf("EOF\n");
      close(cl);
      host_deinit(hosts[cl]);
    }
  }


  return 0;
}