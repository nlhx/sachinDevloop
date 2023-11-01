#####################################
# Author: Matteo Nerini             #
# Email:  m.nerini20@imperial.ac.uk #
# Date:   June 2020                 #
#####################################

# Import
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import csv

headerLines = 5 # Lines to skip
simulationTime = 15

sliceN = 'C'

sliceA = sliceN == 'A'
sliceB = sliceN == 'B'
sliceC = sliceN == 'C'


# -----------------
# --- 1 channel ---
# -----------------

# Columns on .csv file
rxPackets1 = []
txPackets1 =[]
latency1 = []
pe1 = []

with open('2-100-6_1ch_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1 <= (reader.line_num-5)%108 <= 2:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))
            if sliceB and 3 <= (reader.line_num-5)%108 <= 102:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))
            if sliceC and (103 <= (reader.line_num-5)%108 <= 108 or (reader.line_num-5)%108 == 0):
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))

with open('4-100-4_1ch_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1 <= (reader.line_num-5)%108 <= 4:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))
            if sliceB and 5 <= (reader.line_num-5)%108 <= 104:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))
            if sliceC and (105 <= (reader.line_num-5)%108 <= 108 or (reader.line_num-5)%108 == 0):
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))

with open('6-100-2_1ch_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1 <= (reader.line_num-5)%108 <= 6:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))
            if sliceB and 7 <= (reader.line_num-5)%108 <= 106:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))
            if sliceC and (107 <= (reader.line_num-5)%108 <= 108 or (reader.line_num-5)%108 == 0):
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1.append(float(row[5]))

#print len(rxPackets1), rxPackets1
#print len(latency1), latency1 

pe1 = np.zeros(len(txPackets1))
for i in range(len(txPackets1)):
    pe1[i] = (txPackets1[i] - rxPackets1[i]) / txPackets1[i]
#print len(pe1), pe1


# --------------
# --- static ---
# --------------

# Columns on .csv file
rxPackets2 = []
txPackets2 = []
latency2 = []
pe2 = []

with open('2-100-6_static_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 3 <= (reader.line_num-5)%110 <= 4:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))
            if sliceB and 5 <= (reader.line_num-5)%110 <= 104:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))
            if sliceC and (105 <= (reader.line_num-5)%110 <= 110 or (reader.line_num-5)%110 == 0):
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))

with open('4-100-4_static_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 3 <= (reader.line_num-5)%110 <= 6:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))
            if sliceB and 7 <= (reader.line_num-5)%110 <= 106:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))
            if sliceC and (107 <= (reader.line_num-5)%110 <= 110 or (reader.line_num-5)%110 == 0):
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))

with open('6-100-2_static_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 3 <= (reader.line_num-5)%110 <= 8:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))
            if sliceB and 9 <= (reader.line_num-5)%110 <= 108:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))
            if sliceC and (109 <= (reader.line_num-5)%110 <= 110 or (reader.line_num-5)%110 == 0):
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2.append(float(row[5]))

#print len(rxPackets2), rxPackets2
#print len(latency2), latency2

pe2 = np.zeros(len(txPackets2))
for i in range(len(txPackets2)):
    pe2[i] = (txPackets2[i] - rxPackets2[i]) / txPackets2[i]
#print len(pe2), pe2


# ---------------
# --- dynamic ---
# ---------------

# Columns on .csv file
rxPackets3 = []
txPackets3 = []
latency3 = []
pe3 = []

with open('2-100-6_dynamic_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1576 <= (reader.line_num-6)%1684 <= 1577:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))
            if sliceB and 1578 <= (reader.line_num-6)%1684 <= 1677:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))
            if sliceC and 1678 <= (reader.line_num-6)%1684 <= 1683:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))

with open('4-100-4_dynamic_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1576 <= (reader.line_num-6)%1684 <= 1579:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))
            if sliceB and 1580 <= (reader.line_num-6)%1684 <= 1679:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))
            if sliceC and 1680 <= (reader.line_num-6)%1684 <= 1683:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))

with open('6-100-2_dynamic_no_mob.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1576 <= (reader.line_num-6)%1684 <= 1581:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))
            if sliceB and 1582 <= (reader.line_num-6)%1684 <= 1681:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))
            if sliceC and 1682 <= (reader.line_num-6)%1684 <= 1683:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3.append(float(row[5]))

#print len(rxPackets3), rxPackets3
#print len(latency3), latency3

pe3 = np.zeros(len(txPackets3))
for i in range(len(txPackets3)):
    pe3[i] = (txPackets3[i] - rxPackets3[i]) / txPackets3[i]
#print len(pe3), pe3


# --------------------------
# --- 1 channel mobility ---
# --------------------------

# Columns on .csv file
rxPackets1 = []
txPackets1 =[]
latency1m = []
pe1m = []

with open('2-100-6_1ch.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
    	# Read num_scenarios from first line
    	if reader.line_num == 1:
    		num_scenarios = int(row[1])
    	# Read num_seeds from second line
    	elif reader.line_num == 2:
    		num_seeds = int(row[1])
    	# Skip header lines and fill the variables
    	elif reader.line_num > headerLines:
            if sliceA and 1 <= (reader.line_num-5)%108 <= 2:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))
            if sliceB and 3 <= (reader.line_num-5)%108 <= 102:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))
            if sliceC and (103 <= (reader.line_num-5)%108 <= 108 or (reader.line_num-5)%108 == 0):
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))

with open('4-100-4_1ch.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1 <= (reader.line_num-5)%108 <= 4:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))
            if sliceB and 5 <= (reader.line_num-5)%108 <= 104:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))
            if sliceC and (105 <= (reader.line_num-5)%108 <= 108 or (reader.line_num-5)%108 == 0):
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))

with open('6-100-2_1ch.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1 <= (reader.line_num-5)%108 <= 6:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))
            if sliceB and 7 <= (reader.line_num-5)%108 <= 106:
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))
            if sliceC and (107 <= (reader.line_num-5)%108 <= 108 or (reader.line_num-5)%108 == 0):
                txPackets1.append(float(row[3]))
                rxPackets1.append(float(row[4]))
                if float(row[4]) != 0:
                    latency1m.append(float(row[5]))

#print len(rxPackets1), rxPackets1
#print len(latency1), latency1 

pe1m = np.zeros(len(txPackets1))
for i in range(len(txPackets1)):
    pe1m[i] = (txPackets1[i] - rxPackets1[i]) / txPackets1[i]
#print len(pe1), pe1


# -----------------------
# --- static mobility ---
# -----------------------

# Columns on .csv file
rxPackets2 = []
txPackets2 = []
latency2m = []
pe2m = []

with open('2-100-6_static.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 3 <= (reader.line_num-5)%110 <= 4:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))
            if sliceB and 5 <= (reader.line_num-5)%110 <= 104:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))
            if sliceC and (105 <= (reader.line_num-5)%110 <= 110 or (reader.line_num-5)%110 == 0):
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))

with open('4-100-4_static.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 3 <= (reader.line_num-5)%110 <= 6:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))
            if sliceB and 7 <= (reader.line_num-5)%110 <= 106:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))
            if sliceC and (107 <= (reader.line_num-5)%110 <= 110 or (reader.line_num-5)%110 == 0):
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))

with open('6-100-2_static.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 3 <= (reader.line_num-5)%110 <= 8:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))
            if sliceB and 9 <= (reader.line_num-5)%110 <= 108:
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))
            if sliceC and (109 <= (reader.line_num-5)%110 <= 110 or (reader.line_num-5)%110 == 0):
                txPackets2.append(float(row[3]))
                rxPackets2.append(float(row[4]))
                if float(row[4]) != 0:
                    latency2m.append(float(row[5]))

#print len(rxPackets2), rxPackets2
#print len(latency2), latency2

pe2m = np.zeros(len(txPackets2))
for i in range(len(txPackets2)):
    pe2m[i] = (txPackets2[i] - rxPackets2[i]) / txPackets2[i]
#print len(pe2), pe2


# ------------------------
# --- dynamic mobility ---
# ------------------------

# Columns on .csv file
rxPackets3 = []
txPackets3 = []
latency3m = []
pe3m = []

with open('2-100-6_dynamic.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1576 <= (reader.line_num-6)%1684 <= 1577:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))
            if sliceB and 1578 <= (reader.line_num-6)%1684 <= 1677:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))
            if sliceC and 1678 <= (reader.line_num-6)%1684 <= 1683:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))

with open('4-100-4_dynamic.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1576 <= (reader.line_num-6)%1684 <= 1579:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))
            if sliceB and 1580 <= (reader.line_num-6)%1684 <= 1679:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))
            if sliceC and 1680 <= (reader.line_num-6)%1684 <= 1683:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))

with open('6-100-2_dynamic.csv','r') as file:
    reader = csv.reader(file, delimiter=',')
    for row in reader:
        # Read num_scenarios from first line
        if reader.line_num == 1:
            num_scenarios = int(row[1])
        # Read num_seeds from second line
        elif reader.line_num == 2:
            num_seeds = int(row[1])
        # Skip header lines and fill the variables
        elif reader.line_num > headerLines:
            if sliceA and 1576 <= (reader.line_num-6)%1684 <= 1581:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))
            if sliceB and 1582 <= (reader.line_num-6)%1684 <= 1681:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))
            if sliceC and 1682 <= (reader.line_num-6)%1684 <= 1683:
                txPackets3.append(float(row[3]))
                rxPackets3.append(float(row[4]))
                if float(row[4]) != 0:
                    latency3m.append(float(row[5]))

#print len(rxPackets3), rxPackets3
#print len(latency3), latency3

pe3m = np.zeros(len(txPackets3))
for i in range(len(txPackets3)):
    pe3m[i] = (txPackets3[i] - rxPackets3[i]) / txPackets3[i]
#print len(pe3), pe3


data_a = [pe1, pe2, pe3]
data_b = [pe1m, pe2m, pe3m]

ticks = ['Single\nchannel', 'Static\nslicing', 'Dynamic\nslicing']

def set_box_color(bp, color, linestyle):
    plt.setp(bp['boxes'], color=color, linestyle=linestyle)
    plt.setp(bp['whiskers'], color=color, linestyle=linestyle)
    plt.setp(bp['caps'], color=color, linestyle=linestyle)
    plt.setp(bp['medians'], color='#feb24c')

plt.figure()
mpl.rcParams['pdf.fonttype'] = 42
mpl.rcParams['ps.fonttype'] = 42
plt.rcParams.update({'font.family':'Arial', 'font.size': 16})

bpl = plt.boxplot(data_a, positions=np.array(xrange(len(data_a)))*2.0-0.4, sym='', widths=0.6)
bpr = plt.boxplot(data_b, positions=np.array(xrange(len(data_b)))*2.0+0.4, sym='', widths=0.6)
set_box_color(bpl, '#D7191C', '--') # colors are from http://colorbrewer2.org/
set_box_color(bpr, '#2C7BB6', '-')

# draw temporary red and blue lines and use them to create a legend
plt.plot([], c='#D7191C', linestyle='--', label='No mobility')
plt.plot([], c='#2C7BB6', label='Mobility')
plt.legend()

plt.xticks(xrange(0, len(ticks) * 2, 2), ticks)
plt.xlim(-1, len(ticks)*2 -1)
plt.ylim(-0.05, 1.05)
#plt.ylim(-0.01, 0.21)
#plt.tight_layout()
plt.title('Packet Error Probability Slice ' + sliceN)
plt.savefig('figPe_' + sliceN + '.pdf', bbox_inches='tight')

data_a = [latency1, latency2, latency3]
data_b = [latency1m, latency2m, latency3m]

ticks = ['Single\nchannel', 'Static\nslicing', 'Dynamic\nslicing']

def set_box_color(bp, color, linestyle):
    plt.setp(bp['boxes'], color=color, linestyle=linestyle)
    plt.setp(bp['whiskers'], color=color, linestyle=linestyle)
    plt.setp(bp['caps'], color=color, linestyle=linestyle)
    plt.setp(bp['medians'], color='#feb24c')

plt.figure()
mpl.rcParams['pdf.fonttype'] = 42
mpl.rcParams['ps.fonttype'] = 42
plt.rcParams.update({'font.family':'Arial', 'font.size': 16})

bpl = plt.boxplot(data_a, positions=np.array(xrange(len(data_a)))*2.0-0.4, sym='', widths=0.6)
bpr = plt.boxplot(data_b, positions=np.array(xrange(len(data_b)))*2.0+0.4, sym='', widths=0.6)
set_box_color(bpl, '#D7191C', '--') # colors are from http://colorbrewer2.org/
set_box_color(bpr, '#2C7BB6', '-')

# draw temporary red and blue lines and use them to create a legend
plt.plot([], c='#D7191C', linestyle='--', label='No mobility')
plt.plot([], c='#2C7BB6', label='Mobility')
plt.legend()

plt.xticks(xrange(0, len(ticks) * 2, 2), ticks)
plt.xlim(-1, len(ticks)*2 -1)
#plt.ylim(-100, 2100)
#plt.ylim(-12.5, 262.5)
plt.ylim(-6, 126)
#plt.tight_layout()
plt.title('Latency Slice ' + sliceN + ' [ms]')
plt.savefig('figLat_' + sliceN + '.pdf', bbox_inches='tight')