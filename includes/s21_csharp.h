#ifndef SRC_S21_CSHARP_H
#define SRC_S21_CSHARP_H

#include "s21_string.h"

void *s21_to_upper(const char *str);
void *s21_to_lower(const char *str);
void *s21_insert(const char *src, const char *str, s21_size_t start_index);
int start_with(const char *src, const char *totrim, int offset);
int end_with(const char *src, const char *totrim, int offset);
void *s21_trim(const char *src, const char *trim_chars);

#endif  // SRC_S21_CSHARP_H
