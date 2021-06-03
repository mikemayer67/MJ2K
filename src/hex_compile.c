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
  else
  {
    dst_file = "stdout";
  }

  // validate the source data (valid hex_dump)
 
  int has_error = 0;
  
  uint32_t line_no = 0;

  pos = src_data;
  char *eof = pos + src_length;  // end of file
  char *eol = pos;               // end of line
  char *eod = NULL;              // end of data
  char *eox = NULL;              // end of hex string
  for(; pos<eof && !has_error; pos = eol+1)
  {
    line_no += 1;

    // find end of line
    for(eol=pos; eol<eof && *eol!='\n'; ++eol);
    
    // find start/end of data
    for(;pos<eol && *pos != ':'; ++pos);
    if(pos == eol)
    {
      eod = eol;
    }
    else
    {
      pos += 1; // move one past the colon
      for(eod=pos; eod<eol && *eod != ':'; ++eod);
    }

    // parse data
    while(pos < eod)
    {
      for(; pos<eod && *pos==' '; ++pos); // skip over white space
      if(pos == eod) break;
      for(eox=pos+1; eox<eod && *eox!=' '; ++eox); // find next white space
      if(eox != pos + 2) {
        has_error=1;
        fprintf(stderr,"\nSorry:: All hex values must be 2 digits long (line %u)\n\n", line_no);
        break;
      } 
      if( ! (is_hex(*pos) && is_hex(*(pos+1)) ) ) {
        has_error=1;
        fprintf(stderr,"\nSorry:: '%c%c' is not a valid hex value (line %u)\n\n", *pos, *(pos+1), line_no);
        break;
      }

      uint8_t byte = 16*hex_value(*pos) + hex_value(*(pos+1));
      if( fwrite( &byte, 1, 1, dst ) != 1 ) {
        has_error=1;
        fprintf(stderr,"\nSorry:: Failed to write data to %s: %s\n\n", dst_file, strerror(errno));
        break;
      }

      pos = eox;
    }
  }

  // if has error, remove the delete the output file

  if(has_error && dst_file != NULL)
  {
    fclose(dst);
    unlink(dst_file);
  }

  return 0;
}

  





