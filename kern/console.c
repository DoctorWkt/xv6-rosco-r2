#include <xv6/types.h>
#include <xv6/defs.h>

// Console input and output.

extern void consputc(char ch);
extern char consgetc(void);

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

// Should we echo characters on DUART A?
// Should we convert CR to NL on DUART A?
// 1 means yes, 0 means no.
extern char DUART_ECHO_A;
extern char DUART_CRNL_A;

// The line buffer
#define INPUT_BUF 128
static char buf[INPUT_BUF];
static int bufposn=0;
static int bufeof=0;

// Read up to n characters from 
// the console and place them in dst.
int consoleread(char *dst, int n) {
  char ch;

  // We can only return INPUT_BUF characters
  if (n > INPUT_BUF) n= INPUT_BUF;

  // If we have hit EOF, return -1
  if (bufeof==1) {
    bufeof= bufposn= 0; return(-1);
  }

  // Loop trying to get n characters
  while (bufposn < n) {
    // Get the next character
    ch = consgetc();

    // Convert CR to NL as required
    if (DUART_CRNL_A && ch=='\r') ch='\n';

    // Backspace or DEL: remove the last character
    if (bufposn > 0 && (ch=='\b' || ch== 0x7f)) bufposn--;

    // Echo as required, and deal with backspaces and DELs
    if (DUART_ECHO_A) {
      if (ch=='\b' || ch== 0x7f) {
        consputc('\b'); consputc(' '); consputc('\b'); continue;
      } else
        consputc(ch);
    }

    // If it's a control-D, mark that we have hit EOF
    if (ch==0x04) { bufeof= 1; break; }
    
    // Save the character into the buffer
    buf[bufposn++] = ch;

    // Leave the loop on newlines
    if (ch=='\n') break;
  }

  // If the buffer is empty and we hit EOF,
  // clear the EOF flag
  if (bufeof && bufposn==0) bufeof=0;

  // Copy the line and return its length
  strncpy(dst, buf, bufposn);
  n= bufposn; bufposn=0; return(n);
}
