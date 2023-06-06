#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include <unistd.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg) {
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + ehdr->e_phoff);

    printf("Type\tOffset\t\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");
    for (int i = 0; i < ehdr->e_phnum; ++i) {
        // func(phdr + (ehdr->e_phentsize * i), i);
        func(&phdr[i] , i);
    }
    
    return 0;
}

// Function to print program header information
void print_phdr_info(Elf32_Phdr *phdr, int index) {
    // printf("Program header number %d at address 0x%lx\n", index, (unsigned long)phdr);
    const char *type;
    switch (phdr->p_type) {
            case PT_NULL:    type = "NULL"; break;
            case PT_LOAD:    type = "LOAD"; break;
            case PT_DYNAMIC: type = "DYNAMIC"; break;
            case PT_INTERP:  type = "INTERP"; break;
            case PT_NOTE:    type = "NOTE"; break;
            case PT_SHLIB:   type = "SHLIB"; break;
            case PT_PHDR:    type = "PHDR"; break;
            default:         type = "UNKNOWN"; break;
        }
    printf("%s\t%#08x\t%#010x\t%#010x\t%#07x\t%#07x\t%c%c%c\t%#06x\n",
       type, phdr->p_offset, phdr->p_vaddr, phdr->p_paddr,
       phdr->p_filesz, phdr->p_memsz,
       (phdr->p_flags & PF_R) ? 'R' : ' ',
       (phdr->p_flags & PF_W) ? 'W' : ' ',
       (phdr->p_flags & PF_X) ? 'E' : ' ',
       phdr->p_align);

}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }
    
    const char *file_name = argv[1];
    int fd = open(file_name, O_RDONLY);
    
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    
    off_t file_size = lseek(fd, 0, SEEK_END);
    void *map_start = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if (map_start == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }
    
    foreach_phdr(map_start, print_phdr_info, 0);
    
    munmap(map_start, file_size);
    close(fd);
    
    return 0;
}
