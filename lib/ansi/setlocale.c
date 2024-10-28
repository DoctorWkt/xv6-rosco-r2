/*
 * setlocale - set the programs locale
 */
/* $Header: /home/wkt/Minix_1.6.25/lib/ansi/RCS/setlocale.c,v 1.1 2024/10/25 01:13:18 wkt Exp $ */

#include	<locale.h>
#include	<string.h>

struct lconv _lc;

char *
setlocale(int category, const char *locale)
{
	if (!locale) return "C";
	if (*locale && strcmp(locale, "C")) return (char *)NULL;
	
	switch(category) {
	case LC_ALL:
	case LC_CTYPE:
	case LC_COLLATE:
	case LC_TIME:
	case LC_NUMERIC:
	case LC_MONETARY:
		return *locale ? (char *)locale : "C";
	default:
		return (char *)NULL;
	}
}
