from .base import Base

print('Importing B')

class B(Base):
    def __init__(self,name):
        self.handle = name
        super().__init__()

    def __call__(self):
        print(f'Ju suis {self.handle} # {self.id}')

