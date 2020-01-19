#include "fat_shell.h"
#include "fat.h"
#include "shell.h"
#include "libc/stdio.h"

struct directory dir;
fat32 *fs;

void register_fat_shell_api(void){

    fs = get_filesystem();
    populate_root_dir(fs, &dir);

    register_shell_command("touch", "Create new file", touch_command);
    register_shell_command("ls", "Display directory content", touch_command);
}

void touch_command(const char* argv, uint32_t argc){
    //filename?? get word from agrv!!

    write_file(fs, &dir, (uint8_t *)"", "test.txt", 0);
}

void ls_command(const char* argv, uint32_t argc){
    list_directory(fs, &dir);
}