#include "cod.h"
#include "formatter.h"
#include "xfrm_cod.h"

void COD::init(int fd)
{
  MarkerSegment::init(fd, Marker::COD );

  _S                    = get_field(fd, 1, "Scod");
  _SG.prog_order        = get_field(fd, 1, "SGcod.prog_order");
  _SG.num_layers        = get_field(fd, 2, "SGcod.num_layers");
  _SG.mcomp_xform       = get_field(fd, 1, "SGcod.mcomp_xform");
  _SP.num_decomp_levels = get_field(fd, 1, "SGcod.num_decomp_levels");
  _SP.code_block_width  = get_field(fd, 1, "SGcod.code_block_width");
  _SP.code_block_height = get_field(fd, 1, "SGcod.code_block_height");
  _SP.code_block_style  = get_field(fd, 1, "SGcod.code_block_style");
  _SP.wavelet           = get_field(fd, 1, "SGcod.wavelet");

  for(uint16_t i=0; i<_size-12; ++i)
  {
    uint8_t pw = get_field(fd, 1, "SGcod.precint_width");
    _SP.precinct_width.push_back(pw);
  }
}

void COD::display(std::ostream &s) const
{
  Xfrm_Scod  scod;
  Xfrm_SGcod sgcod_po(Xfrm_SGcod::ProgOrder);
  Xfrm_SGcod sgcod_mcx(Xfrm_SGcod::McompXform);
  Xfrm_SPcod spcod_cbw(Xfrm_SPcod::CodeBlockWidth);
  Xfrm_SPcod spcod_cbh(Xfrm_SPcod::CodeBlockHeight);
  Xfrm_SPcod spcod_cbs(Xfrm_SPcod::CodeBlockStyle);
  Xfrm_SPcod spcod_wav(Xfrm_SPcod::Wavelet);
  Xfrm_SPcod spcod_psz(Xfrm_SPcod::PrecintSize);

  Formatter out(s);
  out.display(_size,                 2, "Lcod");
  out.display(_S,                    1, "Scod",    scod);
  out.display(_SG.prog_order,        1, "SGcod-A", sgcod_po, "progression order");
  out.display(_SG.num_layers,        2, "SGcod-B", "number of layers");
  out.display(_SG.mcomp_xform,       1, "SGcod-C", sgcod_mcx);
  out.display(_SP.num_decomp_levels, 1, "SPcod-D", "number of decomp levels");
  out.display(_SP.code_block_width,  1, "SPcod-E", spcod_cbw, "code block width");
  out.display(_SP.code_block_height, 1, "SPcod-F", spcod_cbh, "code block height");
  out.display(_SP.code_block_style,  1, "SPcod-G", spcod_cbs);
  out.display(_SP.wavelet,           1, "SPcod-H", spcod_wav);

  for(int i=0; i<_SP.precinct_width.size(); ++i)
  {
    out.display(_SP.precinct_width.at(i), 1, "SPCOD-I", i, spcod_psz, "precinct size (PPx,PPy)");
  }
}
