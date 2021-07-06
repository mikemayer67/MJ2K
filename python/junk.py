from markers.read import read as read_marker
from markers.siz import SIZMarker

soc = read_marker('ff01')
print((soc,soc.marker))

siz = read_marker('ff02002012196778901234567890')
print((siz,siz.marker,siz.len,siz.data,siz.Asiz,siz.Bsiz))

siz2 = SIZMarker(Asiz=3,Bsiz=123)
print((siz,siz2.marker,siz2.len,siz2.data,siz2.Asiz,siz2.Bsiz))


