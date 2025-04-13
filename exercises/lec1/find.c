#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


int check_special_dir(char *name){
    return strcmp(".", name) == 0 || strcmp("..", name) == 0;
}

void iterrate_though(char *path, char *file) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_DEVICE:
    case T_FILE:
        fprintf(2, "find: cannot search a file %s\n", path);
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }

        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_FILE && strcmp(file, de.name) == 0) {
                printf("%s\n", buf);
            } else if (st.type == T_DIR) {
                if (check_special_dir(de.name)) {
                    continue;
                }
                iterrate_though(buf, file);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(2, "invalid number of arguments");
    }

    char *search_path = argv[1];
    char *file = argv[2];

    iterrate_though(search_path, file);
}