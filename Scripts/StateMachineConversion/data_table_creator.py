# Utility script to create state machines for intrusive thoughts more easily.
# Creates a JSON file that can be imported as a data table (make sure you check "Ignore Extra Fields")
# Copyright TheCreator 2024

from os.path import exists
from colorama import Fore, Style
import inquirer
import json

# Defines to create data table JSON from
TARGET_FILE_NAME = "result.json"

class Transition:
    def __init__(self, target_row: str, chance_weight: float):
        self.target_row = target_row
        self.chance_weight = chance_weight
    
    def __str__(self) -> str:
        return f"Targeting {self.target_row} with chance weight {self.chance_weight}"

class State:
    def __init__(self, row_name: str, is_start_state: bool, example_voiceline: str):
        self.row_name = row_name
        self.is_start_state = is_start_state
        self.transitions: list[Transition] = []
        self.example_voiceline = example_voiceline

    def add_transition(self, transition: Transition):
        self.transitions.append(transition)

    def get_transition_to(self, row_name: str) -> Transition:
        valid_transitions = list(filter(lambda t: t.target_row == row_name , self.transitions))
        return valid_transitions[0]

    def has_transition_to(self, row_name: str) -> bool:
        valid_transitions = list(filter(lambda t: t.target_row == row_name , self.transitions))
        return len(valid_transitions) > 0
    
    def remove_transitions_by_row_name(self, row_name: str):
        new_transitions = list(filter(lambda t: t.target_row != row_name, self.transitions))
        self.transitions = new_transitions
    
    def __str__(self) -> str:
        transition_str = ""
        for transition in self.transitions:
            transition_str += f"Targeting {transition.target_row} with chance weight {transition.chance_weight}\n"
        return f"{self.row_name} (is start state: {self.is_start_state}):\n{transition_str}---------------"

states: list[State] = []

def find_state_by_row_name(row_name: str) -> State | None:
    return list(filter(lambda s: s.row_name == row_name, states))[0]

def validate_row_name(answers: dict, current: str) -> bool:
    if(not current):
        return False

    for state in states:
        if(state.row_name == current):
            return False
    return True

def validate_example_voiceline(answers: dict, current: str) -> bool:
    if(not current):
        return False
    return True

def validate_chance_weight(answers: dict, current: str) -> bool:
    try:
        weight = float(current)
        if(weight < 0.0):
            return False
    except ValueError:
        return False
    return True


# Load data from target file if it exists

if(exists(TARGET_FILE_NAME)):
    target_file = open(TARGET_FILE_NAME, "r")
    state_list = json.loads(target_file.read())

    for state_data in state_list:
        new_state = State(state_data["Name"], state_data["bCanBeStartState"], state_data["ExampleVoiceline"])

        for transition_data in state_data["Transitions"]:
            new_transition = Transition(transition_data["TargetRowName"], transition_data["ChanceWeight"])
            new_state.add_transition(new_transition)

        states.append(new_state)

    target_file.close()



# Prompt player to add a new thought state
while True:
    continue_add_question = [
        inquirer.Confirm("add_state", message=Fore.GREEN + "Add new thought state?" + Style.RESET_ALL),
    ]
    answers = inquirer.prompt(continue_add_question)
    if(answers["add_state"] == False):
        break
    
    new_state_questions = [
        inquirer.Text(
            "row_name", 
            Fore.CYAN + "Enter row name" + Style.RESET_ALL, 
            validate=validate_row_name,
        ),
        inquirer.Confirm(
            "is_start_state", 
            message=Fore.CYAN + "Is start state? " + Style.RESET_ALL,
        ),
        inquirer.Text(
            "example_voiceline", 
            Fore.CYAN + "Enter example voiceline for transition selection later" + Style.RESET_ALL,
            validate=validate_example_voiceline,
        ),
    ]
    answers = inquirer.prompt(new_state_questions)
    
    new_state = State(answers["row_name"], answers["is_start_state"], answers["example_voiceline"])
    states.append(new_state)
    print(Fore.GREEN + f"Added new state {new_state.row_name} with example voiceline '{new_state.example_voiceline}'." + Style.RESET_ALL)



# Prompt player to delete any existing thought states
delete_any_states_question = [
    inquirer.Confirm("delete_any_states", message=Fore.RED + "Delete any existing thought states? "+ Style.RESET_ALL)
]
answers = inquirer.prompt(delete_any_states_question)

if(answers["delete_any_states"]):
    delete_choices = []
    for state in states:
        delete_choices.append((state.example_voiceline, state))

    delete_states_questions = [
        inquirer.Checkbox(
            "deleted_states", 
            message=Fore.RED + "Delete states" + Style.RESET_ALL, 
            choices=delete_choices,
        ),
    ]

    answers = inquirer.prompt(delete_states_questions)
    
    for state in states:
        for deleted_state in answers["deleted_states"]:
            state.remove_transitions_by_row_name(deleted_state.row_name)
    
    for deleted_state in answers["deleted_states"]:
        states.remove(deleted_state)



# Prompt player to add/modify transitions for each state
for state in states:
    should_modify_transitions_question = [
        inquirer.Confirm(
            "modify_transitions",
            message=Fore.YELLOW + "Modify transitions of state with voiceline " + Fore.CYAN + f"'{state.example_voiceline}'?" + Style.RESET_ALL,
        ),
    ]
    answers = inquirer.prompt(should_modify_transitions_question)
    if(answers["modify_transitions"] == False):
        continue
    
    transition_choices = []
    default_transition_choices = []
    for otherstate in states:
        if(otherstate == state):
            continue

        if(state.has_transition_to(otherstate.row_name)):
            existing_transition = state.get_transition_to(otherstate.row_name)
            transition_choices.append((otherstate.example_voiceline, existing_transition))
            default_transition_choices.append(existing_transition)
        else:
            transition_choices.append((otherstate.example_voiceline, Transition(otherstate.row_name, 1.0)))

    modify_transitions_question = [
        inquirer.Checkbox(
            "new_transitions", 
            message="Add or remove transitions for state with voiceline " + Fore.CYAN + f"'{state.example_voiceline}'" + Style.RESET_ALL,
            choices=transition_choices,
            default=default_transition_choices,
        )
    ]
    answers = inquirer.prompt(modify_transitions_question)

    state.transitions.clear()
    for new_transition in answers["new_transitions"]:
        state.add_transition(new_transition)
    
    for transition in state.transitions:
        target_state = find_state_by_row_name(transition.target_row)

        chance_weight_question = [
            inquirer.Text(
                "chance_weight", 
                message="Modify chance weight for transition to " + Fore.CYAN + f"'{target_state.example_voiceline}'" + Style.RESET_ALL,
                validate=validate_chance_weight,
                default=str(transition.chance_weight),
            ),
        ]
        answers = inquirer.prompt(chance_weight_question)
        transition.chance_weight = answers["chance_weight"]



# Save data to result file
payload = []

for state in states:
    payload_transitions = []
    for transition in state.transitions:
        payload_transitions.append({
            "TargetRowName": transition.target_row,
            "ChanceWeight": transition.chance_weight
        })

    payload.append({
        "Name": state.row_name,
        "Sound": "None",
        "Transitions": payload_transitions,
        "bCanBeStartState": state.is_start_state,
        "ExampleVoiceline": state.example_voiceline
    })

target_file = open(TARGET_FILE_NAME, "w")
target_file.write(json.dumps(payload))
target_file.close()

print(Fore.GREEN + f"Success! The created file {TARGET_FILE_NAME} is ready to be imported into Unreal!" + Style.RESET_ALL)