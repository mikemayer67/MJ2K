from .const import *
from .marker import Marker
from .marker import MarkerSegment
from .siz import SIZMarker

def read(data):
        marker = data[:4]
        if marker in [SOC, 'ffe0', 'ffe1']:
            return Marker(marker)
        elif marker == SIZ:
            return SIZMarker(marker,data[4:])
        else:
            return MarkerSegment(marker,data[4:])

