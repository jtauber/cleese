#include "Python.h"

int
PyOS_snprintf(char *str, size_t size, const char *format, ...)
{
  int rc;
  va_list va;

  va_start(va, format);
  rc = va_snprintf(str, size, format, va);
  va_end(va);
  return rc;
}
