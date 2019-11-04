#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "libc/stdio.h"
#include "sys.h"

void multiboot_initialize(void){

    printf("Initialize multiboot\n");

    uint32_t mi_address = get_multibot_info();
    struct multiboot_info *mi = (struct multiboot_info*)mi_address;

    //Verify if 0 bit is set
    uint32_t flags = mi->flags;

    if((flags & (1u << 6)) == 0) kernel_panic("Multiboot memory detection failed\n");

    printf("Multiboot memory map is correct.\n");
}