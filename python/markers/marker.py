from .const import *

class Marker:

    def __init__(self,marker):
        self.marker = marker

class MarkerSegment(Marker):
    def __init__(self,marker,data):
        super().__init__(marker)
        self.len = int(data[:4])
        self.data = data[4:4+self.len]

