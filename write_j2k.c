#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "mamj2k.h"

int main(int argc,char **argv)
{
  char *rd_filename = NULL;
  char *j2k_filename = NULL;

  int lossless   = -1;
  int num_layers = -1;

  for(int i=1; i<argc; ++i)
  {
    if(strcmp(argv[i],"-nl")==0)
    {
      switch(lossless)
      {
        case -1: 
          lossless = 1;
          break;
        case 1:
          fprintf(stderr,"Note:: You specified -nl multiple times\n");
          break;
        case 0:
          fprintf(stderr,"\nSorry:: You cannot specify both -nl and -vl\n");
          return 1;
      }
    }
    else if(strcmp(argv[i],"-vl")==0)
    {
      switch(lossless)
      {
        case -1: 
          lossless = 0;
          break;
        case 0:
          fprintf(stderr,"Note:: You specified -vl multiple times\n");
          break;
        case 1:
          fprintf(stderr,"\nSorry:: You cannot specify both -nl and -vl\n");
          return 1;
      }
    }
    else if(strcmp(argv[i],"-l")==0)
    {
      if(num_layers >= 0 )
      {
        fprintf(stderr,"\nSorry:: You cannot specify number of layer more than once\n\n");
        return 1;
      }
      ++i;
      if( i == argc )
      {
        fprintf(stderr,"\nSorry:: Option -l requires an argument\n\n");
        return 1;
      }

      char *end = NULL;
      num_layers = strtol(argv[i],&end,0);
      if( (end < argv[i] + strlen(argv[i])) || num_layers <= 0 )
      {
        fprintf(stderr,"\nSorry:: Argument to -l must be a postiive integer\n\n");
        return 1;
      }
    }
    else if(rd_filename == NULL)
    {
      struct stat file_stat;
      int rc = stat(argv[i],&file_stat);
      if( rc != 0 )
      {
        fprintf(stderr,"\nSorry:: Cannot read %s (%s)\n\n",argv[i],strerror(errno));
        return 1;
      }
      rd_filename = argv[i];
    }
    else if(j2k_filename == NULL)
    {
      j2k_filename = argv[i];
    }
    else
    {
      fprintf(stderr,"\nUsage argv[0]: [-nl|-vl][-l #] rd_filename [j2k_filename]\n\n");
      return 1;
    }
  }

  if(num_layers < 0) num_layers = 0;
  if(lossless   < 0) lossless   = 1;

  if(rd_filename == NULL)
  {
    fprintf(stderr,"\nUsage argv[0]: [-nl|-vl][-l #] rd_filename [j2k_filename]\n\n");
    return 1;
  }

  if(j2k_filename == NULL)
  {
    const char *ext = strrchr(rd_filename,'.');
    int n = ext-rd_filename;
    j2k_filename = (char *)calloc(n+5, sizeof(char));
    strncpy(j2k_filename, rd_filename, n);
    strcat(j2k_filename,".j2k");
  }

  const char *version = mj2k_opj_version();
  printf("\nMJ2K is wrapping OpenJPEG VERSION: %s\n\n",version);
  printf("   RD file: %s\n", rd_filename);
  printf("  J2K file: %s\n", j2k_filename);
  printf("    Layers: %d\n", num_layers);
  printf("   Quality: %s\n", (lossless ? "Lossless" : "Lossy"));
  printf("\n");

  int fd = open(rd_filename,O_RDWR);
  if( fd < 0 )
  {
    fprintf(stderr,"\nSorry:: Failed to open %s: %s\n\n", rd_filename, strerror(errno));
    return 1;
  }

  uint32_t w;
  if( read(fd, &w, sizeof(w)) != sizeof(w) )
  {
    fprintf(stderr,"\nSorry:: Failed to read width from %s\n\n",argv[1]);
    return 1;
  }

  uint32_t h;
  if( read(fd, &h, sizeof(h)) != sizeof(h) )
  {
    fprintf(stderr,"\nSorry:: Failed to read height from %s\n\n",argv[1]);
    return 1;
  }
  uint32_t n;
  if( read(fd, &n, sizeof(n)) != sizeof(n) )
  {
    fprintf(stderr,"\nSorry:: Failed to read band count from %s\n\n",argv[1]);
    return 1;
  }

  off_t sod = lseek(fd,0,SEEK_CUR);
  off_t eof = lseek(fd,0,SEEK_END);
  lseek(fd,sod,SEEK_SET);
  
  if( eof != sod + w*h*n )
  {
    fprintf(stderr,"\nSorry:: The file size is inconsistent with its header\n    w = %d\n    h = %d\n eof = %lld\n", w, h, eof-sod);
    return 1;
  }

  uint8_t *pixels = (uint8_t *)malloc(w*h*n);
  if(pixels < 0)
  {
    fprintf(stderr,"\nSorry:: Faild to allocate a sufficient buffer for loading the data\n\n");
    return 1;
  }

  off_t to_read = w * h * n;
  uint8_t *pos = pixels;
  while(to_read)
  {
    ssize_t nread = read(fd,pos,to_read);
    if( nread < 0 ) {
      fprintf(stderr,"\nSorry:: Read failed: %s\n\n", strerror(errno));
      return 1;
    }
    to_read -= nread;
    pos += nread;
  }

  mj2k_image_t image;
  image.x0 = 0;
  image.y0 = 0;
  image.x1 = w;
  image.y1 = h;
  image.ncomp = n;
  image.comp  = (mj2k_comp_t *)malloc(n * sizeof(mj2k_comp_t));

  for( int i=0; i<n; ++i )
  {
    image.comp[i].x0 = 0;
    image.comp[i].y0 = 0;
    image.comp[i].w  = w;
    image.comp[i].h  = h;
    image.comp[i].dx = 1;
    image.comp[i].dy = 1;

    image.comp[i].pixels = pixels + i * w * h;
  }

  int rc = mj2k_write_j2k(&image, j2k_filename);

  if( rc != 0 ) {
    printf("\nFailed to create %s\n\n", j2k_filename);
  } else {
    printf("\nDone\n\n");
  }


  return 0;
}
