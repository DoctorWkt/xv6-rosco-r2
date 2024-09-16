#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ls: List directory contents.		Author: Warren Toomey */

/* External variables. */
extern int optind;
extern char *optarg;

int longoutput=0;		// Do a long (-l) output
int showdots=0;			// Show dot files
int showinums=0;		// Show i-numbers not link counts
int dircontents=1;		// Show the contents of a directory

char buf[1024];			// Used to make filename for stat

void listone(char *entry, struct stat *sbptr)
{
  char ftype;

  // Lose the year on the time string
  char *timestring= ctime(&(sbptr->st_mtime));
  timestring[ strlen(timestring) - 6 ]= '\0';

  if (longoutput) {
    // Print the type of entry as the first character
    switch (sbptr->st_mode& S_IFMT) {
      case S_IFDIR: ftype='d'; break;
      case S_IFREG: ftype='-'; break;
      case S_IFBLK:
      case S_IFCHR: ftype='c'; break;
      default: ftype=' ';
    }

    if (ftype=='c')
      printf("%crwxrwxrwx %5d root root %3d,%2d %s %s\n", 
	ftype, (showinums ? sbptr->st_ino : sbptr->st_nlink),
	major(sbptr->st_dev), minor(sbptr->st_dev), timestring, entry);
    else
      printf("%crwxrwxrwx %5d root root %6ld %s %s\n", 
	ftype, (showinums ? sbptr->st_ino : sbptr->st_nlink),
	sbptr->st_size, timestring, entry);
  } else {
    // Just print out the name
    puts(entry);
  }
}

// We keep a array structure of dirents and their stat buffers
typedef struct {
  struct dirent dent;
  struct stat sb;
} Namestat;

#define NLISTSIZE 200
Namestat namelist[NLISTSIZE];

// Compare two Namestat structs using the entry names
int namecmp(const void *a, const void *b)
{
  Namestat *aa= (Namestat *)a;
  Namestat *bb= (Namestat *)b;

  return(strcmp(aa->dent.d_name, bb->dent.d_name));
}

// List the entry (if a file), or its contents (if a directory)
void listmany(char *entry)
{
  DIR *D;
  struct dirent *dent;
  struct stat sb;
  int i,count=0;

  // Ensure the entry exists
  if (stat(entry, &sb)==-1) {
    printf("%s: non-existent\n", entry);
    return;
  }

  // It's not a directory, just print it out
  if (!S_ISDIR(sb.st_mode)) {
    listone(entry, &sb);
    return;
  } else {
    // It's a directory, deal with all of it
    // Only list the directory, not its contents
    if (dircontents==0) {
      listone(entry, &sb);
      return;
    }

    // Open the directory
    D= opendir(entry);
    if (D==NULL) {
      printf("%s: unable to opendir\n", entry);
      return;
    }

    // Process each entry
    while ((dent=readdir(D))!=NULL) {

      // Skip empty directory entries
      if (dent->d_name[0]=='\0') continue;

      // Skip dot files
      if ((showdots==0) && (dent->d_name[0]=='.')) continue;

      // Copy the directory name into the buffer and then
      // append the file's name, so we can stat the name.
      // Don't copy the directory name if it is ".".
      if (!strcmp(entry, ".")) {
	buf[0]= 0;
      } else {
	strcpy(buf, entry);
	strcat(buf, "/");
      }
      strcat(buf, dent->d_name);

      // Get the file's stats
      if (stat(buf, &sb)==-1) {
        printf("%s: non-existent\n", buf);
        continue;
      }

      // and add the file to the array
      memcpy(&(namelist[count].dent), dent, sizeof(struct dirent));
      memcpy(&(namelist[count].sb), &sb, sizeof(sb));
      count++;

    }

    // Sort the array into name order
    qsort(namelist, count, sizeof(Namestat), namecmp);

    // Print each one out
    for (i=0; i < count; i++) 
      listone(namelist[i].dent.d_name, &(namelist[i].sb));

    closedir(D);
  }
}

int main(int argc, char *argv[])
{
  int i,opt;                      /* option letter from getopt() */

  /* Process any command line flags. */
  while ((opt = getopt(argc, argv, "ilad")) != EOF) {
        if (opt == 'l')
                longoutput = 1;
        if (opt == 'a')
                showdots = 1;
        if (opt == 'i')
                showinums = 1;
        if (opt == 'd')
                dircontents = 0;
  }

  // No further arguments, list the current directory
  if (optind==argc) {
    listmany("."); exit(0);
  }

  // Otherwise, process the arguments left
  for (i=optind; i<argc; i++) 
    listmany(argv[i]);
  exit(0);
}
