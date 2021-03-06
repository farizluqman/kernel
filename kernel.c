#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "pic.h"
#include "isr.h"
#include "paging.h"
#include "common.h"
#include "kheap.h"
#include "multiboot.h"
#include "keyboard.h"
#include "fat32.h"
#include "fat32_console.h"
#include "kernio.h"
#include "vesa.h"
#include "terminal.h"

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

extern void pause();
extern char _binary_f32_disk_start;



void kernel_main(struct multiboot_info *mi)
{
    terminal_initialize(COLOR_WHITE);
    uint32_t low_pages = 256; // 1024 * 1024 bytes / 4096
    uint32_t high_pages = (mi->mem_upper * 1024) / 4096;

    uint32_t total_frames = high_pages + low_pages;

    set_vmode();

    init_gdt();

    remap_pic();
    init_idt();

    init_timer(100);

    initialize_keyboard();

    initialize_paging(total_frames, get_framebuffer_addr(), get_framebuffer_length());
    malloc_stats();
    printf("Done setting up paging.\nKernel is ready to go!!!\n\n");
    // Kernel ready to go!

    printf("Creating fat32 filesystem.\n");
    f32 *fs = makeFilesystem("");
    if(fs == NULL) {
        printf("Failed to create fat32 filesystem. Disk may be corrupt.\n");
        return;
    }
    printf("Starting fat32 console.\n");

    fat32_console(fs);

    printf("FAT32 shell exited. It is safe to power off.\nSystem is in free-typing mode.\n");

    while(1) {
        char c = get_ascii_char();
        printf("%c", c);
    };
    printf("Halting.\n");

}
