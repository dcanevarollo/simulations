# List of stages of our pipeline.
stages = [
    "FI",
    "DI",
    "CO",
    "FO",
    "EI",
    "WO"
]

# Pipeline matrix that will be displayed in the end.
pipeline = []


# Checks if the parameter instruction is a label or has an opcode.
def is_label(instruction):
    # In instructions that has to be executed the first character is a blank tabulation.
    return instruction[0] != "\t"


# Insert the stages of the current instruction in the pipeline matrix.
def insert_pipeline(instruction="", discarded=None):
    # The discarded list is the instructions that will be discarded from the
    # pipeline after an interruption on the flow.
    if discarded is None:
        if not is_label(instruction):
            opcode = instruction[1:6].strip()

            # The string format of a list is something like ['a1', 'a2', ..., 'an'].
            # To make a more picturesque visualization we reformat this string,
            # removing the brackets, the commas,the apostrophe, and the blank spaces.
            pipeline.append(str(stages).strip("[").strip("]").replace("'", "|")
                            .replace(",", "").replace(" ", "") + "  (" + opcode + ")")
    else:
        i = 4  # Controls the interruption flow on the pipeline.

        for instruction in discarded:
            if i > 0 and not is_label(instruction):
                opcode = instruction[1:6].strip()

                pipeline.append(str(stages[0:i]).strip("[").strip("]").replace("'", "|")
                                .replace(",", "").replace(" ", "") + "  (" + opcode + ")")

                i -= 1


def print_pipeline():
    tab = 0

    print("Pipeline:\n")
    for set_stages in pipeline:
        temp = "\t" + str(set_stages)
        # expandtabs(tabsize=) sets the size of the tabulation ('\t').
        # For each set of stages printed we increments 4u as tab size
        # to simulates the pipeline flow.
        print(temp.expandtabs(tab))
        tab += 4
