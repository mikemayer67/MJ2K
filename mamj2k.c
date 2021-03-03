#include "mamj2k.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

uint32_t mamj2k_cp_reduce = 0;
uint32_t mamj2k_cp_layer = 0;

/* Data Buffer */

typedef struct
{
  mj2k_byte_t *start;
  mj2k_byte_t *end;
  mj2k_byte_t *pos;
} DataBuffer;

void initialize_buffer(DataBuffer *db, mj2k_bytes_t data, size_t length)
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

void mj2k_init_comp(mj2k_comp_t *m_comp, opj_image_comp_t *o_comp )
{
  m_comp->x0 = o_comp->x0;
  m_comp->y0 = o_comp->y0;
  m_comp->w  = o_comp->w;
  m_comp->h  = o_comp->h;
  m_comp->dx = o_comp->dx;
  m_comp->dy = o_comp->dy;

  int bits = o_comp->prec;

  int32_t dv = ( o_comp->sgnd ? (1 << (bits-1)) : 0 );
  int     bs =  bits - 8;
 
  int nrows = o_comp->h;
  int ncols = o_comp->w;

  m_comp->pixels = (mj2k_pixel_t *)malloc(nrows*ncols * sizeof(mj2k_pixel_t));

  int32_t      *op = o_comp->data;
  mj2k_pixel_t *mp = m_comp->pixels;

  for(int i=0; i<nrows*ncols; ++i, ++op, ++mp)
  {
    uint32_t v = *op + dv;
    if(bits > 8) { v >>= bits-8; }
    if(bits < 8) { v <<= 8-bits; }
    *mp = v;
  }
}

void mj2k_init_image(mj2k_image_t *m_im, opj_image_t *o_im )
{
  m_im->x0 = o_im->x0;
  m_im->y0 = o_im->y0;
  m_im->x1 = o_im->x1;
  m_im->y1 = o_im->y1;

  int ncomp = o_im->numcomps;

  m_im->ncomp = ncomp;
  m_im->comp  = (mj2k_comp_t *)calloc(ncomp,sizeof(mj2k_comp_t)); 

  mj2k_comp_t      *m_comp = m_im->comp;
  opj_image_comp_t *o_comp = o_im->comps;
  for(int i=0; i<ncomp; ++i, ++m_comp, ++o_comp)
  {
    mj2k_init_comp( m_comp, o_comp );
  }
}


/* MJ2K Internal Functions */

void show_decoder_parameters(opj_dparameters_t *dp)
{
  printf("    cp_reduce = %u\n", dp->cp_reduce);
  printf("     cp_layer = %u\n", dp->cp_layer);
  printf("        flags = 0x%x\n", dp->flags);
  printf("        DA_x0 = %u\n", dp->DA_x0);
  printf("        DA_y0 = %u\n", dp->DA_y0);
  printf("        DA_x1 = %u\n", dp->DA_x1);
  printf("        DA_y1 = %u\n", dp->DA_y1);
  printf("      verbose = %s\n", (dp->m_verbose ? "TRUE" : "FALSE"));
  printf("   tile_index = %u\n", dp->tile_index);
  printf("   nb_tile_.. = %u\n", dp->nb_tile_to_decode);
}

void show_image_header(opj_image_t *image)
{
  printf("        x0 = %u\n", image->x0);
  printf("        y0 = %u\n", image->y0);
  printf("        x1 = %u\n", image->x1);
  printf("        y1 = %u\n", image->y1);
  printf("  numcomps = %u\n", image->numcomps);
  switch(image->color_space) {
    case OPJ_CLRSPC_UNKNOWN:     printf("  colspace = unknown\n");     break;
    case OPJ_CLRSPC_UNSPECIFIED: printf("  colspace = unspecified\n"); break;
    case OPJ_CLRSPC_SRGB:        printf("  colspace = sRGB\n");        break;
    case OPJ_CLRSPC_GRAY:        printf("  colspace = grayscale\n");   break;
    case OPJ_CLRSPC_SYCC:        printf("  colspace = YUV.\n");        break;
    case OPJ_CLRSPC_EYCC:        printf("  colspace = e-YCC\n");       break;
    case OPJ_CLRSPC_CMYK:        printf("  colspace = CMYK.\n");       break;
  }

  int ncomp = image->numcomps;

  opj_image_comp_t *comps = (opj_image_comp_t *)image->comps;
  for(int i=0; i<ncomp; ++i)
  {
    printf("  ----COMP %d----\n", i+1);
    printf("        dx = %u\n", comps[i].dx);
    printf("        dy = %u\n", comps[i].dy);
    printf("         w = %u\n", comps[i].w);
    printf("         h = %u\n", comps[i].h);
    printf("        x0 = %u\n", comps[i].x0);
    printf("        y0 = %u\n", comps[i].y0);
    printf("      prec = %u\n", comps[i].prec);
    printf("       bpp = %u\n", comps[i].bpp);
    printf("      sgnd = %u\n", comps[i].sgnd);
    printf("  resno_de = %u\n", comps[i].resno_decoded);
    printf("    factor = %u\n", comps[i].factor);
    printf("     alpha = %u\n", comps[i].alpha);
    printf("      data = 0x%lx\n", (long)comps[i].data);
  }
  printf("  --------------\n");

  printf("  icc_prof_len = %u\n", image->icc_profile_len);
  printf("  icc_prof_buf = 0x%lx\n", (long)image->icc_profile_buf);
}

/* MJ2K Wrapper API Functions */

const char *mj2k_opj_version(void)
{
  return opj_version();
}


mj2k_image_t *mj2k_read_j2k(const char *filename)
{
  int fp = open(filename,0);
  if( fp < 0 )
  {
    fprintf(stderr,"\nError:: Failed to open %s (%s)\n\n", filename, strerror(errno));
    exit(1);
  }

  off_t eof = lseek(fp,0,SEEK_END);
  lseek(fp,0,SEEK_SET);

  printf("%s has length: %lld\n",filename,eof);

  mj2k_bytes_t data = (mj2k_bytes_t)malloc(eof);

  ssize_t to_read = eof;
  mj2k_byte_t *pos = data;
  while(to_read > 0)
  {
    ssize_t n_read = read(fp, pos, to_read);
    if( n_read < 0 ) {
      fprintf(stderr,"\nError:: Failed to read from %s (%s)\n\n", filename, strerror(errno));
      exit(1);
    }
    printf("   read %ld bytes / %ld to go\n", n_read,to_read);
    to_read -= n_read;
    pos     += n_read;
  }

  mj2k_image_t *rval = mj2k_parse_j2k(data, eof);

  free(data);

  return rval;
}


mj2k_image_t *mj2k_parse_j2k(const mj2k_bytes_t j2k_data, off_t length)
{
  printf("\n DECODE\n");
  opj_dparameters_t parameters;
  opj_set_default_decoder_parameters(&parameters);

  parameters.cp_reduce = mamj2k_cp_reduce;
  parameters.cp_layer  = mamj2k_cp_layer;

  show_decoder_parameters(&parameters);

  opj_codec_t *codec = opj_create_decompress(OPJ_CODEC_J2K);
  mj2k_image_t *rval = NULL;

  if (opj_setup_decoder(codec, &parameters))
  {
    printf(" CODEC CREATED\n");
    opj_stream_t *s = opj_stream_create(length, OPJ_TRUE);

    if(s != NULL)
    {
      printf(" STREAM CREATED\n");

      opj_stream_set_read_function(s, read_from_buffer);
      opj_stream_set_seek_function(s, buffer_set_pos);
      opj_stream_set_skip_function(s, buffer_skip_forward);

      printf(" STREAM FUNCTIONS SET\n");

      DataBuffer data_buffer;
      initialize_buffer(&data_buffer, j2k_data, length);

      printf(" BUFFER CREATED\n");

      opj_stream_set_user_data_length(s, length);
      opj_stream_set_user_data(s, (void*)(&data_buffer), NULL);

      printf(" BUFFER ATTACHED TO STREAM\n");

      opj_image_t *image = NULL;
      OPJ_BOOL ok = opj_read_header(s, codec, &image);

      printf(" HEADER READ: %s\n", (ok ? "OK" : "FAILED"));

      if(ok)
      {
        ok = opj_decode(codec, s, image);

        printf("\n IMAGE DECODED: %s\n", (ok ? "OK" : "FAILED"));

        if(ok && image != NULL)
        {
          show_image_header(image);
          rval = (mj2k_image_t *)calloc(1,sizeof(mj2k_image_t));
          mj2k_init_image(rval,image);
        }
      }

      if(image != NULL) { opj_image_destroy(image); }

      opj_stream_destroy(s);
      opj_destroy_codec(codec);
    }
  }
  printf("\n DONE\n");

  return rval;
}


void mj2k_free_image(mj2k_image_t *image)
{
  if(image->comp != NULL)
  {
    for(int i=0; i<image->ncomp; ++i) { free(image->comp[i].pixels); }
    free(image->comp);
  }
}



