#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#include "../common.h"

uint8_t rom[0x8001];

typedef struct{
  char text[20];
  void (*action)();
} menu_item;

typedef struct{
  char title[20];
  menu_item* items[3];
} menu_page;

typedef struct _menu_state{
  menu_page *cur_page;
  int selection;
  struct _menu_state *prev_state;
} menu_state;

menu_state* STATE;

menu_page MENU[3];

void menu_back(){
  if(STATE->prev_state==NULL)
    return;
  menu_state* old_state=STATE;
  STATE=STATE->prev_state;
  free(old_state);
}

void menu_to(menu_page* page){
  menu_state* s=malloc(sizeof(menu_state));
  s->cur_page=page;
  s->selection=0;
  s->prev_state=STATE;
  STATE=s;
}

void a_action(){
  menu_to(&MENU[1]);
}

void b_action(){
  menu_to(&MENU[2]);
}

void init_menu(){
  menu_item *a=malloc(sizeof(menu_item));
  strncpy(a->text,"option A",19);
  a->action = &a_action;
  menu_item *b=malloc(sizeof(menu_item));
  strncpy(b->text,"option B",19);
  b->action = &b_action;
  strncpy(MENU[0].title,"MENU",19);
  MENU[0].items[0] = a;
  MENU[0].items[1] = b;
  MENU[0].items[2] = NULL;

  strncpy(MENU[1].title, "MENU A",19);
  MENU[1].items[0] = b;
  MENU[1].items[1] = NULL;

  menu_item *back=malloc(sizeof(menu_item));
  strncpy(back->text,"back",19);
  back->action = &menu_back;

  strncpy(MENU[2].title,"MENU B",19);
  MENU[2].items[0] = back;
  MENU[2].items[1] = NULL;

  STATE=NULL;
  menu_to(&MENU[0]);

};

int host_init(int argc,char* argv[]){
  init_menu();
  char* dir = dirname(argv[0]);
  char* filename = "/menu.gb";
  char* file = malloc(sizeof(char)*(strlen(dir)+strlen(filename)+1));
  strcpy(file,dir);
  strcat(file,filename);
  #ifdef DEBUG
    printf("init with %s\n",file);
  #endif
  FILE* fp=fopen(file,"r");
  fread(rom,sizeof(uint8_t),0x8000,fp);
  fclose(fp);
  free(file);
  return 1;
}

uint8_t host_read(int host,uint16_t addr){
  if(addr >= 0xA000 && addr <0xC000){
    //printf("RAM R %.4X\n",addr);
    return addr%0xff;
  }else{
    uint8_t value=rom[addr];
    #ifdef DEBUG
      printf("R %.4X %.2X\n", addr,value);
    #endif
    return value;
  }
}

void host_write(int host,uint16_t addr, uint8_t value){
  if(addr >= 0xA000 && addr <0xC000){
    printf("RAM W %.4X %.2X\n",addr, value);
    return; // todo
  }else{
    #ifdef DEBUG
      printf("W %.4X %.2X ignored\n", addr,value);
    #endif
    return; // ignored
  }
}

void host_deinit(int host){
  #ifdef DEBUG
    printf("deinit\n");
  #endif
  return; // ignored
}
