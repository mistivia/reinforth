#ifndef NEBUTIL_STR_H_
#define NEBUTIL_STR_H_

#include <stdio.h>

char *str_strip(char *str);
char **str_split(char *str, char delim);
void str_list_free(char **list);

typedef struct {
    char *buf;
    int size;
    int cap;
} StrBuilder;

void sb_init(StrBuilder *sb);
void sb_append(StrBuilder *sb, char *format, ...);
void sb_appendc(StrBuilder *sb, char c);

char *fgetline(FILE *fp);
int fpeek(FILE *fp);

#endif
