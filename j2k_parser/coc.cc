#include "coc.h"
#include "formatter.h"
#include "xfrm_coc.h"

void COC::init(int fd, const SIZ &siz)
{
  MarkerSegment::init(fd, Marker::COC );

  _lenC = (siz.Csiz() < 257 ? 1 : 2);

  _C                    = get_field(fd, _lenC, "Ccoc");
  _S                    = get_field(fd, 1,     "Scoc");
  _SP.num_decomp_levels = get_field(fd, 1,     "SGcoc.num_decomp_levels");
  _SP.code_block_width  = get_field(fd, 1,     "SGcoc.code_block_width");
  _SP.code_block_height = get_field(fd, 1,     "SGcoc.code_block_height");
  _SP.code_block_style  = get_field(fd, 1,     "SGcoc.code_block_style");
  _SP.wavelet           = get_field(fd, 1,     "SGcoc.wavelet");

  for(uint16_t i=0; i < (_size - (8+_lenC)); ++i)
  {
    uint8_t pw = get_field(fd, 1, "SGcoc.precint_width");
    _SP.precinct_width.push_back(pw);
  }
}

void COC::display(std::ostream &s) const
{
  Xfrm_Scoc  scoc;

  // the following are not a mistake, SPcod and Spcoc have same format
  Xfrm_SPcod spcoc_cbw(Xfrm_SPcod::CodeBlockWidth);
  Xfrm_SPcod spcoc_cbh(Xfrm_SPcod::CodeBlockHeight);
  Xfrm_SPcod spcoc_cbs(Xfrm_SPcod::CodeBlockStyle);
  Xfrm_SPcod spcoc_wav(Xfrm_SPcod::Wavelet);
  Xfrm_SPcod spcoc_psz(Xfrm_SPcod::PrecintSize);

  Formatter out(s);
  out.display(_size,                 2, "Lcoc");
  out.display(_C,                _lenC, "Ccoc",    "component index");
  out.display(_S,                    1, "Scoc",    scoc);
  out.display(_SP.num_decomp_levels, 1, "SPcoc-A", "number of decomp levels");
  out.display(_SP.code_block_width,  1, "SPcoc-B", spcoc_cbw, "code block width");
  out.display(_SP.code_block_height, 1, "SPcoc-C", spcoc_cbh, "code block height");
  out.display(_SP.code_block_style,  1, "SPcoc-D", spcoc_cbs);
  out.display(_SP.wavelet,           1, "SPcoc-E", spcoc_wav);

  for(int i=0; i<_SP.precinct_width.size(); ++i)
  {
    out.display(_SP.precinct_width.at(i), 1, "SPCOC-F", i, spcoc_psz, "precinct size (PPx,PPy)");
  }
}
