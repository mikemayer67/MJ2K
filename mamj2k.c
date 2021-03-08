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

void show_encoder_parameters(opj_cparameters_t *cp)
{
  const char *delim = "";

  printf("     tile_size_on = %s\n", (cp->tile_size_on ? "YES" : "NO"));
  printf("           cp_tx0 = %d\n", cp->cp_tx0);
  printf("           cp_ty0 = %d\n", cp->cp_ty0);
  printf("           cp_tdx = %d\n", cp->cp_tdx);
  printf("           cp_tdy = %d\n", cp->cp_tdy);
  printf("       cp_comment = '%s'\n", cp->cp_comment);
  printf("             csty = %d\n", cp->csty);
  switch(cp->prog_order)
  {
    case OPJ_PROG_UNKNOWN: printf("       prog_order = PJ_PROG_UNKNOWN\n"); break;
    case OPJ_LRCP:         printf("       prog_order = PJ_LRCP\n"); break;
    case OPJ_RLCP:         printf("       prog_order = PJ_RLCP\n"); break;
    case OPJ_RPCL:         printf("       prog_order = PJ_RPCL\n"); break;
    case OPJ_PCRL:         printf("       prog_order = PJ_PCRL\n"); break;
    case OPJ_CPRL:         printf("       prog_order = PJ_CPRL\n"); break;
  }
  printf("          numpocs = %d\n", cp->numpocs); 
  const char *gap = "                      ";
  const char *prog_order[] = {"OPJ_PROG_UNKNOWN","OPJ_LRCP","OPJ_RLcP","OPJ_RPCL","OPJ_PCRL","OPJ_cPRL"};
  for(int i=0; i<cp->numpocs; ++i) {
    opj_poc_t *poc = &(cp->POC[i]);
    printf("          POC[%2d] = {", i+1);
    printf("%sresno0=%d, compno0=%d\n",gap,poc->resno0,poc->compno0);
    printf("%slayno1=%d, resno1=%d, compon01=%d\n",gap,poc->layno1,poc->resno1,poc->compno1);
    printf("%slayno0=%d, precno0=%d, precno1=%d\n",gap,poc->layno0,poc->precno0,poc->precno1);
    printf("%sprg1=%s, prg=%s\n",gap,prog_order[poc->prg1+1],prog_order[poc->prg+1]);
    printf("%sprogorder=%s\n",gap,poc->progorder);
    printf("%stile=%d\n",gap,poc->tile);
    printf("%stx0=%d, tx1=%d, ty0=%d, ty1=%d\n",gap,poc->tx0,poc->tx1,poc->ty0,poc->ty1);
    printf("%slayS=%d, resS=%d, compS=%d, prcS=%d\n",gap,poc->layS,poc->resS,poc->compS,poc->prcS);
    printf("%slayE=%d, resE=%d, compE=%d, prcE=%d\n",gap,poc->layE,poc->resE,poc->compE,poc->prcE);
    printf("%stxS=%d, txE=%d, tyS=%d, tyE=%d, dx=%d, dy=%d\n",gap,
        poc->txS,poc->txE,poc->tyS,poc->tyE,poc->dx, poc->dy);
    printf("%slay_t=%d, res_t=%d, comp_t=%d, prc_t=%d, tx0_t=%d, ty0_t=%d\n",gap,
        poc->lay_t, poc->res_t, poc->comp_t, poc->prc_t, poc->tx0_t, poc->ty0_t);
  }

  printf("        numlayers = %d\n", cp->tcp_numlayers);
  printf("   cp_disto_alloc = %d\n", cp->cp_disto_alloc);
  printf("        tcp_rates = [");
  delim = "";
  for(int i=0; i<cp->tcp_numlayers && cp->cp_fixed_quality; ++i)
  {
    printf("%s%f", delim,cp->tcp_rates[i]);
    delim = ", ";
  }
  printf("]\n");
  printf(" cp_fixed_quality = %d\n", cp->cp_fixed_quality);
  printf("   tcp_distoratio = [");
  delim = "";
  for(int i=0; i<cp->tcp_numlayers && cp->cp_fixed_quality; ++i)
  {
    printf("%s%f", delim,cp->tcp_distoratio[i]);
    delim = ", ";
  }
  printf("]\n");
  printf("   cp_fixed_alloc = %d\n", cp->cp_fixed_alloc);
  printf("       cp_matrice = 0x%x\n", (unsigned int)cp->cp_matrice);

  printf("    numresolution = %d\n", cp->numresolution);
  printf("     cblockw_init = %d\n", cp->cblockw_init);
  printf("     cblockh_init = %d\n", cp->cblockh_init);
  printf("             mode = %d\n", cp->mode);
  printf("     irreversible = %d\n", cp->irreversible);
  printf("       roi_compno = %d\n", cp->roi_compno);
  printf("        roi_shift = %d\n", cp->roi_shift);

  printf("         res_spec = %d\n", cp->res_spec);
  printf("        prcw_init = [");
  delim = "";
  for(int i=0; i<cp->res_spec; ++i)
  {
    printf("%s%d",delim,cp->prcw_init[i]);
    delim = ", ";
  }
  printf("]\n");
  printf("        prch_init = [");
  delim = "";
  for(int i=0; i<cp->res_spec; ++i)
  {
    printf("%s%d",delim,cp->prch_init[i]);
    delim = ", ";
  }
  printf("]\n");

  printf("           infile = %s\n", cp->infile);
  printf("          outfile = %s\n", cp->outfile);
  printf("         index_on = %d\n", cp->index_on);
  printf("            index = %s\n" ,cp->index);
  printf("  image_offset_x0 = %d\n", cp->image_offset_x0);
  printf("  image_offset_y0 = %d\n", cp->image_offset_y0);
  printf("   subsampling_dx = %d\n", cp->subsampling_dx);
  printf("   subsampling_dy = %d\n", cp->subsampling_dy);
  printf("     decod_format = %d\n", cp->decod_format);
  printf("       cod_format = %d\n", cp->cod_format);

  switch(cp->cp_cinema)
  {
    case OPJ_OFF:         printf("        cp_cinema = OPJ_OFF\n"); break;
    case OPJ_CINEMA2K_24: printf("        cp_cinema = OPJ_CINEMA2K_24\n"); break;
    case OPJ_CINEMA2K_48: printf("        cp_cinema = OPJ_CINEMA2K_48\n"); break;
    case OPJ_CINEMA4K_24: printf("        cp_cinema = OPJ_CINEMA4K_24\n"); break;
  }
  printf("    max_comp_size = %d\n", cp->max_comp_size);
  switch(cp->cp_rsiz)
  {
    case OPJ_STD_RSIZ: printf("          cp_rsiz = OPJ_STD_RSIZ\n"); break;
    case OPJ_CINEMA2K: printf("          cp_rsiz = OPJ_CINEMA2K\n"); break;
    case OPJ_CINEMA4K: printf("          cp_rsiz = OPJ_CINEMA4K\n"); break;
    case OPJ_MCT:      printf("          cp_rsiz = OPJ_MCT\n"); break;
  }
  printf("            tp_on = %d\n", cp->tp_on);
  printf("          tp_flag = %d\n", cp->tp_flag);
  printf("          tcp_mct = %d\n", cp->tcp_mct);
  printf("          jpip_on = %s\n", (cp->jpip_on ? "YES" : "NO"));
  printf("         mct_data = 0x%x\n", (unsigned int)cp->mct_data);
  printf("      max_cs_size = %d\n", cp->max_cs_size);
  printf("             rsiz = %u\n", cp->rsiz);
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

int mj2k_write_j2k(mj2k_image_t *image, const char *filename)
{
  printf("ENCODE\n");

  opj_cparameters_t parameters;
  opj_set_default_encoder_parameters(&parameters);

  show_encoder_parameters(&parameters);

  return 0;
}



