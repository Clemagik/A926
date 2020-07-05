#ifndef __file__
#define __file__

int file_open(char *file_name);
int file_close(int fd);

int file_read(int fd, char *buf, int size, int *fr_type);


#endif //__file__

