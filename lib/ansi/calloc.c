/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/calloc.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */
#include	<stdlib.h>

#define ALIGN(x)	(((x) + (sizeof(size_t) - 1)) & ~(sizeof(size_t) - 1))

void *
calloc(size_t nelem, size_t elsize)
{
	register char *p;
	register size_t *q;
	size_t size = ALIGN(nelem * elsize);

	p = malloc(size);
	if (p == NULL) return NULL;
	q = (size_t *) (p + size);
	while ((char *) q > p) *--q = 0;
	return p;
}

