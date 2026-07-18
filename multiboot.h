#ifndef _INCLUDE_MULTIBOOT_H_
#define _INCLUDE_MULTIBOOT_H_

typedef struct multiboot_info {
    unsigned int flags;
    unsigned int mem_lower;
    unsigned int mem_upper;
    unsigned int boot_device;
    unsigned int cmdline;
    unsigned int mods_count;
    unsigned int mods_addr;
    /* outros campos existem na struct real do GRUB, mas nao precisamos
     * deles agora — o layout de memoria continua correto porque estes
     * sao os primeiros campos da struct oficial */
} multiboot_info_t;

typedef struct multiboot_module {
    unsigned int mod_start;
    unsigned int mod_end;
    unsigned int string;
    unsigned int reserved;
} multiboot_module_t;

/* bit 3 das flags = "modules loaded" */
#define MULTIBOOT_FLAG_MODS 0x00000008

#endif