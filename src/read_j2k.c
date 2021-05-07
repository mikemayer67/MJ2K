#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "mamj2k.h"

int main(int argc,char **argv)
{
  char *filename = NULL;
  char *csv_root = NULL;
  struct stat file_stat;

  for(int i=1; i<argc; ++i)
  {
    if(strcmp(argv[i],"-r")==0)
    {
      if( i+1 == argc )
      {
        fprintf(stderr,"\nSorry:: Option -r requires an argument\n\n");
        exit(1);
      }
      else
      {
        ++i;
        char *end = NULL;
        int rc = strtol(argv[i],&end,0);
        if( (end < argv[i] + strlen(argv[i])) || rc < 0 )
        {
          fprintf(stderr,"\nSorry:: Argument to -r must be a postiive integer\n\n");
          exit(1);
        }
        mamj2k_cp_reduce = rc;
      }
    }
    else if(strcmp(argv[i],"-l")==0)
    {
      if( i+1 == argc )
      {
        fprintf(stderr,"\nSorry:: Option -l requires an argument\n\n");
        exit(1);
      }
      else
      {
        ++i;
        char *end = NULL;
        int rc = strtol(argv[i],&end,0);
        if( (end < argv[i] + strlen(argv[i])) || rc < 0 )
        {
          fprintf(stderr,"\nSorry:: Argument to -l must be a postiive integer\n\n");
          exit(1);
        }
        mamj2k_cp_layer = rc;
      }
    }
    else if(filename == NULL)
    {
      filename = argv[i];
      int rc = stat(filename,&file_stat);
      if( rc != 0 )
      {
        fprintf(stderr,"\nSorry:: Cannot read %s (%s)\n\n",filename,strerror(errno));
        exit(1);
      }
    }
    else if(csv_root == NULL)
    {
      csv_root = argv[i];
    }
    else
    {
      fprintf(stderr,"\nUsage argv[0]: [-r #][-l #] j2k_filename [csv_root_filename]\n\n");
      exit(1);
    }

  }

  if(filename == NULL)
  {
    fprintf(stderr,"\nUsage argv[0]: [-r #][-l #] j2k_filename [csv_root_filename]\n\n");
    exit(1);
  }

  if(csv_root == NULL)
  {
    const char *ext = strrchr(filename,'.');
    int n = ext-filename;
    csv_root = (char *)calloc(n+1, sizeof(char));
    strncpy(csv_root, filename, n);
  }


  const char *version = mj2k_opj_version();
  printf("\nMJ2K is wrapping OpenJPEG VERSION: %s\n\n",version);

  mj2k_image_t *image = mj2k_read_j2k(filename);

  if( image == NULL ) {
    fprintf(stderr,"\nSorry::Failed to parse image\n\n");
    exit(1);
  }

  for(int i=0; i<image->ncomp; ++i)
  {
    mj2k_comp_t *comp = &(image->comp[i]);

    char *csv_filename = calloc(strlen(csv_root) + 10, sizeof(char));
    if(image->ncomp > 1) {
      sprintf(csv_filename,"%s_%d.csv",csv_root,i+1);
    } else {
      sprintf(csv_filename,"%s.csv",csv_root);
    }

    printf("Creating %s\n",csv_filename);

    FILE *fp = fopen(csv_filename,"w");

    int nrows = comp->h;
    int ncols = comp->w;

    mj2k_pixel_t *p = comp->pixels;

    const char *delim = "";
    for(int row=0; row<nrows; ++row) {
      for(int col=0; col<ncols; ++col) {
        fprintf(fp,"%s%d",delim,*(p++));
        delim = ", ";
      }
      delim = "\n";
    }

    fclose(fp);
    free(csv_filename);
  }

  mj2k_free_image(image);

  return 0;
}

