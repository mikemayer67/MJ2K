from .base import Base

class A(Base):
    def __init__(self,name):
        self.name = name
        super().__init__()

