#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#define eOK                          0

#define eOS_REG_OPEN_FILE            100
#define eOS_REG_LOAD_LIST            200
#define eOS_REG_LOAD_LIST_FROM_FILE  300

/*
 *  Message Logging
 */

long gErrorCount = 0;
char **gErrorMessage = NULL;

static void mis_LogError(char *fmt, va_list args)
{
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    return;
}

static void misLogError(char *fmt, ...)
{
    long ii;

    va_list args;

    /*
     *  DUMP THE STACKED ERROR MESSAGES
     */

    /* Dump all our stacked errors. */
    for (ii = 0; ii < gErrorCount; ii++)
    {
	mis_LogError(gErrorMessage[ii], NULL);
	free(gErrorMessage[ii]);
    }

    free(gErrorMessage);
    gErrorMessage = NULL;

    gErrorCount = 0;

    /*
     *  DUMP THIS ERROR MESSAGE
     */

    va_start(args, fmt);
    mis_LogError(fmt, args);
    va_end(args);

    return;
}

static void misStackError(char *fmt, ...)
{
    long length;

    va_list args;

    gErrorCount++;

    gErrorMessage = realloc(gErrorMessage, gErrorCount * sizeof(char *));

    /*
    va_start(args, fmt);
    length = mis_StackError(gErrorMessage[gErrorCount-1], fmt, args);
    va_end(args);
    */

    length = 100;

    gErrorMessage[gErrorCount-1] = calloc(1, length + 1);

    va_start(args, fmt);
    vsprintf(gErrorMessage[gErrorCount-1], fmt, args);
    va_end(args);
}

/*
 *  Registry Support
 */

static long sLoadRegistryListFromFile(char *pathname)
{
    FILE *infile;

    infile = fopen(pathname, "r");
    if (!infile)
    {
        misStackError("fopen(%s): %s", pathname, strerror(errno));
        misStackError("sLoadRegistryListFromFile: Could not open registry file");
        return eOS_REG_OPEN_FILE;
    }

    return eOK;
}

long sLoadRegistryList(void)
{
    long status;

    status = sLoadRegistryListFromFile("myfile");
    if (status != eOK)
    {
        misStackError("sLoadRegistryList: Could not load registry list");
        return eOS_REG_LOAD_LIST_FROM_FILE;
    }
}

long osGetRegistryValue(void)
{
    long status;

    status = sLoadRegistryList( );
    if (status != eOK)
    {
        misStackError("osGetRegistryValue: Could not load registry list");
        return eOS_REG_LOAD_LIST;
    }
}

/*
 *  Main
 */

int main(int argc, char *argv[])
{
    long status;

    status = osGetRegistryValue( );
    if (status != eOK)
        misLogError("Could not get registry value");

    fprintf(stderr, "\n");

    status = osGetRegistryValue( );
    if (status != eOK)
        misLogError("Could not get registry value");


    exit(0);
}

