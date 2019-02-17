# In this abstraction the instruction recorder has a set of instructions.
# In a real instruction recorder this don't happen. Actually, the register
# stores only the instruction that will be executed.


class InstructionRegister:
    def __init__(self, instructions=None):
        if instructions is None:
            instructions = []

        self.instructions = instructions
