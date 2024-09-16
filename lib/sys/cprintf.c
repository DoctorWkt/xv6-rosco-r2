#include <xv6/types.h>
#include <xv6/defs.h>

extern void consputc(char ch);

static void printint(int xx, int base, int sign) {
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if (sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);

  if (sign)
    buf[i++] = '-';

  while (--i >= 0)
    consputc(buf[i]);
}

//PAGEBREAK: 50

void cprintf(char *fmt, ...) {
  int i, c;
  uint *argp;
  char *s;

  if (fmt == 0)
    panic("null fmt");

  argp = (uint *) (void *) (&fmt + 1);
  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 0)
      break;
    switch (c) {
    case 'c':
      consputc((char) (*argp & 0xff)); argp++;
      break;
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if ((s = (char *) *argp++) == 0)
	s = "(null)";
      for (; *s; s++)
	consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }
}

void panic(char *s) {
  // WKT cli();
  cprintf("panic: %s\n", s);
  while (1);
}
