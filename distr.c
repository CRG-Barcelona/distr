/* dist - with a bunch of numbers streaming into stdin, find some summary stats */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <float.h>
#include <jkweb/common.h>
#include <jkweb/options.h>
#include <jkweb/linefile.h>
#include <jkweb/sqlNum.h>
#include <beato/stuff.h>

void usage()
/* Explain usage and exit. */
{
errAbort(
  "dist - Read one-per-line stdin stream of numbers and report summary stats\n"
  "usage:\n"
  "   dist [options]"
  "options:\n"
  "   -h       this help message\n"
  );
}

static struct optionSpec options[] = 
{
    {"h", OPTION_BOOLEAN},
   {NULL, 0},
};

void dist(void)
/* dist - main */
{
    struct lineFile *lf = lineFileOpen("/dev/stdin", TRUE);
    char *line;
    int len = 0;
    struct slDouble *list = NULL, *dd;
    double *array;
    int size = 0;
    int i;
    double max = -DBL_MAX;
    double min = DBL_MAX;
    double mean;
    double sum = 0;
    while (lineFileNext(lf, &line, &len))
    {
	if (len > 0)
	{
	    struct slDouble *d = slDoubleNew(sqlDouble(line));
	    slAddHead(&list, d);
	    size++;
	}
    }
    slReverse(&list);
    AllocArray(array, size);
    for (i = 0, dd = list; (dd != NULL) && (i < size); dd = dd->next, i++)
    {
	array[i] = dd->val;
	if (array[i] > max)
	    max = array[i];
	if (array[i] < min)
	    min = array[i];
	sum += array[i];
    }
    mean = sum/size;
    slFreeList(&list);
    doubleWithNASort(size, array);
    printf("#(1)min\t(2)max\t(3)mean\t(4)sum\t(5)1st_10%%\t(6)1st_25%%\t(7)median\t(8)3rd_25%%\t(9)9th_10%%\n");
    printf("%f\t%f\t%f\t%f\t", min, max, mean, sum); 
    printf("%f\t", doubleWithNAInvQuantAlreadySorted(size, array, 10, TRUE));
    printf("%f\t", doubleWithNAInvQuantAlreadySorted(size, array, 4, TRUE));
    printf("%f\t", doubleWithNAMedianAlreadySorted(size, array));
    printf("%f\t", doubleWithNAInvQuantAlreadySorted(size, array, 4, FALSE));
    printf("%f\n", doubleWithNAInvQuantAlreadySorted(size, array, 10, FALSE));
    freeMem(array);
}

int main(int argc, char *argv[])
/* Process command line. */
{
optionInit(&argc, argv, options);
if (argc != 1)
    usage();
dist();
return 0;
}
