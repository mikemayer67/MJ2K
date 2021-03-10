#ifndef MAM_J2K_H
#define MAM_J2K_H

#include <sys/types.h>
#include <stdint.h>

#include <openjpeg.h>

typedef uint8_t        mj2k_byte_t;
typedef mj2k_byte_t   *mj2k_bytes_t;

typedef uint8_t        mj2k_pixel_t;

extern uint32_t mamj2k_cp_reduce;
extern uint32_t mamj2k_cp_layer;

typedef struct {
  uint32_t      x0;
  uint32_t      y0;
  uint32_t      w;
  uint32_t      h;
  uint32_t      dx;
  uint32_t      dy;
  mj2k_pixel_t *pixels;
} mj2k_comp_t;

typedef struct 
{
  uint32_t     x0;
  uint32_t     y0;
  uint32_t     x1;
  uint32_t     y1;
  uint32_t     ncomp;
  mj2k_comp_t *comp;
} mj2k_image_t;

typedef struct
{
  int   irreversible;
  int   tcp_numlayers;
  int   numresolution;
  int   layer_qual_type; // 0 = cp_distoratio, 1 = tcp_rates
  float layer_qual_values[100];
} mj2k_cparam_t;


extern mj2k_image_t *mj2k_read_j2k(const char *filename);
extern mj2k_image_t *mj2k_parse_j2k(const mj2k_bytes_t j2k, off_t length);

extern int mj2k_write_j2k(mj2k_image_t *image, mj2k_cparam_t *cparam, const char *filename);

extern const char *mj2k_opj_version(void);

extern void mj2k_free_image(mj2k_image_t *image);

#endif
