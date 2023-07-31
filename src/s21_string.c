#include "s21_string.h"

int s21_errno;

char *s21_strstr(const char *haystack, const char *needle) {
  const char *ptr1, *ptr2;
  char *str_str = s21_NULL;
  ptr2 = needle;
  if (*ptr2 == '\0') {
    str_str = (char *)haystack;
  } else {
    for (;(*haystack) && !str_str; haystack++) {
      if (*haystack == *ptr2) {
        ptr1 = haystack;
        do {
          if (*ptr2 == '\0') {
            str_str = (char *)haystack;
            break;
          }
        } while (*ptr1++ == *ptr2++);
        ptr2 = needle;
      }
    }
  }
  return str_str;
}

s21_size_t s21_strlen(const char *str) {
  const char *s = str;
  for (; *s; s++)
    ;
  return s - str;
}

char *s21_strrchr(const char *str, int c) {
  char *save = s21_NULL;
  int ch;
  for (;(ch = (int) *str); str++) {
    if (ch == c) {
      save = (char *) str;
    }
  }
  return save;
}

int s21_strcmp(const char *str1, const char *str2) {
  for (;(*str1) && (*str1 == *str2); str1++, str2++)
    ;
  return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
  int cmp = 0;
  if (n) {
    do {
      if (*str1 != *str2++) {
        cmp = (*(unsigned char *) str1 - *(unsigned char *) --str2);
        break;
      }
    } while ((*str1++) && (--n));
  }
  return cmp;
}

char *s21_strcpy(char* restrict dest, const char* restrict src) {
  char *ptr = dest;
  for (; (*dest = *src); ++dest, ++src)
    ;
  return ptr;
}

char *s21_strncpy(char* restrict dest, const char* restrict src, s21_size_t n) {
  char *ptr = dest;
  for (;n != 0; src++, n--) {
    if (!(*dest++ = *src)) {
      break;
    }
  }
  return ptr;
}

char *s21_strcat(char *dest, const char *src) {
  char *ptr = dest;
  for (; *dest; ++dest)
    ;
  for (; (*dest = *src); dest++, src++)
    ;
  return ptr;
}

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  char *ptr = dest;
  for (; *dest; ++dest)
    ;
  if (n) {
    for (; n != 0; src++, n--) {
      if (!(*dest++ = *src)) {
        break;
      }
    }
  }
  *dest = '\0';
  return ptr;
}

char *s21_strchr(const char *str, int c) {
  char *save = s21_NULL;
  int ch;
  for (;(ch = (int) *str); str++) {
    if (ch == c) {
      save = (char *) str;
      break;
    }
  }
  return save;
}

s21_size_t s21_strspn(const char *str1, const char *str2) {
  const char *p = str1, *spanp = str2;
  char c = *p++, sc;
  while ((sc = *spanp++)) {
    if (sc == c) {
      c = *p++;
      spanp = str2;
    }
  }
  return p - 1 - str1;
}

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  const char *p, *spanp;
  char c, sc;
  p = str1;
  do {
    c = *p++;
    spanp = str2;
    do {
      if ((sc = *spanp++) == c) {
        break;
      }
    } while (sc);
  } while(sc != c);
  return p - 1 - str1;
}

char *s21_strtok(char *str, const char *delim) {
  static char *olds;
  char *end;
  if (!str) {
    str = olds;
  }
  if (!*str) {
    olds = str;
    str = s21_NULL;
  } else {
    str += s21_strspn(str, delim);
    if (!*str) {
      olds = str;
      str = s21_NULL;
    } else {
      end = str + s21_strcspn(str, delim);
      if (!*end) {
        olds = end;
      } else {
        *end = '\0';
        olds = end + 1;
      }
    }
  }
  return str;
}

char *s21_strpbrk(const char *str1, const char *str2) {
  const char *scanp;
  char c, sc, *str = s21_NULL;
  while ((c = *str1++) && !str) {
    scanp = str2;
    while ((sc = *scanp++)) {
      if (sc == c) {
        str = ((char *) (str1 - 1));
        break;
      }
    }
  }
  return str;
}

void *s21_memcpy(void* restrict dest, const void* restrict src, s21_size_t n) {
  char *c_dest = (char *)dest;
  const char *c_src = (const char *)src;
  for (s21_size_t i = 0; i < n; i++) {
    c_dest[i] = c_src[i];
  }
  return dest;
}

void *s21_memmove(void *dest, const void *src, s21_size_t n) {
  char *c_dest = (char *)dest;
  const char *c_src = (const char *)src;
  if (c_src < c_dest) {
    for (s21_size_t i = n; i > 0; i--) {
      c_dest[i - 1] = c_src[i - 1];
    }
  } else {
    for (s21_size_t i = 0; i < n; i++) {
      c_dest[i] = c_src[i];
    }
  }
  return dest;
}

int s21_memcmp(const void *s1, const void *s2, s21_size_t n) {
  int cmp = 0;
  const unsigned char *c1 = (const unsigned char *)s1;
  const unsigned char *c2 = (const unsigned char *)s2;
  for (s21_size_t i = 0; i < n; i++) {
    if (c1[i] != c2[i]) {
      cmp = c1[i] - c2[i];
      break;
    }
  }
  return cmp;
}

void *s21_memchr(const void *s, int c, s21_size_t n) {
  void* ptr = s21_NULL;
  const unsigned char *cs = (const unsigned char *)s;
  const unsigned char uc = (unsigned char)c;
  for (s21_size_t i = 0; i < n; i++) {
    if (cs[i] == uc) {
      ptr = (void *)(cs + i);
      break;
    }
  }
  return ptr;
}

void *s21_memset(void *s, int c, s21_size_t n) {
  unsigned char *cs = (unsigned char *)s;
  const unsigned char uc = (unsigned char)c;
  for (s21_size_t i = 0; i < n; i++) {
    cs[i] = uc;
  }
  return s;
}

char *s21_strerror(int errnum) {
#if defined(__APPLE__)
  #define MAX_ERRLIST 107
#define MIN_ERRLIST (-2)
const char *errors[] = {"Undefined error: 0",
                        "Operation not permitted",
                        "No such file or directory",
                        "No such process",
                        "Interrupted system call",
                        "Input/output error",
                        "Device not configured",
                        "Argument list too long",
                        "Exec format error",
                        "Bad file descriptor",
                        "No child processes",
                        "Resource deadlock avoided",
                        "Cannot allocate memory",
                        "Permission denied",
                        "Bad address",
                        "Block device required",
                        "Resource busy",
                        "File exists",
                        "Cross-device link",
                        "Operation not supported by device",
                        "Not a directory",
                        "Is a directory",
                        "Invalid argument",
                        "Too many open files in system",
                        "Too many open files",
                        "Inappropriate ioctl for device",
                        "Text file busy",
                        "File too large",
                        "No space left on device",
                        "Illegal seek",
                        "Read-only file system",
                        "Too many links",
                        "Broken pipe",
                        "Numerical argument out of domain",
                        "Result too large",
                        "Resource temporarily unavailable",
                        "Operation now in progress",
                        "Operation already in progress",
                        "Socket operation on non-socket",
                        "Destination address required",
                        "Message too long",
                        "Protocol wrong type for socket",
                        "Protocol not available",
                        "Protocol not supported",
                        "Socket type not supported",
                        "Operation not supported",
                        "Protocol family not supported",
                        "Address family not supported by protocol family",
                        "Address already in use",
                        "Can't assign requested address",
                        "Network is down",
                        "Network is unreachable",
                        "Network dropped connection on reset",
                        "Software caused connection abort",
                        "Connection reset by peer",
                        "No buffer space available",
                        "Socket is already connected",
                        "Socket is not connected",
                        "Can't send after socket shutdown",
                        "Too many references: can't splice",
                        "Operation timed out",
                        "Connection refused",
                        "Too many levels of symbolic links",
                        "File name too long",
                        "Host is down",
                        "No route to host",
                        "Directory not empty",
                        "Too many processes",
                        "Too many users",
                        "Disc quota exceeded",
                        "Stale NFS file handle",
                        "Too many levels of remote in path",
                        "RPC struct is bad",
                        "RPC version wrong",
                        "RPC prog. not avail",
                        "Program version wrong",
                        "Bad procedure for program",
                        "No locks available",
                        "Function not implemented",
                        "Inappropriate file type or format",
                        "Authentication error",
                        "Need authenticator",
                        "Device power is off",
                        "Device error",
                        "Value too large to be stored in data type",
                        "Bad executable (or shared library)",
                        "Bad CPU type in executable",
                        "Shared library version mismatch",
                        "Malformed Mach-o file",
                        "Operation canceled",
                        "Identifier removed",
                        "No message of desired type",
                        "Illegal byte sequence",
                        "Attribute not found",
                        "Bad message",
                        "EMULTIHOP (Reserved)",
                        "No message available on STREAM",
                        "ENOLINK (Reserved)",
                        "No STREAM resources",
                        "Not a STREAM",
                        "Protocol error",
                        "STREAM ioctl timeout",
                        "Operation not supported on socket",
                        "Policy not found",
                        "State not recoverable",
                        "Previous owner died",
                        "Interface output queue is full"};
#elif defined(__linux__)
  #define MAX_ERRLIST 134
#define MIN_ERRLIST -1
static const char *errors[] = {
    "Success",
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "Input/output error",
    "No such device or address",
    "Argument list too long",
    "Exec format error",
    "Bad file descriptor",
    "No child processes",
    "Resource temporarily unavailable",
    "Cannot allocate memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Device or resource busy",
    "File exists",
    "Invalid cross-device link",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "Too many open files in system",
    "Too many open files",
    "Inappropriate ioctl for device",
    "Text file busy",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Numerical argument out of domain",
    "Numerical result out of range",
    "Resource deadlock avoided",
    "File name too long",
    "No locks available",
    "Function not implemented",
    "Directory not empty",
    "Too many levels of symbolic links",
    "Unknown error 41",
    "No message of desired type",
    "Identifier removed",
    "Channel number out of range",
    "Level 2 not synchronized",
    "Level 3 halted",
    "Level 3 reset",
    "Link number out of range",
    "Protocol driver not attached",
    "No CSI structure available",
    "Level 2 halted",
    "Invalid exchange",
    "Invalid request descriptor",
    "Exchange full",
    "No anode",
    "Invalid request code",
    "Invalid slot",
    "Unknown error 58",
    "Bad font file format",
    "Device not a stream",
    "No data available",
    "Timer expired",
    "Out of streams resources",
    "Machine is not on the network",
    "Package not installed",
    "Object is remote",
    "Link has been severed",
    "Advertise error",
    "Srmount error",
    "Communication error on send",
    "Protocol error",
    "Multihop attempted",
    "RFS specific error",
    "Bad message",
    "Value too large for defined data type",
    "Name not unique on network",
    "File descriptor in bad state",
    "Remote address changed",
    "Can not access a needed shared library",
    "Accessing a corrupted shared library",
    ".lib section in a.out corrupted",
    "Attempting to link in too many shared libraries",
    "Cannot exec a shared library directly",
    "Invalid or incomplete multibyte or wide character",
    "Interrupted system call should be restarted",
    "Streams pipe error",
    "Too many users",
    "Socket operation on non-socket",
    "Destination address required",
    "Message too long",
    "Protocol wrong type for socket",
    "Protocol not available",
    "Protocol not supported",
    "Socket type not supported",
    "Operation not supported",
    "Protocol family not supported",
    "Address family not supported by protocol",
    "Address already in use",
    "Cannot assign requested address",
    "Network is down",
    "Network is unreachable",
    "Network dropped connection on reset",
    "Software caused connection abort",
    "Connection reset by peer",
    "No buffer space available",
    "Transport endpoint is already connected",
    "Transport endpoint is not connected",
    "Cannot send after transport endpoint shutdown",
    "Too many references: cannot splice",
    "Connection timed out",
    "Connection refused",
    "Host is down",
    "No route to host",
    "Operation already in progress",
    "Operation now in progress",
    "Stale file handle",
    "Structure needs cleaning",
    "Not a XENIX named type file",
    "No XENIX semaphores available",
    "Is a named type file",
    "Remote I/O error",
    "Disk quota exceeded",
    "No medium found",
    "Wrong medium type",
    "Operation canceled",
    "Required key not available",
    "Key has expired",
    "Key has been revoked",
    "Key was rejected by service",
    "Owner died",
    "State not recoverable",
    "Operation not possible due to RF-kill",
    "Memory page has hardware error"};
#endif
  char *res = s21_NULL;
  static char not_found[] = "Unknown error";
  if (errnum <= MIN_ERRLIST || errnum >= MAX_ERRLIST) {
    res = not_found;
  } else {
    res = (char *)errors[errnum];
  }
  return res;
}
