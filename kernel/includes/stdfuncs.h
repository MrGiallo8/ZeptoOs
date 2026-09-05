#ifndef STDFUNCS_H 
#define STDFUNCS_H

#include "stout.h"

// dichiarazioni print putc puts

void pputc(char x);
void pputs(const char *s);
void print_int(int n);
void write(const char *fmt, ...);
void clear_screen();

// dichiarazioni stringe
int strleng(const char *s);
int strcmp(const char *a, const char *b);
int strcmp_n(const char *a, const char *b, int n);

void disable_cursor(void);

uint64_t random();
uint64_t random_dig(int cifre);
int random_between(int min, int max); 
int str_to_int(char str[]);

#endif
