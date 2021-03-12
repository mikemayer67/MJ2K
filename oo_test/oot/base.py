class Base:
    nextID = 1
    def __init__(self,id=None):
        if id is None:
            self.id = Base.nextID
            Base.nextID += 1
        else:
            self.id = id


