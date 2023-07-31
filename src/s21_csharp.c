#include "s21_string.h"

void *s21_to_upper(const char *str) {
  char *newstr = calloc(s21_strlen(str) + 1, 1);
  if (newstr) {
    for (unsigned i = 0; i < s21_strlen(str); i++) {
      if (str[i] < 'a' || str[i] > 'z') {
        newstr[i] = str[i];
      } else {
        newstr[i] = (char)(str[i] - 32);
      }
    }
  } else {
    s21_errno = MEMALLERR;
  }
  return newstr;
}

void *s21_to_lower(const char *str) {
  char *newstr = calloc(s21_strlen(str) + 1, 1);
  if (newstr) {
    for (unsigned i = 0; i < (unsigned)s21_strlen(str); i++) {
      if (str[i] < 'A' || str[i] > 'Z') {
        newstr[i] = str[i];
      } else {
        newstr[i] = (char)(str[i] + 32);
      }
    }
  } else {
    s21_errno = MEMALLERR;
  }
  return newstr;
}

int start_with(const char *src, const char *totrim, int offset) {
  int res = 0;
  int size = s21_strlen(totrim);
  for (int i = 0; i < size; i++) {
    if (src[offset] == totrim[i]) {
      res = 1;
    }
  }
  return res;
}

int end_with(const char *src, const char *totrim, int offset) {
  int res = 0;
  offset--;
  int size = s21_strlen(totrim);
  for (int i = 0; i < size; i++) {
    if (src[offset] == totrim[i]) {
      res = 1;
    }
  }
  return res;
}
void *s21_trim(const char *src, const char *trim_chars) {
  char *newstr = s21_NULL;
  if (src) {
    if (trim_chars && trim_chars[0]) {
      newstr = calloc(s21_strlen(src) + 1, 1);
      if (newstr) {
        s21_size_t start = 0, end = s21_strlen(src);
        while (start_with(src, trim_chars, start)) {
          start++;
        }
        if (start != end) {
          while (end_with(src, trim_chars, end)) end--;
        } else {
          newstr[0] = '\0';
        }
        for (int i = 0; start < end; i++) {
          newstr[i] = src[start];
          start++;
        }
      } else {
        s21_errno = MEMALLERR;
      }
    } else {
      newstr = s21_trim(src, "\t\n ");
    }
  }
  return newstr;
}

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
  char *newstr = calloc(s21_strlen(src) + s21_strlen(str) + 2, 1);
  if (newstr) {
    if ((src && s21_strlen(src) != 0 && start_index <= s21_strlen(src) + 1) ||
        (src && s21_strlen(src) == 0 && start_index <= 1 &&
         src[start_index] == '\0')) {
      int end1 = 0;
      for (s21_size_t i = 0; i < start_index; i++) {
        newstr[i] = src[i];
        end1++;
      }
      int end2 = end1;
      for (s21_size_t i = 0; i < s21_strlen(str); i++) {
        newstr[end2] = str[i];
        end2++;
      }
      for (s21_size_t i = end1; i < s21_strlen(src); i++) {
        newstr[end2] = src[i];
        end2++;
      }
      newstr[end2] = '\0';
    } else {
      free(newstr);
      newstr = (char *)s21_NULL;
    }
  } else {
    s21_errno = MEMALLERR;
  }
  return newstr;
}
