#include "s21_sscanf.h"
#include "s21_string.h"

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
  bool space;
  bool h;
  bool l;
  bool L;
  bool space_c;
  s21_size_t len_space;
} sscan_format;

static inline
    bool any_flag(sscan_format *specs) {
return specs->space || specs->h || specs->l || specs->L;
}

static inline
    bool s21_isspace(int c) {
  return (c == '\t') || (c == '\n') || (c == '\v') || (c == '\f') || (c == '\r') || (c == ' ');
}

static inline
    bool s21_isdigit(int c) { return (c >= '0') && (c <= '9'); }

static inline
    bool s21_isalpha(int c) {
  return ((c >= 'a') && (c <= 'f')) || ((c >= 'A') && (c <= 'F'));
}

static inline
    bool s21_isupper(int c) { return (c >= 'A') && (c <= 'Z'); }

static inline
    bool check_width(sscan_format *specs) {
bool ret = false;
if (specs->space) {
if (!specs->len_space) {
ret = true;
}
specs->len_space--;
}
return ret;
}

static
const char *skip_to_perc(const char *ptr) {
  for (; *ptr != '%'; ptr++)
    ;
  return ptr;
}

static
char *skip_space(char *ptr) {
  for(; s21_isspace(*ptr);ptr++)
    ;
  return ptr;
}

static
char *skip_non_space(char *ptr) {
  for (; !s21_isspace(*ptr);ptr++);
  return ptr;
}

static
char *skip_format(char *ptr) {
  for(;*ptr;ptr++)
    ;
  return ptr;
}

static
uint64_t s21_strtoul(const char *nptr, char **endptr, int base, sscan_format *spec) {
  const char *s = nptr;
  unsigned long int acc, cutoff;
  int32_t c = (int32_t)*s++, neg = 0, any, cutlim;
  if (c == '-' || c == '+') {
    if (c == '-') {
      if (spec->space) {
        spec->len_space--;
      }
      neg = 1;
    }
    c = (int)*s++;
  }
  if ((base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
    c = (int)s[1];
    s += 2;
    base = 16;
    if (spec->space) {
      spec->len_space-=2;
    }
  }
  cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
  cutlim = (int)((unsigned long)ULONG_MAX % (unsigned long)base);
  for (acc = 0, any = 0; c >= base; c = (int)*s++) {
    if (s21_isdigit(c)) {
      c -= '0';
    } else if (s21_isalpha(c) && base == 16) {
      c -= s21_isupper(c) ? 'A' - 10 : 'a' - 10;
    } else {
      if (c != ' ') {
        s21_errno = SPECERR;
      }
      break;
    }
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
      any = -1;
    } else {
      if (check_width(spec)) {
        break;
      }
      any = 1;
      acc = acc * base + c;
    }
  }
  if (any < 0) {
    acc = ULONG_MAX;
    s21_errno = SPECERR;
  } else if (neg) {
    acc = -acc;
  }
  if (endptr) *endptr = (char *)(any ? s - 1 : nptr);
  return acc;
}

static
char *processing_u(char *buff, va_list *argp, sscan_format *specs,
                   s21_size_t *len) {
  char *end = s21_NULL, *start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  if (specs->l) {
    *(uint64_t *)va_arg(*argp, uint64_t *) =
        (uint64_t)s21_strtoul(buff, &end, 10, specs);
  } else if (specs->h) {
    *(uint16_t *)va_arg(*argp, uint16_t *) =
        (uint16_t)s21_strtoul(buff, &end, 10, specs);
  } else {
    *(uint32_t *)va_arg(*argp, uint32_t *) =
        (uint32_t)s21_strtoul(buff, &end, 10, specs);
  }
  *len += (s21_size_t)(end - buff);
  buff = end;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
char *processing_d(char *buff, const char *format,
                   va_list *argp, sscan_format *specs, s21_size_t *len) {
  char *end = s21_NULL,*start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  int base = 10;
  if (*format == 'i') {
    char *check_base = buff;
    for (; s21_isspace(*check_base); check_base++)
      ;
    if (*check_base == '0') {
      check_base++;
      if (*check_base == 'x' || *check_base == 'X') {
        base = 16;
      } else {
        base = 8;
      }
    }
  }
  if (specs->l) {
    *(int64_t *)va_arg(*argp, int64_t *) =
        (int64_t)s21_strtoul(buff, &end, base, specs);
  } else if (specs->h) {
    *(int16_t *)va_arg(*argp, int16_t *) =
        (int16_t)s21_strtoul(buff, &end, base, specs);
  } else {
    *(int32_t *)va_arg(*argp, int32_t *) =
        (int32_t)s21_strtoul(buff, &end, base, specs);
  }
  *len += (s21_size_t)(end - buff);
  buff = end;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
char *processing_s(char *buff, va_list *argp, sscan_format *specs,
                   s21_size_t *len) {
  char *end,*start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  if (specs->l) {
    wchar_t *ws_loc = (wchar_t *)va_arg(*argp, wchar_t *);
    char *s_ptr = s21_strchr(buff, ' ');
    s21_size_t size_str;
    size_str = (specs->space) ? specs->len_space : (s21_size_t)(s_ptr - buff);
    mbstowcs(ws_loc, buff, size_str);
    for (; size_str; buff++, *len += 1, ws_loc++, size_str--)
      ;
    *ws_loc = L'\0';
  } else {
    end = (char *)va_arg(*argp, char *);
    for (;*buff && !s21_isspace(*buff); end++, *len += 1, buff++) {
      if (check_width(specs)) {
        break;
      }
      *end = *buff;
    }
    *end = '\0';
  }
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
char *processing_c(char *buff, va_list *argp, sscan_format *specs, s21_size_t *len) {
  if (specs->space_c) {
    buff = skip_space(buff);
  }
  if (specs->l) {
    mbtowc(&(*(wchar_t *)va_arg(*argp, wchar_t *)), buff, 1);
  } else {
    *(char *)va_arg(*argp, char *) = *buff;
  }
  *len += 1;
  buff++;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
double s21_strtod(const char *str, char **endptr, sscan_format *specs) {
  int exponent = 0, negative = 0, n, num_digits = 0, num_decimals = 0,
      error = 0;
  char *p = (char *)str;
  double number = 0.0, p10;
  switch (*p) {
    case '-':
      if (specs->space) {
        specs->len_space--;
      }
      negative = 1;
      p++;
      break;
    case '+':
      p++;
      break;
  }
  for (; s21_isdigit(*p); p++, num_digits++) {
    if (check_width(specs)) {
      break;
    }
    number = number * 10.0 + (*p - '0');
  }
  if (*p == '.') {
    p++;
    if (specs->space) {
      specs->len_space--;
    }
    for (; s21_isdigit(*p); p++, num_digits++, num_decimals++) {
      if (check_width(specs)) {
        break;
      }
      number = number * 10. + (*p - '0');
    }
    exponent -= num_decimals;
  }
  if (num_digits == 0) {
    s21_errno = SPECERR;
    error = 1;
    number = 0.0;
  }
  if (!error) {
    negative ? number = -number : number;
    if ((*p == 'e' || *p == 'E') && !specs->space) {
      negative = 0;
      switch (*++p) {
        case '-':
          negative = 1;
          p++;
          break;
        case '+':
          p++;
          break;
      }
      for (n = 0; s21_isdigit(*p); p++) {
        n = n * 10 + (*p - '0');
      }
      if (negative) {
        exponent -= n;
      } else {
        exponent += n;
      }
    }
    if (exponent < DBL_MIN_EXP || exponent > DBL_MAX_EXP) {
      s21_errno = SPECERR;
      number = HUGE_VAL;
      error = 1;
    }
    if (!error) {
      p10 = 10.0;
      n = exponent;
      n < 0 ? n = -n : n;
      while (n) {
        if (n & 1) {
          if (exponent < 0) {
            number /= p10;
          } else {
            number *= p10;
          }
        }
        n >>= 1;
        p10 *= p10;
      }
      if (number == HUGE_VAL) s21_errno = SPECERR;
      if (endptr) *endptr = p;
    }
  }
  return number;
}

static
char *processing_f(char *buff, va_list *argp, sscan_format *specs,
                   s21_size_t *len) {
  char *end = s21_NULL,*start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  if (specs->L) {
    *(long double *)va_arg(*argp, long double *) =
    (long double)s21_strtod(buff, &end, specs);
  } else if (specs->l) {
    *(double *)va_arg(*argp, double *) = (double)s21_strtod(buff, &end, specs);
  } else {
    *(float *)va_arg(*argp, float *) = (float)s21_strtod(buff, &end, specs);
  }
  *len += (s21_size_t)(end - buff);
  buff = end;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
char *processing_o(char *buff, va_list *argp, sscan_format *specs,
                   s21_size_t *len) {
  char *end = s21_NULL,*start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  if (specs->l) {
    *(uint64_t *)va_arg(*argp, uint64_t *) =
        (uint64_t)s21_strtoul(buff, &end, 8, specs);
  } else if (specs->h) {
    *(uint16_t *)va_arg(*argp, uint16_t *) =
        (uint16_t)s21_strtoul(buff, &end, 8, specs);
  } else {
    *(uint32_t *)va_arg(*argp, uint32_t *) =
        (uint32_t)s21_strtoul(buff, &end, 8, specs);
  }
  *len += (s21_size_t)(end - buff);
  buff = end;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
char *processing_x(char *buff, va_list *argp, sscan_format *specs,
                   s21_size_t *len) {
  char *end = s21_NULL, *start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  if (specs->l) {
    *(uint64_t *)va_arg(*argp, uint64_t *) =
        (uint64_t)s21_strtoul(buff, &end, 16, specs);
  } else if (specs->h) {
    *(uint16_t *)va_arg(*argp, uint16_t *) =
        (uint16_t)s21_strtoul(buff, &end, 16, specs);
  } else {
    *(uint32_t *)va_arg(*argp, uint32_t *) =
        (uint32_t)s21_strtoul(buff, &end, 16, specs);
  }
  *len += (s21_size_t)(end - buff);
  buff = end;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
char *processing_p(char *buff, va_list *argp, sscan_format *specs,
                   s21_size_t *len) {
  char *end = s21_NULL, *start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  *(void **)va_arg(*argp, void **) = (void *)s21_strtoul(buff, &end, 16, specs);
  *len += (s21_size_t)(end - buff);
  buff = end;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

static
char *processing_n(char *buff, va_list *argp, sscan_format *specs, s21_size_t *len) {
  char *start = buff;
  buff = skip_space(buff);
  *len += buff - start;
  *(uint32_t *)va_arg(*argp, uint32_t *) = (uint32_t)*len;
  s21_memset(specs, 0, sizeof(sscan_format));
  return buff;
}

int s21_sscanf(char *str, const char *format, ...) {
  s21_errno = -1;
  va_list argp;
  sscan_format specs = {0};
  va_start(argp, format);
  s21_size_t len = 0;
  int32_t ret = (!*str) ? -1 : 0;
  char *buff = str;
  while (*format && s21_errno < 0) {
    if (*format == '%' || any_flag(&specs)) {
      format++;
      switch (*format) {
        case '%':
          buff = skip_format(buff);
          break;
        case '*':
          buff = skip_non_space(buff);
          format = skip_to_perc(format);
          continue;
        case '0' ... '9':
          specs.len_space = specs.len_space * 10 + (*format - '0');
          specs.space = true;
          continue;
        case 'L':
          specs.L = true;
          continue;
        case 'l':
          specs.l = true;
          continue;
        case 'h':
          specs.h = true;
          continue;
        case 'u':
          buff = processing_u(buff, &argp, &specs, &len);
          ret++;
          break;
        case 's':

          buff = processing_s(buff, &argp, &specs, &len);
          ret++;
          break;
        case 'd':
        case 'i':
          buff = processing_d(buff, format, &argp, &specs, &len);
          if (s21_errno < 0) {
            ret++;
          }
          break;
        case 'c':
          buff = processing_c(buff, &argp, &specs, &len);
          ret++;
          break;
        case 'f':
        case 'g':
        case 'G':
        case 'E':
        case 'e':
          buff = processing_f(buff, &argp, &specs, &len);
          if (s21_errno < 0) {
            ret++;
          }
          break;
        case 'o':
          buff = processing_o(buff, &argp, &specs, &len);
          ret++;
          break;
        case 'X':
        case 'x':
          buff = processing_x(buff, &argp, &specs, &len);
          ret++;
          break;
        case 'p':
          buff = processing_p(buff, &argp, &specs, &len);
          ret++;
          break;
        case 'n':
          buff = processing_n(buff, &argp, &specs, &len);
          break;
        default:
          s21_errno = SPECERR;
          buff = skip_format(buff);
          break;
      }
    } else if (*format == ' ') {
      specs.space_c = true;
    } else {
      break;
    }
    format++;
  }
  va_end(argp);
  return ret;
}