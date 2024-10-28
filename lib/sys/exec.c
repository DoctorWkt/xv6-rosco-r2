#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define	PTRSIZE	(sizeof(char *))

#ifdef _ANSI
int execl(const char *name, const char *arg, ...)
#else
int execl(name)
char *name;
#endif
{
  va_list argp;
  int result;

  va_start(argp, name);

  /* The following cast of argp is not portable.  Doing it right by copying
   * the args to a true array will cost as much as ARG_MAX bytes of space.
   */
  result = execv(name, (char **) argp);
  va_end(argp);
  return(result);
}


#ifdef _ANSI
int execle(const char *name, const char *arg, ...)
#else
int execle(name)
char *name;
#endif
{
  va_list argp;
  char **p;
  int result;

  va_start(argp, name);
  
  /* The following cast of argp is not portable, as for execl(). */
  p = (char **) argp;
  while (*p++ != NULL)
	;			/* null statement */
  result = execv(name, (char **) argp);
  va_end(argp);
  return(result);
}

int execve(path, argv, envp)
_CONST char *path;		/* pointer to name of file to be executed */
char * _CONST argv[];		/* pointer to argument array */
char * _CONST envp[];		/* pointer to environment */
{
  return(execv(path, argv));
}
