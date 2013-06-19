/* $Id: example.c,v 1.1.1.1 2008/01/06 03:24:00 holger Exp $ */

#if HAVE_CONFIG_H
#include <config.h>
#endif	/* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>	/* For free(3). */
#include "system.h"

int
main(void)
{
	FILE *fp;
	char *buf;
	unsigned int random;

	if ((fp = fopen("/dev/urandom", "r")) == NULL &&
	    (fp = fopen("/dev/random", "r")) == NULL) {
		perror("Cannot open random device");
		return 1;
	}
	if (fread(&random, sizeof(random), 1, fp) != 1) {
		(void)fputs("Cannot read random device.", stderr);
		return 1;
	}
	if (fclose(fp) != 0) {
		perror("Cannot close random device");
		return 1;
	}

	/* We never heard of printf(3), so we use asprintf(3)/puts(3) :-) */
	if (asprintf(&buf, "Random %zu-bit integer: %#.*x",
	    sizeof(random) * 8, (int)sizeof(random) * 2, random) < 0) {
		perror("asprintf(3) failed");
		return 1;
	}
	(void)puts(buf);
	free(buf);
	return 0;
}
