#include "kernel.h"
#include "multiboot.h"


multiboot_info_t multiboot_header;
unsigned long mb_magic;

void multiboot_initalize( unsigned long _mb_magic, multiboot_info_t *mbh ) {
    // DO NOT PUT KLOG FUNCTIONS HERE
	mb_magic = _mb_magic;
	memcpy( &multiboot_header, mbh, sizeof( multiboot_info_t ) );

    
    multiboot_header.cmdline += KERNEL_VIRT_LOAD_BASE;
    multiboot_header.mods_addr += KERNEL_VIRT_LOAD_BASE;
    multiboot_header.u.elf_sec.addr += KERNEL_VIRT_LOAD_BASE;
    multiboot_header.mmap_addr += KERNEL_VIRT_LOAD_BASE;
    multiboot_header.drives_addr += KERNEL_VIRT_LOAD_BASE;
    multiboot_header.boot_loader_name += KERNEL_VIRT_LOAD_BASE;
    //multiboot_header.framebuffer_addr += KERNEL_VIRT_LOAD_BASE;
}

multiboot_info_t * get_multiboot_header( void ) {
	return &multiboot_header;
}

void multiboot_echo_to_serial( void ) {
    uint32_t mb_flags = multiboot_header.flags;

    #define kdebug_multiboot
    #ifdef kdebug_multiboot
 
    debugf( "\nMultiboot v1 Dump\n" );
    debugf( "-----------------\n" );
    debugf( "magic: 0x%08X\n", mb_magic );
    
    
    debugf_bit_array( mb_flags );
    debugf( "-\n" );
    debugf( "mem_lower:             0x%08X\n", multiboot_header.mem_lower );
    debugf( "mem_upper:             0x%08X\n", multiboot_header.mem_upper );
    debugf( "-\n" );
    debugf( "boot_device:           0x%08X\n", multiboot_header.boot_device );
    debugf( "-\n" );
    debugf( "cmdline:               '%s'\n", (char *)(multiboot_header.cmdline) );
    debugf( "-\n" );
    debugf( "mods_count:            0x%08X\n", multiboot_header.mods_count );
    debugf( "mods_addr:             0x%08X\n", multiboot_header.mods_addr );
    debugf( "-\n" );
    debugf( "elf_sec.num:           0x%08X\n", multiboot_header.u.elf_sec.num );
    debugf( "elf_sec.size:          0x%08X\n", multiboot_header.u.elf_sec.size );
    debugf( "elf_sec.addr:          0x%08X\n", multiboot_header.u.elf_sec.addr );
    debugf( "elf_sec.shndx:         0x%08X\n", multiboot_header.u.elf_sec.shndx );
    debugf( "-\n" );
    debugf( "mmap_length:           0x%08X\n", multiboot_header.mmap_length );
    debugf( "mmap_addr:             0x%08X\n", multiboot_header.mmap_addr );
    debugf( "-\n" );
    debugf( "drives_length:         0x%08X\n", multiboot_header.drives_length );
    debugf( "drives_addr:           0x%08X\n", multiboot_header.drives_addr );
    debugf( "-\n" );
    debugf( "config_table:          0x%08X\n", multiboot_header.config_table );
    debugf( "-\n" );
    debugf( "boot_loader_name:      '%s'\n", (char *)(multiboot_header.boot_loader_name) );
    debugf( "-\n" );
    debugf( "vbe_control_info:      0x%08X\n", multiboot_header.vbe_control_info );
    debugf( "vbe_mode_info:         0x%08X\n", multiboot_header.vbe_mode_info );
    debugf( "vbe_mode:              0x%04X\n", multiboot_header.vbe_mode );
    debugf( "vbe_interface_seg:     0x%04X\n", multiboot_header.vbe_interface_seg );
    debugf( "vbe_interface_off:     0x%04X\n", multiboot_header.vbe_interface_off );
    debugf( "vbe_interface_len:     0x%04X\n", multiboot_header.vbe_interface_len );
    debugf( "-\n" );
    debugf( "framebuffer_addr:      0x%016X\n", multiboot_header.framebuffer_addr );
    debugf( "framebuffer_pitch:     0x%08X\n", multiboot_header.framebuffer_pitch );
    debugf( "framebuffer_width:     0x%08X\n", multiboot_header.framebuffer_width );
    debugf( "framebuffer_height:    0x%08X\n", multiboot_header.framebuffer_height );
    debugf( "framebuffer_bpp:       0x%02X\n", multiboot_header.framebuffer_bpp );
    debugf( "framebuffer_type:      0x%02X\n", multiboot_header.framebuffer_type );

    debugf( "\n" );

    #endif
}