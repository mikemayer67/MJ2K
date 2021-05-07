class TRE:
    nextID = 1
    def __init__(self):
        self.id = TRE.nextID
        TRE.nextID += 1

        if not hasattr(self,'subid'):
            self.subid = 0

        cls = self.__class__.__name__.split('.')[-1]
        print(f'Constructed {cls} {self.id}.{self.subid}')



