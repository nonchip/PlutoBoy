#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"

uint8_t rom[0x8001];

int host_init(int argc,char* argv[]){
  #ifdef DEBUG
    printf("init with %s\n",argv[1]);
  #endif
  FILE* fp=fopen(argv[1],"r");
  fread(rom,sizeof(uint8_t),0x8000,fp);
  fclose(fp);
  return 1;
}

uint8_t host_read(int host,uint16_t addr){
  uint8_t value=rom[addr];
  #ifdef DEBUG
    printf("R %.4X %.2X\n", addr,value);
  #endif
  return value;
}

void host_write(int host,uint16_t addr, uint8_t value){
  #ifdef DEBUG
    printf("W %.4X %.2X ignored\n", addr,value);
  #endif
  return; // ignored
}

void host_deinit(int host){
  #ifdef DEBUG
    printf("deinit\n");
  #endif
  return; // ignored
}
