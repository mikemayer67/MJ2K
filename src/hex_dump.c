#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, const char **argv)
{
  if( argc < 2 || argc > 3 ) {
    fprintf(stderr, "\nUsage:: %s input_file [output_file]\n\n", argv[0]);
    exit(1);
  }

  const char *src_file = argv[1];
  const char *dst_file = ( argc==3 ? argv[2] : NULL );

  int src = open(src_file, O_RDONLY);
  if( src < 0 ) {
    fprintf(stderr, "\nSorry:: Cannot open %s: %s\n\n", src_file, strerror(errno));
    exit(1);
  }

  FILE *dst = stdout;
  if(dst_file != NULL)
  {
    if( access(dst_file, F_OK) == 0 ) {
      fprintf(stderr, "\nSorry:: Will not overwrite existing %s\n\n", dst_file);
      exit(1);
    }
    dst = fopen(dst_file,"w");
    if( dst == NULL ) {
      fprintf(stderr, "\nSorry:: Cannot create %s: %s\n\n", dst_file, strerror(errno));
      exit(1);
    }
  }

  off_t bytes_remaining = lseek(src,0,SEEK_END);
  lseek(src,0,SEEK_SET);

  off_t pos = 0;
  uint8_t bytes[32];

  while(bytes_remaining > 0)
  {
    int nread = read(src, bytes, 32);
    if( nread < 0) {
      fprintf(stderr, "\nSorry:: Error reading from %s at offset %lld: %s\n\n", src_file, pos, strerror(errno));
      exit(1);
    }

    fprintf(dst,"%08x:", (uint32_t)pos);
    for(int i=0; i<32; ++i) {
      if( i%8 == 0 ) { fprintf(dst, "  ");              }
      if( i<nread )  { fprintf(dst, " %02x", bytes[i]); }
      else           { fprintf(dst, "   ");             }
    }
    fprintf(dst,"  :  ");
    for(int i=0; i<nread; ++i) {
      fprintf(dst, "%c", bytes[i] < 0x20 ? '.' : bytes[i]);
    }
    fprintf(dst,"\n");

    bytes_remaining -= nread;
    pos += 32;
  }
}
