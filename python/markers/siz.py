from .marker import MarkerSegment
from .const import SIZ

class SIZMarker(MarkerSegment):
    def __init__(self,*args,**kwargs):
        if args:
            super().__init__(*args)
        elif kwargs:
            print(f'kwargs={kwargs}')
            self.marker = SIZ
            self.len = 20
            self.data = f"{kwargs['Asiz']:02}{kwargs['Bsiz']:04}xxxxxxxxxxxxxx"

    @property
    def Asiz(self):
        return int(self.data[:2])

    @property
    def Bsiz(self):
        return int(self.data[2:6])

