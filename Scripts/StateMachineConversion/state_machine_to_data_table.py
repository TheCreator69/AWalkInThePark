# Utility script to convert downloaded JSON files describing state machine diagrams into JSON files,
# ready to be used for convert to a data table in Unreal.
# 
# State machine designer used: https://matthewscholefield.github.io/fsm/
# Copyright TheCreator 2024

import json

# Defines to create data table JSON from
DIAGRAM_FILE_NAME = "exportedToJson.json"
TARGET_FILE_NAME = "result.json"

class Transition:
    def __init__(self, target_row: str, chance_weight: float):
        self.target_row = target_row
        self.chance_weight = chance_weight
    
    def __str__(self) -> str:
        return f"Targeting {self.target_row} with chance weight {self.chance_weight}"

class State:
    def __init__(self, row_name: str, is_start_state: bool):
        self.row_name = row_name
        self.is_start_state = is_start_state
        self.transitions: list[Transition] = []

    def add_transition(self, transition: Transition):
        self.transitions.append(transition)
    
    def __str__(self) -> str:
        transition_str = ""
        for transition in self.transitions:
            transition_str += f"Targeting {transition.target_row} with chance weight {transition.chance_weight}\n"
        return f"{self.row_name} (is start state: {self.is_start_state}):\n{transition_str}---------------"



# Reading and parsing state machine export into JSON for data table import
diagram_file = open(DIAGRAM_FILE_NAME, "r")

diagram_export = json.loads(diagram_file.read())
nodes: list = diagram_export["nodes"]
links: list = diagram_export["links"]

states: list[State] = []

for node in nodes:
    # I mark states that can be start states as "accept states" in the diagram creator because it's very convenient (but technically incorrect)
    states.append(State(node["text"], node["isAcceptState"]))

for link in links:
    if(link["type"] == "SelfLink"):
        node_index = int(link["node"])
        node = nodes[node_index]

        chance_weight = 1.0
        try:
            chance_weight = float(link["text"])
        except ValueError:
            chance_weight = 1.0

        new_transition = Transition(node["text"], chance_weight)
        states[node_index].add_transition(new_transition)
    elif(link["type"] == "Link"):
        target_node_index = int(link["nodeB"])
        target_node = nodes[target_node_index]

        chance_weight = 1.0
        try:
            chance_weight = float(link["text"])
        except ValueError:
            chance_weight = 1.0

        new_transition = Transition(target_node["text"], chance_weight)

        start_node_index = int(link["nodeA"])
        states[start_node_index].add_transition(new_transition)

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
        "bCanBeStartState": state.is_start_state
    })

target_file = open(TARGET_FILE_NAME, "w")
target_file.write(json.dumps(payload))
target_file.close()

print(f"Success! The created file {TARGET_FILE_NAME} is ready to be imported into Unreal!")