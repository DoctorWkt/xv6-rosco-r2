/*
 *	Provide the wrapper to rl_edit for GNU readline
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

extern int rl_edit(int fd, int ofd, const char *prompt, char *input, size_t len);

char *readline(const char *prompt)
{
    int len;
    char *p = (char *)malloc(256);
    fflush(stdout);
    if (p == NULL)
        return NULL;
    len = rl_edit(0, 1, prompt, p, 255);
    if (len < 0) {
        free(p);
        return NULL;
    }
    p[len] = 0;
    return p;
}
