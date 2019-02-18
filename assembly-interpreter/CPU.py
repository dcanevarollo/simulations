from modules.ControlUnit import *

try:
    file_name = input("Assembly's file name:\n> ")

    # Adds the extension '.txt' if the name read don't has one.
    if ".txt" not in file_name:
        file_name += ".txt"

    entry = int(input("\nInitial value:\n> "))

    assembly = open("docs/" + file_name, "r")
    store(assembly, entry)  # Stores the instructions and the initial value.
    decode()  # Main function that interprets the assembly's code.
    assembly.close()
except FileNotFoundError:
    print("\nError! Could not open the file.")
    input("\nPress enter to exit the prompt.\n> ")
    exit(1)
except ValueError:
    print("\nError! The initial value must be an integer.")
    input("\nPress enter to exit the prompt.\n> ")
    exit(1)
