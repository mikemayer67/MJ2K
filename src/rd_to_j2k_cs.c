#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <openjpeg.h>


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

typedef struct {
  uint8_t *data;
  uint64_t pos;
  uint64_t end;
  uint64_t alloc;
} cs_buffer_t;

void cs_buffer_init(cs_buffer_t *csb)
{
  csb->data   = NULL;
  csb->pos    = 0;
  csb->end    = 0;
  csb->alloc  = 0;
}


OPJ_SIZE_T write_f(void *p_buffer, OPJ_SIZE_T p_nb_bytes, void *p_user_data)
{
  printf("\nwrite_f(0x%08lx, %lu, 0x%08lx)\n", (unsigned long)p_buffer, p_nb_bytes, (unsigned long)p_user_data);

  if(p_user_data == NULL) { return 0; }
  cs_buffer_t *csb = p_user_data;

  uint64_t new_pos = csb->pos + p_nb_bytes;
  if( new_pos > csb->alloc )
  {
    csb->data = realloc(csb->data, new_pos);
    csb->alloc = new_pos;
  }

  memcpy( csb->data + csb->pos, p_buffer, p_nb_bytes );
  csb->pos = new_pos;
  if(new_pos > csb->end) csb->end = new_pos;

  return p_nb_bytes;
}

OPJ_OFF_T skip_f(OPJ_OFF_T p_nb_bytes, void *p_user_data)
{
  printf("\nskip_f(%lld, 0x%08lx)\n", p_nb_bytes, (unsigned long)p_user_data);

  if(p_user_data == NULL) { return 0; }
  cs_buffer_t *csb = p_user_data;

  uint64_t new_pos = csb->pos + p_nb_bytes;
  if( new_pos > csb->alloc )
  {
    csb->data = realloc(csb->data, new_pos);
    csb->alloc = new_pos;
  }

  return p_nb_bytes;
}

OPJ_BOOL seek_f(OPJ_OFF_T p_nb_bytes, void *p_user_data)
{
  printf("\nseek_f(%lld, 0x%08lx)\n", p_nb_bytes, (unsigned long)p_user_data);

  if(p_user_data == NULL) { return OPJ_FALSE; }
  cs_buffer_t *csb = p_user_data;

  uint64_t new_pos = p_nb_bytes;
  if( new_pos > csb->alloc )
  {
    csb->data = realloc(csb->data, new_pos);
    csb->alloc = new_pos;
  }

  return OPJ_TRUE;
}

#define USAGE() \
  printf("\nUsage:: %s [(--tilesize|-t) width height] rd_file [j2k_file]\n\n",argv[0]);\
  exit(1);

#define _USAGE_ERROR_FMT(FMT,...) { \
  printf("Sorry:: " FMT "\n", __VA_ARGS__); \
  USAGE(); \
}
#define _USAGE_ERROR(ERR) { \
  printf("Sorry:: " ERR "\n"); \
  USAGE(); \
}
#define _USAGE_SELECT(fmt,arg1,arg2,FUNC,...) FUNC
#define USAGE_ERROR(...) _USAGE_SELECT(__VA_ARGS__,_USAGE_ERROR_FMT,_USAGE_ERROR_FMT,_USAGE_ERROR)(__VA_ARGS__)
  

int main(int argc,const char **argv)
{
  char *rd_file  = NULL;
  char *j2k_file = NULL;

  int tdx=0;
  int tdy=0;

  for(int i=1; i<argc; ++i)
  {
    if( argv[i][0] == '-' )
    {
      if( strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h") == 0)
      {
        USAGE();
      }
      else if( strcmp(argv[i],"--tilesize")==0 || strcmp(argv[i],"-t") == 0)
      {
        if(tdx > 0)      USAGE_ERROR("Can only specify tilesize once");
        if(i == argc-1 ) USAGE_ERROR("Missing tile size");
        if(i == argc-2 ) USAGE_ERROR("Missing tile height");

        char *end;
        ++i;
        tdx = strtol(argv[i], &end, 10);
        if( *end != '\0' ) USAGE_ERROR("Invalid tile width (%s)", argv[i]);
        if( tdx <= 0) USAGE_ERROR("Tile width must be postitive (not %d)", tdx);
        ++i;
        tdy = strtol(argv[i], &end, 10);
        if( *end != '\0' ) USAGE_ERROR("Invalid tile height (%s)", argv[i]);
        if( tdy <= 0) USAGE_ERROR("Tile height must be postitive (not %d)", tdy);
      }
      else
      {
        USAGE_ERROR("Unrecognized option (%s)",argv[i]);
      }
    }
    else if(rd_file == NULL)
    {
      rd_file = (char *)malloc(1+strlen(argv[i]));
      strcpy(rd_file,argv[i]);
    }
    else if(j2k_file == NULL)
    {
      rd_file = (char *)malloc(1+strlen(argv[i]));
      strcpy(rd_file,argv[i]);
    }
    else
    {
      USAGE_ERROR("Too many arguments");
    }
  }

  if(rd_file == NULL)
  {
    USAGE_ERROR("Missing rd_file");
  }

  if(j2k_file == NULL) 
  {
    j2k_file = (char *)malloc(5+strlen(rd_file));
    strcpy(j2k_file,rd_file);
    strcat(j2k_file,".j2k");
  }

  const char *version = opj_version();
  printf("version = %s\n",version);

  printf("rd_file: %s\n", rd_file);
  printf("j2k_file: %s\n", j2k_file);
  if(tdx == 0) {
    printf("single tile\n\n");
  } else {
    printf("tile size: %d x %d\n\n", tdx, tdy);
  }

  int src = open(rd_file,O_RDONLY);
  if(src < 0) {
    printf("\nSorry:: Could not open %s: %s\n\n", rd_file, strerror(errno));
    exit(1);
  }

  int nrow = 0;
  int ncol = 0;
  int ncomp = 0;
  read(src, &nrow, 2);
  read(src, &ncol, 2);
  read(src, &ncomp, 2);

  OPJ_COLOR_SPACE clrspc = (ncomp==3 ? OPJ_CLRSPC_SRGB : OPJ_CLRSPC_GRAY);

  opj_image_cmptparm_t *cmptparm = (opj_image_cmptparm_t *)malloc(ncomp * sizeof(opj_image_cmptparm_t));

  for(int i=0; i<ncomp; ++i)
  {
    cmptparm[i].dx = 1;
    cmptparm[i].dy = 1;
    cmptparm[i].w = ncol;
    cmptparm[i].h = nrow;
    cmptparm[i].x0 = 0;
    cmptparm[i].y0 = 0;
    cmptparm[i].prec = 8;
    cmptparm[i].bpp  = 8;
    cmptparm[i].sgnd = 0;
  }

  opj_image_t *image = opj_image_create( ncomp, cmptparm, clrspc );
  image->x0 = 0;
  image->y0 = 0;
  image->x1 = ncol;
  image->y1 = nrow;

  for(int i=0; i<ncomp; ++i)
  {
    uint64_t nbytes = nrow * ncol * sizeof(int32_t);

    uint64_t to_read = nbytes;
    char *pos = (char *)image->comps[i].data;
    while(to_read)
    {
      int64_t nread = read(src, pos, to_read);
      if(nread < 0) {
        printf("\nSorry:: Failed to read data from %s: %s\n\n", rd_file, strerror(errno));
        exit(1);
      }
      pos += nread;
      to_read -= nread;
    }
  }

  printf("Image created: 0x%016lx\n", (unsigned long)image);
  printf("(x0,y0): (%u,%u)\n", image->x0,image->y0);
  printf("(x1,y1): (%u,%u)\n", image->x1,image->y1);
  printf("  ncomp: %u\n", image->color_space);
  printf("  comps: 0x%016lx\n", (unsigned long)(image->comps));
  printf("    icc: 0x%016lx\n", (unsigned long)(image->icc_profile_buf));
  printf("   Licc: %u\n", image->icc_profile_len);
  for(int i=0; i<ncomp; ++i)
  {
    printf(" ic[%i].dx      = %u\n",i,image->comps[i].dx);
    printf(" ic[%i].dy      = %u\n",i,image->comps[i].dy);
    printf(" ic[%i].w       = %u\n",i,image->comps[i].w);
    printf(" ic[%i].h       = %u\n",i,image->comps[i].h);
    printf(" ic[%i].x0      = %u\n",i,image->comps[i].x0);
    printf(" ic[%i].y0      = %u\n",i,image->comps[i].y0);
    printf(" ic[%i].prec    = %u\n",i,image->comps[i].prec);
    printf(" ic[%i].bpp     = %u\n",i,image->comps[i].bpp );
    printf(" ic[%i].sgnd    = %u\n",i,image->comps[i].sgnd);
    printf(" ic[%i].resno   = %u\n",i,image->comps[i].resno_decoded);
    printf(" ic[%i].factor  = %u\n",i,image->comps[i].factor);
    printf(" ic[%i].alpha   = %u\n",i,image->comps[i].alpha);
    printf(" ic[%i].data    = 0x%016lx\n",i, (unsigned long)image->comps[i].data);
    printf("              0 : 0x%08x\n",image->comps[i].data[0]);
    printf("            100 : 0x%08x\n",image->comps[i].data[100]);
    printf("            200 : 0x%08x\n",image->comps[i].data[200]);
  }

  opj_cparameters_t cp;
  opj_set_default_encoder_parameters(&cp);

  if(tdx>0 && tdy>0)
  {
    cp.tile_size_on = OPJ_TRUE;
    cp.cp_tdx = tdx;
    cp.cp_tdy = tdy;
  }

  cp.cp_comment = "This was created using rd_to_j2k_cs";

  printf("\nCPARAM\n");

  printf(" tile_size_on      = %s\n", (cp.tile_size_on ? "TRUE" : "FALSE"));
  printf(" cp_tx0            = %d\n", cp.cp_tx0);
  printf(" cp_ty0            = %d\n", cp.cp_ty0);
  printf(" cp_tdx            = %d\n", cp.cp_tdx);
  printf(" cp_tdy            = %d\n", cp.cp_tdy);
  printf(" cp_disto_alloc    = %d\n", cp.cp_disto_alloc);
  printf(" cp_fixed_alloc    = %d\n", cp.cp_fixed_alloc);
  printf(" cp_fixed_quality  = %d (%s)\n", cp.cp_fixed_quality, 
      cp.cp_fixed_alloc ? "used" : "not used");
  printf(" cp_matrice        = 0x%016lx\n", (unsigned long)cp.cp_matrice);
  printf(" cp_comment        = %s\n", cp.cp_comment);
  printf(" csty              = %d\n", cp.csty);
  printf(" prog_order        = %s\n", (
        cp.prog_order == 0 ? "LRCP" :
        cp.prog_order == 1 ? "RLCP" :
        cp.prog_order == 2 ? "RPCL" :
        cp.prog_order == 3 ? "PCRL" :
        cp.prog_order == 4 ? "CPRL" : "UNKNOWN" ) );
  printf(" numpocs           = %u\n", cp.numpocs);
  for(int i=0; i<cp.numpocs; ++i) {
    printf(" poc[%d] resno0    = %u\n", i, cp.POC[i].resno0);
    printf(" poc[%d] compno0   = %u\n", i, cp.POC[i].compno0);
    printf(" poc[%d] layno1    = %u\n", i, cp.POC[i].layno1);
    printf(" poc[%d] resno1    = %u\n", i, cp.POC[i].resno1);
    printf(" poc[%d] compno1   = %u\n", i, cp.POC[i].compno1);
    printf(" poc[%d] layno0    = %u\n", i, cp.POC[i].layno0);
    printf(" poc[%d] precno0   = %u\n", i, cp.POC[i].precno0);
    printf(" poc[%d] precno1   = %u\n", i, cp.POC[i].precno1);
    printf(" poc[%d] prg1      = %s\n", i, (
          cp.POC[i].prg1 == 0 ? "LRCP" :
          cp.POC[i].prg1 == 1 ? "RLCP" :
          cp.POC[i].prg1 == 2 ? "RPCL" :
          cp.POC[i].prg1 == 3 ? "PCRL" :
          cp.POC[i].prg1 == 4 ? "CPRL" : "UNKNOWN" ) );
    printf(" poc[%d[ prg                      = %s\n", i, (
          cp.POC[i].prg == 0 ? "LRCP" :
          cp.POC[i].prg == 1 ? "RLCP" :
          cp.POC[i].prg == 2 ? "RPCL" :
          cp.POC[i].prg == 3 ? "PCRL" :
          cp.POC[i].prg == 4 ? "CPRL" : "UNKNOWN" ) );
    printf(" poc[%d] progorder = %s\n", i, cp.POC[i].progorder);
    printf(" poc[%d] tile      = %u\n", i, cp.POC[i].tile);
    printf(" poc[%d] tx0       = %d\n", i, cp.POC[i].tx0);
    printf(" poc[%d] tx1       = %d\n", i, cp.POC[i].tx1);
    printf(" poc[%d] ty0       = %d\n", i, cp.POC[i].ty0);
    printf(" poc[%d] ty1       = %d\n", i, cp.POC[i].ty1);
    printf(" poc[%d] layS      = %u\n", i, cp.POC[i].layS);
    printf(" poc[%d] resS      = %u\n", i, cp.POC[i].resS);
    printf(" poc[%d] compS     = %u\n", i, cp.POC[i].compS);
    printf(" poc[%d] prcS      = %u\n", i, cp.POC[i].prcS);
    printf(" poc[%d] layE      = %u\n", i, cp.POC[i].layE);
    printf(" poc[%d] resE      = %u\n", i, cp.POC[i].resE);
    printf(" poc[%d] compE     = %u\n", i, cp.POC[i].compE);
    printf(" poc[%d] prcE      = %u\n", i, cp.POC[i].prcE);
    printf(" poc[%d] txS       = %u\n", i, cp.POC[i].txS);
    printf(" poc[%d] txE       = %u\n", i, cp.POC[i].txE);
    printf(" poc[%d] tyS       = %u\n", i, cp.POC[i].tyS);
    printf(" poc[%d] tyE       = %u\n", i, cp.POC[i].tyE);
    printf(" poc[%d] dx        = %u\n", i, cp.POC[i].dx);
    printf(" poc[%d] dy        = %u\n", i, cp.POC[i].dy);
    printf(" poc[%d] lay_t     = %u\n", i, cp.POC[i].lay_t);
    printf(" poc[%d] res_t     = %u\n", i, cp.POC[i].res_t);
    printf(" poc[%d] comp_t    = %u\n", i, cp.POC[i].comp_t);
    printf(" poc[%d] prc_t     = %u\n", i, cp.POC[i].prc_t);
    printf(" poc[%d] tx0_t     = %d\n", i, cp.POC[i].tx0_t);
    printf(" poc[%d] ty0_t     = %d\n", i, cp.POC[i].ty0_t);
  }
  printf(" tcp_numlayers     = %d\n", cp.tcp_numlayers);
  printf(" tcp_rates         :");
  if( cp.cp_disto_alloc ) {
    for(int i=0; i<cp.numpocs; ++i) printf(" %f", cp.tcp_rates[i]);
  }
  printf("\n");
  printf(" tcp_distoratio    :");
  if( cp.cp_fixed_quality ) {
    for(int i=0; i<cp.numpocs; ++i) printf(" %f", cp.tcp_distoratio[i]);
  }
  printf("\n");
  printf(" numresolution     = %d\n", cp.numresolution);
  printf(" cblockw_init      = %d\n", cp.cblockw_init);
  printf(" cblockh_init      = %d\n", cp.cblockh_init);
  printf(" mode              = %d\n", cp.mode);
  printf(" irreversible      = %d\n", cp.irreversible);
  printf(" roi_compno        = %d\n", cp.roi_compno);
  printf(" roi_shift         = %d\n", cp.roi_shift);
  printf(" res_spec          = %d\n", cp.res_spec);
  printf(" prcw_init         :");
  for(int i=0; i<cp.res_spec; ++i) { printf(" %d", cp.prcw_init[i]); }
  printf("\n");
  printf(" prch_init         :");
  for(int i=0; i<cp.res_spec; ++i) { printf(" %d", cp.prch_init[i]); }
  printf("\n");
  printf(" infile            = %s\n", cp.infile);
  printf(" outfile           = %s\n", cp.outfile);
  printf("---deprecated--\n");
  printf(" index_on          = %d\n", cp.index_on);
  printf(" index             = %s\n", cp.index);
  printf(" image_offset_x0   = %d\n", cp.image_offset_x0);
  printf(" image_offset_y0   = %d\n", cp.image_offset_y0);
  printf(" subsampling_dx    = %d\n", cp.subsampling_dx);
  printf(" subsampling_dy    = %d\n", cp.subsampling_dy);
  printf(" decod_format      = %d\n", cp.decod_format);
  printf(" cod_format        = %d\n", cp.cod_format);
  printf("---deprecated (use rsiz, profile, and max comp size--\n");
  printf(" max_comp_size     = %d\n", cp.max_comp_size);
  printf("---deprecated (use rsiz, profile, and extension)--\n");
  printf(" cp_rsiz           = %s\n", (
        cp.cp_rsiz == 0 ? "OPJ_STD_RSIZ" :
        cp.cp_rsiz == 3 ? "OPJ_CINEMA2K" :
        cp.cp_rsiz == 4 ? "OPJ_CINEMA4K" :
        cp.cp_rsiz == 0x8100 ? "OPJ_MCT" : "???" ));
  printf(" tp_on             = 0x%02x\n", cp.tp_on);
  printf(" tp_flag           = 0x%02x\n", cp.tp_flag);
  printf(" tcp_mct           = 0x%02x\n", cp.tcp_mct);
  printf(" jpip_on           = %s\n", (cp.jpip_on ? "TRUE" : "FALSE"));
  printf(" mct_data          = 0x%016lx\n", (unsigned long)cp.mct_data);
  printf(" max_cs_size       = %d\n", cp.max_cs_size);
  printf(" rsiz              = %u\n", cp.rsiz);


  opj_codec_t *codec = opj_create_compress(OPJ_CODEC_J2K);
  printf("\n\n");
  printf("codec  = 0x%016lx\n", (unsigned long)codec);

  opj_set_info_handler(codec, info_handler, NULL);
  opj_set_warning_handler(codec, warning_handler, NULL);
  opj_set_error_handler(codec, error_handler, NULL);

  opj_setup_encoder(codec, &cp, image);

  const char *extra[] = {"PLT=YES", NULL};
  opj_encoder_set_extra_options(codec, extra);

  opj_stream_t *j2k_stream = opj_stream_default_create(OPJ_FALSE);
  printf("stream = 0x%016lx\n",(unsigned long)j2k_stream);

  cs_buffer_t cs_buffer;

  opj_stream_set_write_function(j2k_stream, write_f);
  opj_stream_set_skip_function(j2k_stream, skip_f);
  opj_stream_set_seek_function(j2k_stream, seek_f);
  opj_stream_set_user_data(j2k_stream, &cs_buffer, NULL);

//  opj_stream_set_user_data_length(5);

  OPJ_BOOL rc = opj_start_compress(codec,image,j2k_stream);
  printf("\nCompression started: rc=%s\n", (rc ? "TRUE" : "FALSE"));

  rc = opj_encode(codec, j2k_stream);
  printf("\nEncode complete:     rc=%s\n", (rc ? "TRUE" : "FALSE"));

  rc = opj_end_compress(codec, j2k_stream);
  printf("\nCompression complete:     rc=%s\n", (rc ? "TRUE" : "FALSE"));

  opj_stream_destroy(j2k_stream);
  opj_destroy_codec(codec);
  opj_image_destroy(image);
  free(cmptparm);

  printf("CS Buffer::\n");
  printf("      data: 0x%08lx\n", (unsigned long)cs_buffer.data);
  printf("       pos: %llu\n",    cs_buffer.pos);
  printf("       end: %llu\n",    cs_buffer.end);
  printf("     alloc: %llu\n",    cs_buffer.alloc);


  int out = open(j2k_file, O_CREAT, 0644);
  write(out, cs_buffer.data, cs_buffer.end);
  close(out);
  

  free(cs_buffer.data);
  free(j2k_file);

  return 0;
}
