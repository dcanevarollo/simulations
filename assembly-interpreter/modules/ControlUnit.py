from models.InstructionRegister import InstructionRegister  # IR class.
from models.Register import Register  # Register class.
from modules.Pipeline import *  # Pipeline module.

# Global instance of the instruction register object.
ir = InstructionRegister()

# Dictionary of the assembly's labels.
labels = {

}

# Dictionary of registers of our CPU:
registers = {
    "esp": Register(),  # Summation entry.
    "ebp": Register(),
    "eax": Register(),
    "temp": Register(),
    "temp2": Register()
}


# Stores the indexes of the instructions that contains a label.
def store_labels():
    index = 0

    for instruction in ir.instructions:
        if is_label(instruction):
            labels[instruction.strip().strip(":")] = index

        index += 1


# Stores all the instructions in the instruction register to run them later.
def store(assembly, entry):
    registers.get("esp").content = entry  # Sets the entry to our esp register.

    for line in assembly:
        ir.instructions.append(line)

    store_labels()


# Executes each instruction stored on the instruction register.
def execute(instruction, f_equals, f_leave):
    opcode = instruction[1:6].strip()
    parameters = instruction[6:].strip().split(",")  # Get the list of parameters of the instruction.

    insert_pipeline(instruction)  # Inserts the current instruction in the pipeline.

    if opcode == "movl":
        # reg1 <- reg2
        registers.get(parameters[0]).content = registers.get(parameters[1]).content
    elif opcode == "mvil":
        # reg <- value
        registers.get(parameters[0]).content = int(parameters[1])
    elif opcode == "addl":
        # reg1 <- reg1 + reg2
        registers.get(parameters[0]).content += registers.get(parameters[1]).content
    elif opcode == "subl":
        # reg1 <- reg1 - reg2
        registers.get(parameters[0]).content -= registers.get(parameters[1]).content
    elif opcode == "cmpl":
        # Set the equals flag to active the jle instruction.
        if registers.get(parameters[0]).content <= registers.get(parameters[1]).content:
            f_equals = True
    elif opcode == "incl":
        # reg <- reg + 1
        registers.get(parameters[0]).content += 1
    elif opcode == "leave":
        # Prepares to return.
        f_leave = True
    elif opcode == "jle":
        if f_equals:
            insert_pipeline(discarded=ir.instructions[ir.instructions.index(instruction) + 1:])
            # The subroutine to be executed is the instructions below the label.
            # Furthermore, that label is the instruction's parameter[0].
            subroutine(ir.instructions[labels.get(parameters[0]) + 1:])
    elif opcode == "jmp":
        # Sends to the pipeline matrix the instructions that will be discarded.
        insert_pipeline(discarded=ir.instructions[ir.instructions.index(instruction) + 1:])
        # Jump to the label referenced on parameter[0].
        subroutine(ir.instructions[labels.get(parameters[0]) + 1:])
    elif opcode == "ret":
        # Exit the program routine if the leave flag is true.
        if f_leave:
            print("\nResult: %i.\n" % registers.get("eax").content)
            print_pipeline()  # Prints the pipeline when the program will be closed.
            input("\nPress enter to leave the prompt.\n> ")
            exit(0)

    return f_equals, f_leave


# Executes a subroutine (e.g., the lines below a label).
def subroutine(instructions):
    f_leave = False
    f_equals = False

    for instruction in instructions:
        # The flags must be parametrized cause their values cannot be lost.
        (f_equals, f_leave) = execute(instruction, f_equals, f_leave)


#  Main method called that executes the subroutines stored in the instruction register.
def decode():
    subroutine(ir.instructions)
