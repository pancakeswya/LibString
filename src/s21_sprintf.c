#include "s21_sprintf.h"

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define BUFF_SIZE 512

typedef struct {
  bool dot;
  bool space;
  bool plus;
  bool minus;
  bool h;
  bool l;
  bool L;
  bool O;
  bool star;
  bool space_star;
  bool no_sign;
  bool cell;
  s21_size_t len_space;
}sprint_format;

static inline int any_flag(sprint_format* specs) {
  return specs->plus || specs->space || specs->dot || specs->h ||
      specs->l || specs->L || specs->minus || specs->no_sign ||
      specs->cell;
}
static inline void s21_chrcat(char* buffer, const char* format) {
  char ch[1] = {*format};
  s21_strcat(buffer, ch);
}

static inline void to_upAlpha(char* str) {
  for (;*str; str++) {
    if (*str >= 'a' && *str <= 'z') {
      *str -= 32;
    }
  }
}

static inline void calculate_space(sprint_format* specs, s21_size_t after_point, s21_size_t length) {
  if (specs->dot) {
    specs->O = false;
    if (specs->len_space > after_point) {
      if (after_point > length) {
        specs->len_space -= after_point;
        if (!specs->minus) {
          specs->len_space += length;
        }
      }
    } else {
      specs->len_space = 0;
    }
  }
}

static inline void width_print(char* buffer, sprint_format* specs, s21_size_t length) {
  if (length < specs->len_space) {
    if (specs->plus || specs->no_sign) {
      length++;
    }
    specs->len_space -= length;
    for (; specs->len_space; specs->len_space--) {
      if (specs->O) {
        s21_strcat(buffer, "0");
      } else {
        s21_strcat(buffer, " ");
      }
    }
  }
}

static inline void get_width_star(sprint_format* specs, va_list* argp, s21_size_t* after_point) {
  if (specs->dot) {
    *after_point = (s21_size_t)va_arg(*argp, int64_t);
    specs->star = true;
  } else {
    specs->len_space = (s21_size_t)va_arg(*argp, int64_t);
    specs->space_star = true;
    specs->space = true;
  }
}

static inline void get_width_num(const char* format, sprint_format* specs, s21_size_t* after_point) {
  if (!specs->dot) {
    if (*format == '0' && !specs->len_space) {
      specs->O = true;
    }
    if (!specs->space_star) {
      specs->len_space = specs->len_space * 10 + (*format - '0');
    }
    specs->space = true;
  } else if (!specs->star) {
    *after_point = *after_point * 10 + (*format - '0');
  }
}


static char* s21_itoa(int64_t val, int base) {
  char* ret = (char*)calloc(BUFF_SIZE, sizeof(char));
  if (ret) {
    char tmp[BUFF_SIZE] = {0}, digitPick[] = "0123456789abcdef";
    int idx = BUFF_SIZE - 2, neg = 0;
    if (val < 0) {
      neg = 1;
      val = -val;
    }
    if (!val) {
      tmp[idx] = '0';
    }
    while (val > 0) {
      idx--;
      tmp[idx] = digitPick[val % base];
      val /= base;
    }
    for (int j = 0; tmp[idx]; idx++, j++) {
      if (neg && j == 0) {
        ret[j++] = '-';
      }
      ret[j] = tmp[idx];
    }
  } else {
    s21_errno = MEMALLERR;
  }
  return ret;
}

static void format_d(char* buffer, va_list* argp, sprint_format* specs, s21_size_t* after_point) {
  bool neg = false;
  int64_t num = va_arg(*argp, int64_t);
  s21_size_t length = 0;
  if (specs->l) {
    num = (int64_t)num;
  } else if (specs->h) {
    num = (int16_t)num;
  } else {
    num = (int32_t)num;
  }
  if (num < 0) {
    neg = true;
    num = -num;
    specs->plus = false;
    specs->no_sign = false;
    length++;
  }
  char* i_str = s21_itoa(num, 10);
  if (i_str) {
    s21_size_t len_str = s21_strlen(i_str);
    length += len_str;
    if (!specs->minus && specs->space) {
      calculate_space(specs, *after_point, len_str);
      width_print(buffer, specs, length);
    }
    if (neg) {
      s21_strcat(buffer, "-");
    } else if (specs->plus) {
      s21_strcat(buffer, "+");
    } else if (specs->no_sign) {
      s21_strcat(buffer, " ");
    }
    if (specs->dot && (*after_point > len_str)) {
      *after_point -= len_str;
      for (; *after_point; length++, *after_point -= 1) {
        s21_strcat(buffer, "0");
      }
    } else if (specs->O) {
      width_print(buffer, specs, length);
    }
    s21_strcat(buffer, i_str);
    if (specs->minus && specs->space) {
      calculate_space(specs, *after_point, len_str);
      width_print(buffer, specs, length);
    }
  }
  *after_point = 6;
  s21_memset(specs, 0, sizeof(sprint_format));
  free(i_str);
}

static char* s21_utoa(uint64_t val, int base) {
  char* ret = (char*)calloc(BUFF_SIZE, sizeof(char));
  if (ret) {
    char buf[BUFF_SIZE + 1] = {0}, digitPick[] = "0123456789abcdef";
    int idx = BUFF_SIZE - 1;
    if (!val) {
      buf[idx] = '0';
      idx--;
    }
    for (; val && idx; --idx, val /= base) {
      buf[idx] = digitPick[val % base];
    }
    for (int j = 0; buf[idx + 1]; idx++, j++) {
      ret[j] = buf[idx + 1];
    }
  } else {
    s21_errno = MEMALLERR;
  }
  return ret;
}

static void format_u(char* buffer, va_list* argp, sprint_format* specs, s21_size_t* after_point) {
  uint64_t uNum = va_arg(*argp, uint64_t);
  if (specs->l) {
    uNum = (uint64_t)uNum;
  } else if (specs->h) {
    uNum = (uint16_t)uNum;
  } else {
    uNum = (uint32_t)uNum;
  }
  char *u_str = s21_utoa(uNum, 10);
  if (u_str) {
    s21_size_t length = s21_strlen(u_str);
    if (!specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
    if (specs->dot && (*after_point > length)) {
      *after_point -= length;
      for (; *after_point; length++, *after_point -= 1) {
        s21_strcat(buffer, "0");
      }
    }
    s21_strcat(buffer, u_str);
    if (specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
  }
  *after_point = 6;
  s21_memset(specs, 0, sizeof(sprint_format));
  free(u_str);
}

static void format_s(char* buffer, va_list* argp, sprint_format* specs, s21_size_t* after_point) {
  char tmp[BUFF_SIZE] = {0};
  char* ptr = tmp;
  if (specs->l) {
    wchar_t *wStr = va_arg(*argp, wchar_t*);
    wcstombs(tmp, wStr, sizeof(tmp));
  } else {
    ptr = va_arg(*argp, char*);
  }
  s21_size_t length = s21_strlen(ptr);
  if (specs->dot && (*after_point < length)) {
    length = *after_point;
  }
  if (!specs->minus && specs->space) {
    width_print(buffer, specs, length);
  }
  if (specs->dot) {
    s21_strncat(buffer, ptr, *after_point);
  } else {
    s21_strcat(buffer, ptr);
  }
  if (specs->minus && specs->space) {
    width_print(buffer, specs, length);
  }
  *after_point = 6;
  s21_memset(specs, 0, sizeof(sprint_format));
}

static void format_c(char* buffer, va_list* argp, sprint_format *specs) {
  char ch[6] = {0};
  if (specs->l) {
    wchar_t wch = va_arg(*argp, int);
    wctomb(ch, wch);
  } else {
    ch[0] = va_arg(*argp, int);
  }
  if (!specs->minus && specs->space) {
    width_print(buffer, specs, 1);
  }
  s21_strcat(buffer, ch);
  if (specs->minus && specs->space) {
    width_print(buffer, specs, 1);
  }
  s21_memset(specs, 0, sizeof(sprint_format));
}

static inline void round_float(char* str, int i) {
  if (str[i - 1] != '9') {
    str[i - 1] += 1;
  } else {
    int idx = 1;
    while (str[i - idx] == '9') {
      str[i - idx] = '0';
      idx++;
    }
    if (str[i - idx] != '.') {
      str[i - idx] += 1;
    } else {
      str[i - idx - 1] += 1;
    }
  }
}

static char* s21_dtoa(long double n, s21_size_t after_point, sprint_format* specs) {
  char* ret = (char*)calloc(BUFF_SIZE, sizeof(char));
  if (ret) {
    char reverseInt[BUFF_SIZE] = {0};
    int charCount = 0;
    bool neg = false;
    long double fp_int, fp_frac;
    if (n < 0.0) {
      neg = 1;
      n = -n;
    }
    fp_frac = modfl(n, &fp_int);
    if (fp_int == 0) {
      reverseInt[charCount++] = '0';
    }
    while (fp_int > 0) {
      reverseInt[charCount++] = (char)('0' + (int)fmodl(fp_int, 10));
      fp_int = floorl(fp_int / 10.0);
    }
    if (neg) {
      reverseInt[charCount++] = '-';
    }
    for (int i = 0; i < charCount; i++) {
      ret[i] = reverseInt[charCount - i - 1];
    }
    if (after_point || specs->cell) {
      ret[charCount++] = '.';
    }
    for (after_point++; after_point; after_point--) {
      fp_frac *= 10.0;
      if (after_point == 1) {
        if (fp_frac >= 5) {
          round_float(ret, charCount);
        }
      } else {
        fp_frac = modfl(fp_frac, &fp_int);
        ret[charCount++] = (char)('0' + (int)fp_int);
      }
    }
  } else {
    s21_errno = MEMALLERR;
  }
  return ret;
}

static char* s21_e_dtoa(long double n, s21_size_t afterPoint, sprint_format* specs) {
  char* ret = (char*)calloc(BUFF_SIZE, sizeof(char));
  if (ret) {
    bool neg = 0;
    char reverseInt[BUFF_SIZE] = {'\0'};
    int charCount = 0, zeroPnt = 0, exp = 0;
    long double fp_int, fp_frac;
    if (n < 0.0) {
      neg = 1;
      n = -n;
    }
    if (n < 1.0) {
      for (; n < 1.0; n *= 10.0, exp++)
        ;
      zeroPnt = 1;
    }
    fp_frac = modfl(n, &fp_int);
    if (fp_int == 0) {
      reverseInt[charCount++] = '0';
      zeroPnt = 1;
    }
    while (fp_int > 0) {
      if (fp_int >= 1 && fp_int <= 9) {
        if (!zeroPnt) {
          exp = charCount;
        }
        if (afterPoint) {
          reverseInt[charCount++] = '.';
        }
      }
      reverseInt[charCount++] = (char)('0' + (int)fmodl(fp_int, 10));
      fp_int = floorl(fp_int / 10.0);
    }
    if (neg) {
      reverseInt[charCount++] = '-';
      afterPoint++;
    }
    for (int i = 0; i < charCount; i++) {
      if (!afterPoint && i >= 1 && n >= 10.0) {
        if (reverseInt[charCount - i - 1] - '0' >= 5) {
          round_float(ret, i);
        }
        break;
      }
      if (i > 1) {
        afterPoint--;
      }
      ret[i] = reverseInt[charCount - i - 1];
    }
    if (!s21_strchr(ret, '.') && specs->cell) {
      s21_strcat(ret, ".");
    }
    for (afterPoint++; afterPoint; afterPoint--) {
      fp_frac *= 10.0;
      if (afterPoint == 1) {
        if (fp_frac >= 5) {
          round_float(ret, charCount);
        }
      } else {
        fp_frac = modfl(fp_frac, &fp_int);
        ret[charCount++] = (char)('0' + (int)fp_int);
      }
    }
    s21_strcat(ret, "e");
    if (!zeroPnt) {
      s21_strcat(ret, "+");
    } else {
      s21_strcat(ret, "-");
    }
    if (exp < 10) {
      s21_strcat(ret, "0");
    }
    char *nStr = s21_itoa(exp, 10);
    if (nStr) {
      s21_strcat(ret, nStr);
    }
    free(nStr);
  } else {
    s21_errno = MEMALLERR;
  }
  return ret;
}

static void delete_trail0(char*g_str) {
  char exp_buff[BUFF_SIZE] = {0};
  int j = 0;
  s21_size_t i = s21_strlen(g_str) - 1;
  bool found_e = false;
  found_e = !(s21_strchr(g_str, 'e'));
  for (; !found_e; i--, j++) {
    exp_buff[j] = g_str[i];
    found_e = (g_str[i] == 'e');
    g_str[i] = '\0';
  }
  for (; g_str[i] == '0'; i--) {
    g_str[i] = '\0';
  }
  for (i++; j--; i++) {
    g_str[i] = exp_buff[j];
  }
}

static int exp_count(long double n) {
  int exp = 0, char_count = 0;
  bool zero_pnt = false;
  long double fp_int;
  n < 0.0 ? n = -n : n;
  if (n < 1.0) {
    for (; n < 1.0; n *= 10.0, exp++)
      ;
    exp = -exp;
    zero_pnt = true;
  }
  modfl(n, &fp_int);
  if (fp_int == 0) {
    zero_pnt = true;
  }
  while (fp_int > 0) {
    if (fp_int >= 1 && fp_int <= 9) {
      if (!zero_pnt) {
        exp = char_count;
      }
    }
    char_count++;
    fp_int = floorl(fp_int / 10.0);
  }
  return exp;
}

static char* s21_g_dtoa(long double n, s21_size_t after_point, sprint_format* specs) {
  char *g_str = s21_NULL;
  int exp = 0, p;
  if (after_point) {
    p = (int)after_point;
  } else if (!specs->dot) {
    p = 6;
  } else {
    p = 1;
  }
  exp = exp_count(n);
  if (exp < p && exp >= -4) {
    after_point = p - (exp + 1);
    g_str = s21_dtoa(n, after_point, specs);
  } else {
    if (after_point) {
      after_point--;
    }
    g_str = s21_e_dtoa(n, after_point, specs);
  }
  if (g_str && !specs->cell) {
    delete_trail0(g_str);
  }
  return g_str;
}

static void format_f(char* buffer, const char* format, va_list* argp, sprint_format* specs,
              s21_size_t *after_point) {
  bool neg = false;
  char *e_str = s21_NULL;
  s21_size_t length = 0;
  long double e = (specs->L) ?  va_arg(*argp, long double) : va_arg(*argp, double);
  if (e < 0.0) {
    e = -e;
    if (!specs->O) {
      neg = true;
    } else {
      s21_strcat(buffer, "-");
    }
    length++;
    specs->plus = false;
    specs->no_sign = false;
  } else if (specs->O) {
    if (specs->plus) {
      s21_strcat(buffer, "+");
    } else if (specs->no_sign) {
      s21_strcat(buffer, " ");
    }
  }
  if (*format == 'f') {
    e_str = s21_dtoa(e, *after_point, specs);
  } else if (*format == 'e' || *format == 'E') {
    e_str = s21_e_dtoa(e, *after_point, specs);
  } else if (*format == 'g' || *format == 'G') {
    e_str = s21_g_dtoa(e, *after_point, specs);
  }
  if (e_str) {
    length += s21_strlen(e_str);
    if (!specs->minus && specs->space) {
      width_print(buffer, specs, length);
    }
    if (neg) {
      s21_strcat(buffer, "-");
    } else if (specs->plus) {
      s21_strcat(buffer, "+");
    } else if (specs->no_sign) {
      s21_strcat(buffer, " ");
    }
    if (*format == 'E' || *format == 'G') {
      to_upAlpha(e_str);
    }
    s21_strcat(buffer, e_str);
    if (specs->minus && specs->space) {
      width_print(buffer, specs, length);
    }
  }
  *after_point = 6;
  s21_memset(specs, 0, sizeof(sprint_format));
  free(e_str);
}

static void format_o(char* buffer, va_list* argp, sprint_format* specs, s21_size_t* after_point) {
  uint64_t oNum = va_arg(*argp, uint64_t);
  if (specs->l) {
    oNum = (uint64_t)oNum;
  } else if (specs->h) {
    oNum = (uint16_t)oNum;
  } else {
    oNum = (uint32_t)oNum;
  }
  char *o_str = s21_utoa(oNum, 8);
  if (o_str) {
    s21_size_t length = s21_strlen(o_str);
    if (specs->cell) {
      length++;
    }
    if (!specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
    if (specs->dot && (*after_point > length)) {
      *after_point -= length;
      for (; *after_point; length++, *after_point -= 1) {
        s21_strcat(buffer, "0");
      }
    }
    if (specs->cell) {
      s21_strcat(buffer, "0");
    }
    s21_strcat(buffer, o_str);
    if (specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
  }
  *after_point = 6;
  s21_memset(specs, 0, sizeof(sprint_format));
  free(o_str);
}

static void format_x(char* buffer, const char* format, va_list* argp, sprint_format* specs,
              s21_size_t* after_point) {
  uint64_t xNum = va_arg(*argp, uint64_t);
  if (specs->l) {
    xNum = (uint64_t)xNum;
  } else if (specs->h) {
    xNum = (uint16_t)xNum;
  } else {
    xNum = (uint32_t)xNum;
  }
  char* x_str = s21_utoa(xNum, 16);
  if (x_str) {
    s21_size_t length = s21_strlen(x_str);
    if (specs->cell) {
      specs->len_space -= 2;
    }
    if (!specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
    if (specs->cell) {
      s21_strcat(buffer, "0");
      if (*format == 'X') {
        s21_strcat(buffer, "X");
      } else {
        s21_strcat(buffer, "x");
      }
    }
    if (specs->dot && (*after_point > length)) {
      *after_point -= length;
      for (; *after_point; length++, *after_point -= 1) {
        s21_strcat(buffer, "0");
      }
    }
    if (*format == 'X') {
      to_upAlpha(x_str);
    }
    s21_strcat(buffer, x_str);
    if (specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
  }
  *after_point = 6;
  s21_memset(specs, 0, sizeof(sprint_format));
  free(x_str);
}

static void format_p(char* buffer, va_list* argp, sprint_format* specs, s21_size_t* after_point) {
  void *p = va_arg(*argp, void*);
  char *p_str = s21_utoa((uint64_t)p, 16);
  if (p_str) {
    s21_size_t length = s21_strlen(p_str) + 2;
    if (!specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
    s21_strcat(buffer, "0x");
    if (specs->dot && (*after_point > length)) {
      *after_point -= length - 2;
      for (; *after_point; length++, *after_point -= 1) {
        s21_strcat(buffer, "0");
      }
    }
    s21_strcat(buffer, p_str);
    if (specs->minus && specs->space) {
      calculate_space(specs, *after_point, length);
      width_print(buffer, specs, length);
    }
  }
  *after_point = 6;
  s21_memset(specs, 0, sizeof(sprint_format));
  free(p_str);
}

static void format_n(char* buffer, va_list* argp, sprint_format* specs) {
  int *ret = va_arg(*argp, int*);
  *ret = (int)s21_strlen(buffer);
  s21_memset(specs, 0, sizeof(sprint_format));
}

int s21_sprintf(char* buffer, char* format, ...) {
  s21_errno = -1;
  va_list argp;
  sprint_format specs = {0};
  char* format_start = format;
  char str_buff[BUFF_SIZE] = {0};
  s21_size_t after_point = 6;
  va_start(argp, format);
  while (*format && s21_errno < 0) {
    if (*format == '%' || any_flag(&specs)) {
      format++;
      switch (*format) {
        case '%':
          s21_chrcat(str_buff, format);
          break;
        case '*':
          get_width_star(&specs, &argp, &after_point);
          continue;
        case '-':
          specs.minus = true;
          continue;
        case ' ':
          specs.no_sign = true;
          continue;
        case '#':
          specs.cell = true;
          continue;
        case '+':
          specs.plus = true;
          continue;
        case '.':
          after_point = 0;
          specs.dot = true;
          continue;
        case '0' ... '9':
          get_width_num(format, &specs, &after_point);
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
          format_u(str_buff, &argp, &specs, &after_point);
          break;
        case 's':
          format_s(str_buff, &argp, &specs, &after_point);
          break;
        case 'd':
        case 'i':
          format_d(str_buff, &argp, &specs, &after_point);
          break;
        case 'c':
          format_c(str_buff, &argp, &specs);
          break;
        case 'f':
        case 'g':
        case 'G':
        case 'E':
        case 'e':
          format_f(str_buff, format, &argp, &specs, &after_point);
          break;
        case 'o':
          format_o(str_buff, &argp, &specs, &after_point);
          break;
        case 'X':
        case 'x':
          format_x(str_buff, format, &argp, &specs, &after_point);
          break;
        case 'p':
          format_p(str_buff, &argp, &specs, &after_point);
          break;
        case 'n': {
          format_n(str_buff, &argp, &specs);
          break;
        }
        default:
          s21_strcat(str_buff, format_start);
          break;
      }
    } else {
      s21_chrcat(str_buff, format);
    }
    s21_strcpy(buffer, str_buff);
    format++;
  }
  va_end(argp);
  return (int)s21_strlen(buffer);
}