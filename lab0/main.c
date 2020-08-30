#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>

void usage(void)
{
    fprintf(stderr, "Incorrect usage\n");
}

int main (int argc, char **argv)
{

    /* Argument Parsing */
      int opts;
      char algo[6];

      static struct option long_options[] = {
          {"name", no_argument, NULL, 'n'},
          {"alg", required_argument, NULL, 'a'}
      };
      while ((opts = getopt_long (argc, argv, "nao:", long_options, NULL)) != -1) {
          switch (opts) {
              case 'o':
                  printf("Output file is %s\n", optarg);
                  break;
              case 'n':
                  printf("Name - Atharva Nandanwar\n");
                  break;
              case 'a':
                  memcpy(algo, optarg, 6);
                  printf("Algorithm is %s\n", algo);
                  break;
              default:
                  usage();
          }
      }
      /* Argument Parsing Ends */

      /* Quick Sort implementation */
      if (!strcmp(algo, "quick")) {
          printf("quick sort\n");
      }
      /* Quick Sort Implementation Ends */
      /* Merge Sort Implementation */
      else if (!strcmp(algo, "merge")) {
          printf("merge sort\n");
      }
      /* Merge Sort Implementation Ends */
      return 0;
}
