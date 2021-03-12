from .base import Base

class B(Base):
    def __init__(self,name):
        self.handle = name
        super().__init__()

