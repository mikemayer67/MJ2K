#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <openjpeg.h>
#include <assert.h>


void info_handler(const char *msg, void *user_data)
{
  printf("\nOPJ_INFO:: %s\n", msg);
}

void warning_handler(const char *msg, void *user_data)
{
  printf("\nOPJ_WARNING:: %s\n", msg);
}

void error_handler(const char *msg, void *user_data)
{
  printf("\nOPJ_ERROR:: %s\n", msg);
}

/* Data Buffer */

typedef struct
{
  uint8_t *start;
  uint8_t *end;
  uint8_t *pos;
} DataBuffer;

void initialize_buffer(DataBuffer *db, uint8_t *data, size_t length)
{
  db->start = data;
  db->end   = data + length;
  db->pos   = data;
}

OPJ_SIZE_T read_from_buffer(void *p_into, OPJ_SIZE_T p_num_bytes, void *p_buffer)
{
  printf(" - read_from_buffer(%ld)\n",p_num_bytes);
  DataBuffer *db = (DataBuffer*)p_buffer;

  if(db->pos + p_num_bytes > db->end)
  {
    fprintf(stderr," --> Trying to read past end of buffer (%ld of %ld remaining, %ld requested)\n",
        (size_t)(db->end-db->pos), (size_t)(db->end-db->start), p_num_bytes);
    p_num_bytes = (size_t)(db->end-db->pos);
  }

  memcpy(p_into, db->pos, p_num_bytes);
  printf(" - %ld bytes copied\n", p_num_bytes);
  db->pos += p_num_bytes;

  return p_num_bytes;
}

OPJ_BOOL buffer_set_pos(OPJ_OFF_T pos, void *p_buffer)
{
  printf(" - buffer_set_pos(%lld)\n",pos);
  DataBuffer *db = (DataBuffer*)p_buffer;

  if(db->start + pos > db->end)
  {
    fprintf(stderr," --> Trying to position buffer head at byte %lld (buffer size = %ld)\n",
        pos, (size_t)(db->end - db->start));
    return OPJ_FALSE;
  }

  db->pos = db->start + pos;

  return OPJ_TRUE;
}

OPJ_OFF_T buffer_skip_forward(OPJ_OFF_T p_skip, void *p_buffer)
{
  printf(" - buffer_skip_formward(%lld)\n",p_skip);
  DataBuffer *db = (DataBuffer*)p_buffer;

  if(db->pos + p_skip > db->end)
  {
    fprintf(stderr," --> Trying to jump past end of buffer (%ld of %ld remaining, %lld requested)\n",
        (size_t)(db->end-db->pos), (size_t)(db->end-db->start), p_skip);
    p_skip = (size_t)(db->end-db->pos);
  }

  db->pos += p_skip;

  return p_skip;
}


int main(int argc,const char **argv)
{
  if( argc < 2 ) {
    printf("\nSorry:: Missing j2k_file\n\nUsage:: %s j2k_file [rd_file]\n\n", argv[0]);
    exit(1);
  }
  if( argc > 3 ) {
    printf("\nSorry:: Too many arguments\n\nUsage:: %s j2k_file [rd_file]\n\n", argv[0]);
    exit(1);
  }

  const char *version = opj_version();
  printf("version = %s\n",version);

  const char *j2k_file = argv[1];
  char *rd_file = NULL;

  if(argc==3) {
    rd_file = (char *)malloc(1 + strlen(argv[2]));
    strcpy(rd_file, argv[2]);
  } else {
    rd_file = (char *)malloc(5 + strlen(j2k_file));
    strcpy(rd_file, j2k_file);
    strcat(rd_file, ".rd");
  }

  printf("j2k_file: %s\n", j2k_file);
  printf(" rd_file: %s\n", rd_file);

  int src = open(j2k_file,O_RDONLY);
  if(src < 0) {
    printf("\nSorry:: Could not open %s: %s\n\n", rd_file, strerror(errno));
    exit(1);
  }

  off_t eof = lseek(src,0,SEEK_END);
  lseek(src,0,SEEK_SET);

  printf("%s has length: %lld\n",j2k_file,eof);
  uint8_t *j2k_data = (uint8_t*)malloc(eof);

  ssize_t to_read = eof;
  uint8_t *pos = j2k_data;
  while(to_read > 0)
  {
    ssize_t n_read = read(src,pos,to_read);
    if( n_read < 0) {
      printf("Sorry:: Failed to read %s: %s\n\n", rd_file, strerror(errno));
      exit(1);
    }
    to_read -= n_read;
    pos     += n_read;
  }

  opj_dparameters_t dp;
  opj_set_default_decoder_parameters(&dp);

  opj_codec_t *codec = opj_create_decompress(OPJ_CODEC_J2K);
  printf("Codec created: 0x%0x16lx\n", (unsigned long)codec);

  opj_set_info_handler(codec, info_handler, NULL);
  opj_set_warning_handler(codec, warning_handler, NULL);
  opj_set_error_handler(codec, error_handler, NULL);

  OPJ_BOOL rc = opj_setup_decoder(codec, &dp);
  printf("Decoder set up: rc=%s\n", (rc ? "TRUE" : "FALSE" ));

  opj_stream_t *j2k_stream = opj_stream_create(eof,OPJ_TRUE);
  printf("Stream created: 0x%0x16lx\n", (unsigned long)j2k_stream);

  opj_stream_set_read_function(j2k_stream, read_from_buffer);
  opj_stream_set_seek_function(j2k_stream, buffer_set_pos);
  opj_stream_set_skip_function(j2k_stream, buffer_skip_forward);

  DataBuffer data_buffer;
  initialize_buffer(&data_buffer, j2k_data, eof);
  printf("Buffer Created\n");

  opj_stream_set_user_data_length(j2k_stream, eof);
  opj_stream_set_user_data(j2k_stream, (void*)(&data_buffer), NULL);
  printf("Buffer Attached to Stream\n");

  opj_image_t *image = NULL;
  rc = opj_read_header(j2k_stream, codec, &image);
  printf("Header read from stream: rc=%s\n", (rc?"TRUE":"FALSE"));

  rc = opj_decode(codec, j2k_stream, image);
  printf("Image decoded from stream: rc=%s\n", (rc?"TRUE":"FALSE"));

  int ncomp = image->numcomps;
  int x0 = image->x0;
  int y0 = image->y0;
  int x1 = image->x1;
  int y1 = image->y1;
  printf("Bounding box: (%d,%d), (%d,%d)\n",x0,y0,x1,y1);
  printf("Number of components: %d\n",ncomp);

  int nrow = y1-y0;
  int ncol = x1-x0;

  int dst = open(rd_file,O_RDWR|O_CREAT,0644);
  if(dst < 0)
  {
    printf("Sorry:: Failed to create %s: %s\n", rd_file, strerror(errno));
    exit(1);
  }
  write(dst,&nrow,2);
  write(dst,&ncol,2);
  write(dst,&ncomp,2);

  for(int i=0; i<ncomp; ++i)
  {
    opj_image_comp_t *comp = &(image->comps[i]);
    int cx0  = comp->x0;
    int cy0  = comp->y0;
    int cdx  = comp->dx;
    int cdy  = comp->dy;
    int bpp  = comp->prec;
    int sgnd = comp->sgnd;

    assert(nrow == comp->h);
    assert(ncol == comp->w);

    write(dst,comp->data,nrow*ncol*sizeof(uint32_t));

    printf("component %d: %dx%d+%d+%d (%dx%d) [%s %d bits]\n",i,ncol,nrow,cx0,cy0,cdx,cdy,
        (sgnd?"signed":"unsigned"), bpp);
  }

  close(dst);
  close(src);

  opj_image_destroy(image);
  opj_stream_destroy(j2k_stream);
  opj_destroy_codec(codec);

  free(j2k_data);
  free(rd_file);

  return 0;
}
