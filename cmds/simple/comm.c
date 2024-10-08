/* comm - select lines from two sorted files	Author: Martin C. Atkins */

/*
 *	This program was written by:
 *		Martin C. Atkins,
 *		University of York,
 *		Heslington,
 *		York. Y01 5DD
 *		England
 *	and is released into the public domain, on the condition
 *	that this comment is always included without alteration.
 */

#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFER_SIZE (512)
#define LINMAX (600)

struct file {
  char *name;			/* the file's name */
  int fd;			/* the file descripter */
  char buf[BUFFER_SIZE];		/* buffer storage */
  char *next;			/* the next character to read */
  char *endp;			/* the first invalid character */
  int seeneof;			/* an end of file has been seen */
} files[2];

char lines[2][LINMAX];

int colflgs[3] = {1, 2, 3};	/* number of tabs + 1: 0 => no column */

static char *umsg = "Usage: comm [-[123]] file1 file2\n";

int main(int argc, char *argv[]);
void usage(void);
void error(char *s, char *f);
void eopen(char *fn, struct file *file);
int getbuf(struct file *file);
int readline(int fno);
void comm(void);
void putcol(int col, char *buf);
void cpycol(int col);

int main(int argc, char *argv[])
{
  int cnt;
  if (argc > 1 && argv[1][0] == '-' && argv[1][1] != '\0') {
	char *ap;
	for (ap = &argv[1][1]; *ap; ap++) switch (*ap) {
		    case '1':
		    case '2':
		    case '3':
			cnt = *ap - '1';
			if (colflgs[cnt] == 0) break;
			colflgs[cnt] = 0;
			for (cnt++; cnt < 3; cnt++) colflgs[cnt]--;
			break;
		    default:	usage();
		}
	argc--;
	argv++;
  }
  if (argc != 3) usage();
  eopen(argv[1], &files[0]);
  eopen(argv[2], &files[1]);
  comm();
  exit(0);
}

void usage(void)
{

  fprintf(stderr,umsg);
  exit(1);
}

void error(char *s, char *f)
{
  fprintf(stderr, "comm: %s", s);
  if (f) fprintf(stderr, "%s", f);
  fprintf(stderr, "\n");
  exit(1);
}

void eopen(char *fn, struct file *file)
{
  file->name = fn;
  file->next = file->endp = &file->buf[0];
  file->seeneof = 0;
  if (fn[0] == '-' && fn[1] == '\0')
	file->fd = 0;
  else if ((file->fd = open(fn, O_RDONLY)) < 0)
	error("can't open ", fn);
}


int getbuf(struct file *file)
{
/* Get a buffer-full from the file.  Return true if no characters
 * were obtained because we are at end of file.
 */
  int n;

  if (file->seeneof) return(1);
  if ((n = read(file->fd, &file->buf[0], BUFFER_SIZE)) < 0)
	error("read error on ", file->name);
  if (n == 0) {
	file->seeneof++;
	return 1;
  }
  file->next = &file->buf[0];
  file->endp = &file->buf[n];
  return(0);
}


int readline(int fno)
{
/* Read up to the next '\n' character to buf.
 * Return a complete line, even if end of file occurs within a line.
 * Return false at end of file/
 */
  register struct file *file = &files[fno];
  char *buf = lines[fno];

  if (file->next == file->endp && getbuf(file)) return(0);
  while ((*buf++ = *file->next++) != '\n')
	if (file->next == file->endp && getbuf(file)) {
		*buf++ = '\n';
		*buf = '\0';
		return(1);
	}
  *buf = '\0';
  return(1);
}

void comm(void)
{
  register int res;

  if (!readline(0)) {
	cpycol(1);
	return;
  }
  if (!readline(1)) {
	putcol(0, lines[0]);
	cpycol(0);
	return;
  }
  for (;;) {
	if ((res = strcmp(lines[0], lines[1])) != 0) {
		res = res > 0;
		putcol(res, lines[res]);
		if (!readline(res)) {
			putcol(!res, lines[!res]);
			cpycol(!res);
			return;
		}
	} else {
		putcol(2, lines[0]);	/* files[1]lin == f2lin */
		if (!readline(0)) {
			cpycol(1);
			return;
		}
		if (!readline(1)) {
			putcol(0, lines[0]);
			cpycol(0);
			return;
		}
	}
  }

  /* NOTREACHED */
}

void putcol(int col, char *buf)
{
  int cnt;

  if (colflgs[col] == 0) return;
  for (cnt = 0; cnt < colflgs[col] - 1; cnt++) printf("\t");
  printf("%s", buf);
}

void cpycol(int col)
{
  if (colflgs[col]) while (readline(col))
		putcol(col, lines[col]);
}
