#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc,char **argv)
{
  if( argc != 2 ) 
  {
    fprintf(stderr,"\nUsage:: %s rd_file\n\n", argv[0]);
    exit(1);
  }

  int src = open(argv[1],O_RDONLY);
  if( src < 0 )
  {
    fprintf(stderr,"\nSorry:: Failed to open %s  (%s)\n\n", argv[1], strerror(errno));
    exit(1);
  }

  uint32_t w;
  if( read(src, &w, sizeof(w)) != sizeof(w) )
  {
    fprintf(stderr,"\nSorry:: Failed to read width from %s\n\n",argv[1]);
    exit(1);
  }

  uint32_t h;
  if( read(src, &h, sizeof(h)) != sizeof(h) )
  {
    fprintf(stderr,"\nSorry:: Failed to read height from %s\n\n",argv[1]);
    exit(1);
  }
  uint32_t n;
  if( read(src, &n, sizeof(n)) != sizeof(n) )
  {
    fprintf(stderr,"\nSorry:: Failed to read band count from %s\n\n",argv[1]);
    exit(1);
  }

  uint32_t bytes = w * h;
  uint8_t *buffer = (uint8_t *)malloc(bytes);

  char *dot = strrchr(argv[1],'.');
  int root_length = dot - argv[1];
  char *root = (char *)malloc(1 + root_length);
  strncpy(root,argv[1],root_length);
  root[root_length] = '\0';

  for(int i=0; i<n; ++i)
  {
    char *filename = (char *)malloc(7 + root_length);

    if(n==1) sprintf(filename,"%s.csv",root);
    else     sprintf(filename,"%s_%d.csv", root, i+1);

    FILE *dst = fopen(filename,"w");
    if( dst == NULL )
    {
      fprintf(stderr,"\nSorry:: Failed to create %s  (%s)\n\n", filename, strerror(errno));
      exit(1);
    }

    uint32_t toread = bytes;
    uint8_t *pos = buffer;
    while( toread > 0 )
    {
      uint32_t nread = read(src,pos,toread);
      if( nread < 0 )
      {
        fprintf(stderr,"Sorry:: Error reading %s  (%s)\n\n", argv[1], strerror(errno));
        exit(1);
      }
      toread -= nread;
      pos    += nread;
    }

    uint8_t *pixel = buffer;
    
    const char *delim = "";
    for(int r=0; r<h; ++r) {
      for(int c=0; c<w; ++c) {
        fprintf(dst,"%s%d", delim, *(pixel++));
        delim = ", ";
      }
      delim = "\n";
    }
    fprintf(dst,"\n");
    fclose(dst);
  }

  close(src);

  printf("%d %d %d\n",w,h,n);
}
