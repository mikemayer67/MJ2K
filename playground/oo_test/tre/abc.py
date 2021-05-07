from .tre import TRE

class ABC (TRE):
    ABC.nextID = 1
    def __init__(self):
        self.subid = ABC.nextID
        super.__init__(self)
        ABC.nextID += 1

