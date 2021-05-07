#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "mamj2k.h"

typedef enum {LOSSLESS, LOSSY, NUM_DWT } dwt_t;
typedef enum {COMPRESSION, PSNR, NUM_QUAL } qual_t;

typedef struct
{
  dwt_t  dwt;
  int    num_res;
  qual_t qual_type;
  int    num_layers;
  float  layer_qual[100];
  char  *infile;
  char  *outfile;
} options_t;


typedef const char *string_t;
typedef string_t   *string_ptr;

const char *gProc = NULL;

void error(const char *err)
{
  fprintf(stderr,"\nSorry:: %s\n\n", err);
  exit(1);
}

void usage(const char *err)
{
  if(strlen(err)>0) printf("\n%s\n",err);

  printf("\nUsage:: %s [-nl|-vl] [(-comp|-psnr) layer_values] [-r num_res] rd_file [j2k_file]\n\n",
      (gProc ? gProc : "write_j2k" ));
  printf("   -nl = lossless compression\n");
  printf("   -vl = lossy compression\n");
  printf("\n");
  printf("   -comp [value [value [...]]] = layer compression ratio, decreasing values, 1 = lossless\n");
  printf("   -psnr [value [value [...]]] = layer PSNR values, increasing values, 0 = lossless\n");
  printf("\n");
  printf("   -r num_res = Number of resolutions\n\n");

  exit( strlen(err)>0 ? 1 : 0 );
}


int parse_dwt_arg(options_t *opts, string_ptr cur_arg, string_ptr end_arg)
{
  dwt_t dwt = NUM_DWT;
  if      (strcmp(*cur_arg,"-nl")==0) { dwt = LOSSLESS; }
  else if (strcmp(*cur_arg,"-vl")==0) { dwt = LOSSY;    }
  else                                { return 0; }

  switch(opts->dwt)
  {
    case NUM_DWT:
      opts->dwt = dwt;
      break;
    case LOSSY:
      switch(dwt) {
        case LOSSLESS: error("Cannot set both -nl and -vl"); break;
        case LOSSY:    error("Cannot set -vl twice");        break;
        default: break;
      }
      break;
    case LOSSLESS:
      switch(dwt) {
        case LOSSY:    error("Cannot set both -nl and -vl"); break;
        case LOSSLESS: error("Cannot set -nl twice");        break;
        default: break;
      }
      break;
  }
  return 1;
}

int parse_qual_arg(options_t *opts, string_ptr cur_arg, string_ptr end_arg)
{
  qual_t qual = NUM_QUAL;
  if      (strcmp(*cur_arg,"-comp")==0) { qual = COMPRESSION; }
  else if (strcmp(*cur_arg,"-psnr")==0) { qual = PSNR; }
  else                                { return 0; }

  switch(opts->qual_type)
  {
    case NUM_QUAL:
      opts->qual_type = qual;
      break;
    case PSNR:
      switch(qual) {
        case COMPRESSION: error("Cannot set both -comp and -psnr"); break;
        case PSNR:        error("Cannot set -psnr twice");          break;
        default: break;
      }
      break;
    case COMPRESSION:
      switch(qual) {
        case PSNR:        error("Cannot set both -comp and -psnr"); break;
        case COMPRESSION: error("Cannot set -comp twice");          break;
        default: break;
      }
      break;
  }

  opts->num_layers = 0;
  for( ++cur_arg; cur_arg<end_arg; ++cur_arg)
  {
    float value;
    if( sscanf(*cur_arg,"%f",&value) != 1 ) break;

    if( opts->num_layers > 99 )
    {
      switch(qual)
      {
        case COMPRESSION: error("Cannot specify more than 100 compression values"); break;
        case PSNR:        error("Cannot specify more than 100 PSNR values");        break;
        default: break;
      }
    }

    opts->layer_qual[opts->num_layers] = value;
    opts->num_layers += 1;
  }

  if( opts->num_layers < 1 )
  {
    switch(qual)
    {
      case COMPRESSION: error("Must specify at least one compression value"); break;
      case PSNR:        error("Must specify at least one PSNR value");        break;
      default: break;
    }
  }

  return 1 + opts->num_layers;
}

int parse_res_arg(options_t *opts, string_ptr cur_arg, string_ptr end_arg)
{
  if( strcmp(*cur_arg,"-r") != 0 ) { return 0; }

  if(opts->num_res >= 0 ) error("You cannot specify number of resolutions more than once");

  cur_arg += 1;
  if( cur_arg >= end_arg ) error("Option -r requries an argument");

  if( sscanf(*cur_arg,"%d",&(opts->num_res)) != 1 ) error("Argument to -r must be a postive integer");
  if( opts->num_res < 1 ) error("Argument to -r must be a postive integer");

  return 2;
}


void parse_args(options_t *opts, int argc, const char **argv)
{
  opts->dwt        = NUM_DWT;
  opts->qual_type  = NUM_QUAL;
  opts->num_res    = -1;
  opts->num_layers = -1;

  opts->infile  = NULL;
  opts->outfile = NULL;

  string_ptr cur_arg = argv;
  string_ptr end_arg = argv + argc;

  gProc = *cur_arg++;

  while(cur_arg < end_arg)
  {
    int narg;

    if( strcmp(*cur_arg,"-h")==0 ) { usage(""); }

    else if ((narg = parse_dwt_arg ( opts, cur_arg, end_arg))) { cur_arg += narg; }
    else if ((narg = parse_qual_arg( opts, cur_arg, end_arg))) { cur_arg += narg; }
    else if ((narg = parse_res_arg ( opts, cur_arg, end_arg))) { cur_arg += narg; }

    else if(opts->infile == NULL)
    {
      opts->infile = (char *)malloc(1 + strlen(*cur_arg));
      strcpy(opts->infile,*cur_arg);
      cur_arg += 1;
    }
    else if(opts->outfile == NULL)
    {
      opts->outfile = (char *)malloc(1 + strlen(*cur_arg));
      strcpy(opts->outfile,*cur_arg);
      cur_arg += 1;
    }
    else
    {
      const char *prefix = "Unknown argument: ";
      char *err = (char *)malloc(strlen(prefix) + strlen(*cur_arg) + 1);
      strcpy(err,prefix);
      strcat(err,*cur_arg);
      error(err);
    }
  }

  if(opts->dwt == NUM_DWT)  { opts->dwt = LOSSLESS; }
  if(opts->num_layers < 0 ) { opts->num_layers = 1; }
  if(opts->num_res    < 0 ) { opts->num_res    = 6; }

  if(opts->infile == NULL) usage("Missing rd_infile");

  struct stat file_stat;
  int rc = stat(opts->infile,&file_stat);
  if( rc != 0 )
  {
    fprintf(stderr,"\nSorry:: Cannot read %s (%s)\n\n",opts->infile,strerror(errno));
    exit(1);
  }

  if(opts->outfile == NULL)
  {
    const char *ext = strrchr(opts->infile,'.');
    int n = ext-opts->infile;
    opts->outfile = (char *)malloc(n+5);
    strncpy(opts->outfile, opts->infile, n);
    strcat(opts->outfile,".j2k");
  }

  printf("Command Line Options:\n");
  printf("           DWT: %s\n", opts->dwt == LOSSLESS ? "Lossless" : "Lossy");
  printf("   Resolutions: %d\n", opts->num_res);
  printf("        Layers: %d\n", opts->num_layers);
  if(opts->num_layers) {
    printf("     Qual Type: %s\n", opts->qual_type == COMPRESSION ? "Compression" : "PSNR");
    const char *delim = "";
    printf("   Qual Values: [");
    for(int i=0; i<opts->num_layers; ++i) {
      printf("%s%f", delim, opts->layer_qual[i]);
      delim = ", ";
    }
    printf("]\n");
  }
  printf("      Infile: %s\n", opts->infile);
  printf("     Outfile: %s\n", opts->outfile);
}




int main(int argc,const char **argv)
{
  options_t opts;
  parse_args(&opts, argc, argv);

  const char *version = mj2k_opj_version();
  printf("\nMJ2K is wrapping OpenJPEG VERSION: %s\n\n",version);

  int fd = open(opts.infile,O_RDWR);
  if( fd < 0 )
  {
    fprintf(stderr,"\nSorry:: Failed to open %s: %s\n\n", opts.infile, strerror(errno));
    return 1;
  }

  uint32_t w;
  if( read(fd, &w, sizeof(w)) != sizeof(w) )
  {
    fprintf(stderr,"\nSorry:: Failed to read width from %s\n\n",opts.infile);
    return 1;
  }

  uint32_t h;
  if( read(fd, &h, sizeof(h)) != sizeof(h) )
  {
    fprintf(stderr,"\nSorry:: Failed to read height from %s\n\n",opts.infile);
    return 1;
  }
  uint32_t n;
  if( read(fd, &n, sizeof(n)) != sizeof(n) )
  {
    fprintf(stderr,"\nSorry:: Failed to read band count from %s\n\n",opts.infile);
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

  close(fd);

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

  mj2k_cparam_t cparam;
  cparam.irreversible    = (opts.dwt == LOSSY ? 1 : 0);
  cparam.numresolution   = opts.num_res;
  cparam.tcp_numlayers   = opts.num_layers;
  cparam.layer_qual_type = (opts.qual_type == COMPRESSION ? 1 : 0);
  memset(cparam.layer_qual_values, 0, sizeof(cparam.layer_qual_values));
  for(int i=0; i<opts.num_layers; ++i) cparam.layer_qual_values[i] = opts.layer_qual[i];

  int rc = mj2k_write_j2k(&image, &cparam, opts.outfile);

  if( rc != 0 ) {
    printf("\nFailed to create %s\n\n", opts.outfile);
  } else {
    printf("\nDone\n\n");
  }


  return 0;
}
