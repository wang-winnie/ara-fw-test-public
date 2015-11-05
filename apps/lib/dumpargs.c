#include "stdio.h"

void dumpargs(int argc, char **argv)
{
    int n;
    for (n=0; n<argc; n++)
	fprintf(stdout, "argv[%d]='%s'\n",n,argv[n]);
}
