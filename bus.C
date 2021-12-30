
#include <fjtag.H>

void Bus::print_flash_info()
{
 if (flash) {
  const char *b = flash->info();
  char t[128], *t1 = t;
  while (*b) {
    *t1 = *b;
    if (*b == '\n') {
      *t1 = 0;
      message(t);
      t1 = t;
      b++;
      continue;
    }
    b++; t1++;
  }
  if (t1 != t) {
    *t1 = *b;
    message(t);
  }
 }
}
