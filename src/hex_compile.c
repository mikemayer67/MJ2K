#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_hex(char c)
{
  if( c >= '0' && c <= '9' ) { return 1; }
  if( c >= 'a' && c <= 'f' ) { return 1; }
  if( c >= 'A' && c <= 'F' ) { return 1; }
  return 0;
}

int hex_value(char c)
{
  if( c >= '0' && c <= '9' ) { return (c - '0'); }
  if( c >= 'a' && c <= 'f' ) { return 10 + (c-'a'); }
  if( c >= 'A' && c <= 'F' ) { return 10 + (c-'A'); }
  fprintf(stderr,"\nERROR:: Invalid hex digit (%c)\n\n",c);
  exit(1);
}

int main(int argc, const char **argv)
{
  // parse inputs

  if( argc < 2 || argc > 3 ) {
    fprintf(stderr, "\nUsage:: %s input_file [output_file]\n\n", argv[0]);
    exit(1);
  }

  const char *src_file = argv[1];
  const char *dst_file = ( argc==3 ? argv[2] : NULL );

  // open the source (hex_dump) file for reading

  int src = open(src_file, O_RDONLY);
  if( src < 0 ) {
    fprintf(stderr, "\nSorry:: Cannot open %s: %s\n\n", src_file, strerror(errno));
    exit(1);
  }

  // verify that we're not overwriting a file with output

  if(dst_file != NULL)
  {
    if( access(dst_file, F_OK) == 0 ) {
      fprintf(stderr, "\nSorry:: Will not overwrite existing %s\n\n", dst_file);
      exit(1);
    }
  }

  // read source data into memory

  off_t src_length = lseek(src,0,SEEK_END);
  lseek(src,0,SEEK_SET);

  char *src_data = (char *)malloc(src_length);
  if(src_data == NULL) {
    fprintf(stderr, "\nSorry:: Cannot load all of %s into memory\n\n", dst_file);
    exit(1);
  }

  char   *pos    = src_data;
  char   *end    = pos + src_length;
  size_t to_read = src_length;
  while(to_read) {
    ssize_t nread = read(src, pos, to_read);
    if( nread < 0 ) { 
      fprintf(stderr, "\nSorry:: Failed to read from %s\n\n", src_file);
      exit(1);
    }
    to_read -= nread;
    pos     += nread;
  }

  // validate the source data (valid hex_dump)
 
  size_t dst_length = 0;
  
  uint32_t line_no = 0;

  pos = src_data;
  while(pos < end)
  {
    line_no += 1;

    // skip address

    int isblank = 1;
    for(; pos<end && *pos==' '    ; ++pos);             // skip whitespace
    for(; pos<end && is_hex(*pos) ; ++pos) isblank = 0; // skip address
    for(; pos<end && *pos==' '    ; ++pos);             // skip whitespace

    if( pos == end || *pos == '\n' ) { // end of data or end of line
      if ( !isblank ) { // has address, but not data
        fprintf(stderr,"\nSorry:: invalid data on line %ld\n\n", line_no);
        exit(1);
      }
      else // blank line
      {
        if(pos == end) break;    // end of data
        else           continue; // go to next line
      }
    }

    // get colon seperating address from data

    if( *pos != ':') {
      fprintf(stderr,"\nSorry invalid address on line %ld\n\n", line_no);
      exit(1);
    }
    ++pos;

    // read data

    isblank = 1;
    
    while( pos<end )
    {
      for(; pos<end && *pos==' '    ; ++pos);             // skip whitespace

      if(*pos == ':') { // end of data
        if(isblank) {
          fprintf(stderr,"\nSorry:: missing data on line %ld\n\n", line_no);
          exit(1);
        }
        while(pos<end && *pos !='\n') ++pos;  // skip to end of line or end of data
        if(pos == end) break;                 // end of data
        else           continue;              // go to next line
      }

      for(int i=0; i<2; ++i)
      {
        if(!is_hex(*pos)) {
          fprintf(stderr,"\nSorry:: invalid hex digit (%c) on line %ld\n\n", *pos, line_no);
          exit(1);
        }
        ++pos;
      }

      if(!is_hex(*pos)) {
        fprintf(stderr,"\nSorry:: invalid hex digit (%c) on line %ld\n\n", *pos, line_no);
        exit(1);
      }
      ++pos;
      ++dst_length;

      if(is_hex(*pos)) {
        fprintf(stderr,"\nSorry:: invalid (3 digit) hex data on line %ld\n\n", line_no);
        exit(1);
      }


    }
    `
  }

  




  // prepare the output file (which may be stdout)

  FILE *dst = stdout;
  if(dst_file != NULL)
  {
    dst = fopen(dst_file,"w");
    if( dst == NULL ) {
      fprintf(stderr, "\nSorry:: Cannot create %s: %s\n\n", dst_file, strerror(errno));
      exit(1);
    }
  }

