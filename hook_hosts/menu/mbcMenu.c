#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#include "../common.h"

uint8_t rom[0x8001];

typedef struct{
  char text[19];
  void (*action)();
} menu_item;

typedef struct{
  char title[17];
  menu_item* items[16];
} menu_page;

typedef struct _menu_state{
  menu_page *cur_page;
  int selection;
  struct _menu_state *prev_state;
} menu_state;

menu_state* STATE;

menu_page MENU[3];

uint8_t menu_buffer[1025];

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
  strncpy(a->text,"option A",18);
  a->action = &a_action;
  menu_item *b=malloc(sizeof(menu_item));
  strncpy(b->text,"option B",18);
  b->action = &b_action;
  strncpy(MENU[0].title,"MENU",16);
  MENU[0].items[0] = a;
  MENU[0].items[1] = b;
  MENU[0].items[2] = NULL;

  strncpy(MENU[1].title, "MENU A",16);
  MENU[1].items[0] = b;
  MENU[1].items[1] = NULL;

  menu_item *back=malloc(sizeof(menu_item));
  strncpy(back->text,"back",18);
  back->action = &menu_back;

  strncpy(MENU[2].title,"MENU B",16);
  MENU[2].items[0] = back;
  MENU[2].items[1] = NULL;

  STATE=NULL;
  menu_to(&MENU[0]);

}

void redraw_buffer(){
  memset(menu_buffer,' ',1024);
  int l=strlen(STATE->cur_page->title);
  memcpy(menu_buffer+((20-l)/2),STATE->cur_page->title,l);

  for (int i = 0; i < 16; ++i)
  {
    if (STATE->cur_page->items[i]==NULL)
      break;
    if (STATE->selection==i)
      *(menu_buffer + 64 + i*32) = 0x10; // right arrow
    memcpy(menu_buffer + 64 + i*32 + 2, STATE->cur_page->items[i]->text,strlen(STATE->cur_page->items[i]->text));
  }
}

void joypad_event(uint8_t value){
  int max_sel;
  for(max_sel = 0; STATE->cur_page->items[max_sel+1] != NULL; ++max_sel);
  if(! (value&(1<<4))){ // directional
    if(! (value&(1<<3))){ // down
      STATE->selection++;
    }else if(! (value&(1<<2))){ // up
      STATE->selection--;
    }
    if(STATE->selection<0)
      STATE->selection=max_sel;
    if(STATE->selection>max_sel)
      STATE->selection=0;
  }else if(! (value&(1<<5))){ // buttons
    if(! (value&(1<<0))){ // A
      STATE->cur_page->items[STATE->selection]->action();
    }else if(! (value&(1<<1))){ // B
      menu_back();
    }

  }
  redraw_buffer();
}

int host_init(int argc,char* argv[]){
  init_menu();
  redraw_buffer();
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
    return menu_buffer[addr-0xA000];
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
    if(addr == 0xA001)
      joypad_event(value);
    //printf("RAM W %.4X %.2X\n",addr, value);
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
