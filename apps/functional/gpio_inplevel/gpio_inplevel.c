#define APP_NAME "gpio_inplevel"

#include "stdio.h"
#include "libfwtest.h"

void usage()
{
    fprintf(stdout,"Usage: %s\n", APP_NAME);
}

int main(int argc, char **argv)
{
    fprintf(stdout,"%s\n", APP_NAME);
    dumpargs(argc, argv);
    return 0;
}
