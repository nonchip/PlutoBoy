#ifndef GB_MEM_H
#define GB_MEM_H

#include <stdint.h>
#include <stdlib.h>


/* Directly inject a value into IO memory without performing
 * any checks or operations on the data. Should be used by
 * controllers that have direct access to modifying this memory
 * and not the CPU. */
void io_write_override(uint8_t loc, uint8_t val);

void set_mem_override(uint16_t loc, uint8_t val); 

// Read contents from given 16 bit memory address
uint8_t get_mem(uint16_t const addr);

/*  Write an 8 bit value to the given 16 bit address */
void set_mem(uint16_t const addr, uint8_t const val);

/* Write 16bit value starting at the given memory address 
 * into memory.  Written in little-endian byte order */
void set_mem_16(uint16_t loc, uint16_t val);


/* Read contents of 2 memory locations starting at the
 * given address. Returned as little-endian byte order 16 bit value */
uint16_t get_mem_16(uint16_t loc); 
    
/* Given the ROM data, load the ROM into
 * Gameboy memory and setup banks */
int load_rom(char const * filename, unsigned char const *file_data, size_t size);


#endif