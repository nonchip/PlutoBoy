//HDMA CGB module
#include <stdint.h>
#include <stdio.h>
#include "hdma.h"
#include "memory.h"

#include "hook_mem.h"

#include "../memory_layout.h"
#include "../timers.h" 
#include "../lcd.h"
#include "../emu.h"

void check_cgb_dma(uint8_t value) {

    hdma_bytes = 0x10 + ((value & 0x7F) * 0x10);
    
    if (hdma_in_progress) {
        if (value & BIT_7) {
            io_write_override(HDMA5_REG - 0xFF00, value & 0x7F); // Transferring
        } else {
            io_write_override(HDMA5_REG - 0xFF00, 0xFF); // Finished transferring
            hdma_in_progress = 0;
        }
    
    } else {
        if (value & BIT_7) {
            hdma_in_progress = 1;
            io_write_override(HDMA5_REG - 0xFF00, value & 0x7F);
            if (lcd_hblank_mode()) {
                long cycles = perform_hdma();
                add_current_cycles(cycles);
            } 
        } else {
            perform_gdma(value);
        }
    }
    
}

// Performs a HDMA transfer of 0x10 bytes from source to destination
// returns the amount of machine cycles taken
long perform_hdma() {

    uint16_t source = hdma_source & 0xFFF0;
    uint16_t dest = (hdma_dest & 0x1FF0) | 0x8000;

    if(hook_dma(source,0x10)){
        uint8_t buf[0x11];
        do_hook_dma(source,0x10,&buf);
        for (int i = 0; i < 0x10; i++) {
            set_mem(dest + i, buf[i]);
        }
    }else{
        for (int i = 0; i < 0x10; i++) {
            set_mem(dest + i, get_mem(source + i));       
        }
    }

    hdma_source += 0x10;
    hdma_dest +=  0x10;

    //Keep destination address between 0x8000 and 0x9FFF
    if (hdma_dest == 0xA000) {
        hdma_dest = 0x8000;
    }

    // Keep source address away from destination addresses
    if (hdma_source == 0x8000) {
        hdma_source = 0xA000;
    }

    io_write_override(HDMA1_REG - 0xFF00, hdma_source >> 8); 
    io_write_override(HDMA2_REG - 0xFF00, hdma_source & 0xFF);
    io_write_override(HDMA3_REG - 0xFF00, hdma_dest >> 8);
    io_write_override(HDMA4_REG - 0xFF00, hdma_dest & 0xFF);

    hdma_bytes -= 0x10;

    io_write_override(HDMA5_REG - 0xFF00, get_mem(HDMA5_REG) - 1); // 1 less block to transfer

    if (get_mem(HDMA5_REG) == 0xFF || hdma_bytes == 0) {
        hdma_in_progress = 0;
    }
       
    return (cgb_speed ? 68 : 36); 
}


void perform_gdma(uint8_t value) {                     
    uint16_t source = hdma_source & 0xFFF0;
    uint16_t dest = (hdma_dest & 0x1FF0) | 0x8000;
 
    if(hook_dma(source,hdma_bytes)){
        uint8_t buf[hdma_bytes+1];
        do_hook_dma(source,hdma_bytes,&buf);
        for (int i = 0; i < hdma_bytes; i++) {
            set_mem(dest + i, buf[i]);
        }
    }else{
        for (int i = 0; i < hdma_bytes; i++) {
            set_mem(dest + i, get_mem(source + i));       
        }
    }

    io_write_override(HDMA1_REG - 0xFF00, 0xFF); 
    io_write_override(HDMA2_REG - 0xFF00, 0xFF);
    io_write_override(HDMA3_REG - 0xFF00, 0xFF);
    io_write_override(HDMA4_REG - 0xFF00, 0xFF);
    io_write_override(HDMA5_REG - 0xFF00, 0xFF); 

    hdma_source += hdma_bytes;
    hdma_dest += hdma_bytes;
  
    long clock_cycles = 0;
    if (cgb_speed) {
        clock_cycles = 2 + 16 * ((value & 0x7F) + 1);
    } else {
        clock_cycles = 1 + 8 * ((value & 0x7F) + 1);
    }
  
    add_current_cycles(clock_cycles * 4);  
}


