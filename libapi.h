#ifndef LIBAPI_H
#define LIBAPI_H

#include <fcntl.h>

int open_file(const char* filename);
ssize_t read_file(const char* filename);
ssize_t write_file(const char *filename, const char *content);
int create_file(const char* filename);
int close_file(int fd);
void add_tag(const char *filename, const char *tag);
char *search_tag(const char *tag);
void remove_tag(const char *filename, const char *tag);

#endif
