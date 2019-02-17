# Each register on our simulation will store a value/content.
# Practically, it's only a representation of a "special variable".


class Register:
    def __init__(self, content=0):
        self.content = int(content)
