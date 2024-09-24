#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

struct passwd *getpwent(void) {
  struct passwd *p;

  p= (struct passwd *)malloc(sizeof(struct passwd));
  if (p==NULL) return(NULL);

  p->pw_name= strdup("root");
  p->pw_passwd= strdup("Geheim");		// Minix :-)
  p->pw_uid= 0;
  p->pw_gid= 0;
  p->pw_gecos= strdup("Root user");
  p->pw_dir= strdup("/");
  p->pw_shell= strdup("/bin/sh");
  return(p);
}

struct passwd *getpwnam(const char *name) {
  return(getpwent());
}

struct passwd *getpwuid(uid_t uid) {
  return(getpwent());
}
