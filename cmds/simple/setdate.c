#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Set the time - Warren Toomey, some code from FUZIX (Alan Cox)

static char dim[13] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


int main() {
  long tim;
  struct tm *tm = (struct tm *) calloc(1, sizeof(struct tm));
  char newval[128];
  int y, m, d, h, min, s;

  while (1) {
    printf("Enter new date (yyyy-mm-dd): ");
    fflush(stdout);
    fgets(newval, 128, stdin);
    if (*newval == '\r' || *newval == '\n')
      continue;
    if (sscanf(newval, "%d-%d-%d", &y, &m, &d) != 3 ||
	y < 1970 || m < 1 || m > 12 || d < 1 || d > dim[m]) {
      fputs("Invalid date.\n", stderr);
      continue;
    }
    break;
  }

  while (1) {
    printf("Enter new time (hh:mm:ss): ");
    fflush(stdout);
    fgets(newval, 128, stdin);
    if (*newval == '\r' || *newval == '\n')
      continue;
    if (sscanf(newval, "%d:%d:%d", &h, &min, &s) < 2 ||
	h < 0 || h > 23 || min < 0 || min > 59 || s < 0 || s > 59) {
      fputs("Invalid time.\n", stderr);
      continue;
    }
    break;
  }

  tm->tm_hour = h;
  tm->tm_min = min;
  tm->tm_sec = s;

  tm->tm_year = y - 1900;
  tm->tm_mon = m - 1;
  tm->tm_mday = d;

  tim = mktime(tm);
  printf("New time is %s\n", ctime(&tim));
  stime(tim);
  return (0);
}
