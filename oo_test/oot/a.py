from .base import Base

print('Importing A')

class A(Base):
    def __init__(self,name):
        self.name = name
        super().__init__()

    def __call__(self):
        print(f'I am {self.name} # {self.id}')

