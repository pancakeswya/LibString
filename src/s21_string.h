#ifndef S21_STRING_SRC_S21_STRING_H
#define S21_STRING_SRC_S21_STRING_H

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define BUFF_SIZE 512
#define MEMALLERR 11
#define SPECERR 31

typedef long unsigned s21_size_t;
#define s21_NULL ((void *)0)

extern int s21_errno;

#include "s21_csharp.h"
#include "s21_sprintf.h"
#include "s21_sscanf.h"

char *s21_strstr(const char *, const char *);
size_t s21_strlen(const char *str);
int s21_strcmp(const char *str1, const char *str2);
int s21_strncmp(const char *str1, const char *str2, size_t n);
char *s21_strcpy(char *dest, const char *src);
char *s21_strncpy(char *dest, const char *src, size_t n);
char *s21_strcat(char *dest, const char *src);
char *s21_strncat(char *dest, const char *src, s21_size_t n);
char *s21_strchr(const char *str, int c);
s21_size_t s21_strspn(const char *str1, const char *str2);
s21_size_t s21_strcspn(const char *str1, const char *str2);
char *s21_strtok(char *str, const char *delim);
char *s21_strpbrk(const char *str1, const char *str2);
void *s21_memcpy(void *dest, const void *src, s21_size_t n);
void *s21_memmove(void *dest, const void *src, s21_size_t n);
int s21_memcmp(const void *str1, const void *str2, s21_size_t n);
void *s21_memchr(const void *str, int c, s21_size_t n);
void *s21_memset(void *str, int c, s21_size_t n);
char *s21_strerror(int errnum);
char *s21_strrchr(const char *str, int ch);

#endif  // S21_STRING_SRC_S21_STRING_H
