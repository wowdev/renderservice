from idautils import *
from functools import *
from ida_name import *
import pickle
import re

names = {}

script_dir = os.path.dirname(os.path.realpath(__file__))

for position, name in Names():
	names[position] = name

with open(script_dir + "/idb-" + str(ida_nalt.retrieve_input_file_crc32()) + ".pickle", "w") as file:
	pickle.dump (names, file)
print("pickled names")
