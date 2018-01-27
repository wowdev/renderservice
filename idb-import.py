from idautils import *
from functools import *
from ida_name import *
import pickle
import re

names = {}

script_dir = os.path.dirname(os.path.realpath(__file__))

with open(script_dir + "/idb-" + str(ida_nalt.retrieve_input_file_crc32()) + ".pickle", "r") as file:
	names = pickle.load (file)
	
for position, name in names.iteritems():
	idc.MakeName(position, name)
print("unpickled names")
