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


# -----------------
# --- 1 channel ---
# -----------------

# Columns on .csv file
rxPackets11 = []
rxPacketsSum11 = []
rxPackets12 = []
rxPacketsSum12 = []
rxPackets13 = []
rxPacketsSum13 = []

thSum1 = []

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
        	rxPackets11.append(float(row[4]))
#print len(rxPackets1), rxPackets1
rxPacketsSum11 = np.zeros((num_seeds))
for i in range(len(rxPackets11)):
    rxPacketsSum11[i//(len(rxPackets11)/num_seeds)] += rxPackets11[i]
#print len(rxPacketsSum11), rxPacketsSum11

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
            rxPackets12.append(float(row[4]))
#print len(rxPackets1), rxPackets1
rxPacketsSum12 = np.zeros((num_seeds))
for i in range(len(rxPackets12)):
    rxPacketsSum12[i//(len(rxPackets12)/num_seeds)] += rxPackets12[i]
#print len(rxPacketsSum12), rxPacketsSum12

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
            rxPackets13.append(float(row[4]))
#print len(rxPackets1), rxPackets1
rxPacketsSum13 = np.zeros((num_seeds))
for i in range(len(rxPackets13)):
    rxPacketsSum13[i//(len(rxPackets13)/num_seeds)] += rxPackets13[i]
#print len(rxPacketsSum13), rxPacketsSum13

# Throughput [Mbit/s]
thSum1 = np.concatenate([rxPacketsSum11, rxPacketsSum12, rxPacketsSum13]) * 1472 * 8 / (simulationTime * 1000000.0);
#print len(thSum1), thSum1

# Bandwidth [MHz]
bSum1 = 160

# Spectral Efficiency: 1*160MHz is utilized
specEff1 = thSum1/bSum1
#print len(specEff1), specEff1


# --------------
# --- static ---
# --------------

# Columns on .csv file
rxPackets21 = []
rxPacketsSum21 = []
b21 = []
bSum21 = []
rxPackets22 = []
rxPacketsSum22 = []
b22 = []
bSum22 = []
rxPackets23 = []
rxPacketsSum23 = []
b23 = []
bSum23 = []

thSum2 = []
bSum2 = []

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
            if (reader.line_num-5)%110 != 1 and (reader.line_num-5)%110 != 2:
                rxPackets21.append(float(row[4]))
#print len(rxPackets2), rxPackets2
rxPacketsSum21 = np.zeros((num_seeds))
for i in range(len(rxPackets21)):
    rxPacketsSum21[i//(len(rxPackets21)/num_seeds)] += rxPackets21[i]
#print len(rxPacketsSum21), rxPacketsSum21

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
            if (reader.line_num-5)%110 != 1 and (reader.line_num-5)%110 != 2:
                rxPackets22.append(float(row[4]))
#print len(rxPackets2), rxPackets2
rxPacketsSum22 = np.zeros((num_seeds))
for i in range(len(rxPackets22)):
    rxPacketsSum22[i//(len(rxPackets22)/num_seeds)] += rxPackets22[i]
#print len(rxPacketsSum22), rxPacketsSum22

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
            if (reader.line_num-5)%110 != 1 and (reader.line_num-5)%110 != 2:
                rxPackets23.append(float(row[4]))
#print len(rxPackets2), rxPackets2
rxPacketsSum23 = np.zeros((num_seeds))
for i in range(len(rxPackets23)):
    rxPacketsSum23[i//(len(rxPackets23)/num_seeds)] += rxPackets23[i]
#print len(rxPacketsSum23), rxPacketsSum23

# Throughput [Mbit/s]
thSum2 = np.concatenate([rxPacketsSum21, rxPacketsSum22, rxPacketsSum23]) * 1472 * 8 / (simulationTime * 1000000.0);
#print len(thSum2), thSum2

# Bandwidth [MHz] 10 elements with average bandwidth utilized
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
            if (reader.line_num-5)%110 == 2:
                b21.append(float(row[1]))
                b21.append(float(row[3]))
                b21.append(float(row[5]))
#print len(b2), b2
bSum21 = np.zeros((num_seeds))
for i in range(len(b21)):
    bSum21[i//(len(b21)/num_seeds)] += b21[i]
#print len(bSum21), bSum21

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
            if (reader.line_num-5)%110 == 2:
                b22.append(float(row[1]))
                b22.append(float(row[3]))
                b22.append(float(row[5]))
#print len(b2), b2
bSum22 = np.zeros((num_seeds))
for i in range(len(b22)):
    bSum22[i//(len(b22)/num_seeds)] += b22[i]
#print len(bSum22), bSum22

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
            if (reader.line_num-5)%110 == 2:
                b23.append(float(row[1]))
                b23.append(float(row[3]))
                b23.append(float(row[5]))
#print len(b2), b2
bSum23 = np.zeros((num_seeds))
for i in range(len(b23)):
    bSum23[i//(len(b23)/num_seeds)] += b23[i]
#print len(bSum23), bSum23

bSum2 = np.concatenate([bSum21, bSum22, bSum23])
#print len(bSum2), bSum2

# Spectral Efficiency
specEff2 = thSum2/bSum2
#print len(specEff2), specEff2


# ---------------
# --- dynamic ---
# ---------------

# Columns on .csv file
rxPackets31 = []
rxPacketsSum31 = []
b31 = []
bSum31 = []
rxPackets32 = []
rxPacketsSum32 = []
b32 = []
bSum32 = []
rxPackets33 = []
rxPacketsSum33 = []
b33 = []
bSum33 = []

thSum3 = []
bSum3 = []

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
            if (reader.line_num-6)%1684 > 1575:
                rxPackets31.append(float(row[4]))
#print len(rxPackets3), rxPackets3
rxPacketsSum31 = np.zeros((num_seeds))
for i in range(len(rxPackets31)):
	rxPacketsSum31[i//(len(rxPackets31)/num_seeds)] += rxPackets31[i]
#print len(rxPacketsSum31), rxPacketsSum31

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
            if (reader.line_num-6)%1684 > 1575:
                rxPackets32.append(float(row[4]))
#print len(rxPackets3), rxPackets3
rxPacketsSum32 = np.zeros((num_seeds))
for i in range(len(rxPackets32)):
    rxPacketsSum32[i//(len(rxPackets32)/num_seeds)] += rxPackets32[i]
#print len(rxPacketsSum32), rxPacketsSum32

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
            if (reader.line_num-6)%1684 > 1575:
                rxPackets33.append(float(row[4]))
#print len(rxPackets3), rxPackets3
rxPacketsSum33 = np.zeros((num_seeds))
for i in range(len(rxPackets33)):
    rxPacketsSum33[i//(len(rxPackets33)/num_seeds)] += rxPackets33[i]
#print len(rxPacketsSum33), rxPacketsSum33

# Throughput [Mbit/s]
thSum3 = np.concatenate([rxPacketsSum31, rxPacketsSum32, rxPacketsSum33]) * 1472 * 8 / (simulationTime * 1000000.0);
#print len(thSum3), thSum3

# Bandwidth [MHz] 10 elements with average bandwidth utilized
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
            if 1 <= (reader.line_num-6)%1684 <= 3 or (1 <= ((reader.line_num-10)%1684)%112 <= 3 and ((reader.line_num-10)%1684)//112 != 14):
                b31.append(float(row[1]))
#print len(b3), b3
bSum31 = np.zeros((num_seeds))
for i in range(len(b31)):
	bSum31[i//(len(b31)/num_seeds)] += b31[i]
#print len(bSum31), bSum31

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
            if 1 <= (reader.line_num-6)%1684 <= 3 or (1 <= ((reader.line_num-10)%1684)%112 <= 3 and ((reader.line_num-10)%1684)//112 != 14):
                b32.append(float(row[1]))
#print len(b3), b3
bSum32 = np.zeros((num_seeds))
for i in range(len(b32)):
    bSum32[i//(len(b32)/num_seeds)] += b32[i]
#print len(bSum32), bSum32

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
            if 1 <= (reader.line_num-6)%1684 <= 3 or (1 <= ((reader.line_num-10)%1684)%112 <= 3 and ((reader.line_num-10)%1684)//112 != 14):
                b33.append(float(row[1]))
#print len(b3), b3
bSum33 = np.zeros((num_seeds))
for i in range(len(b33)):
    bSum33[i//(len(b33)/num_seeds)] += b33[i]
#print len(bSum33), bSum33

bSum3 = np.concatenate([bSum31, bSum32, bSum33])/simulationTime
#print len(bSum3), bSum3

# Spectral Efficiency
specEff3 = thSum3/bSum3
#print len(specEff3), specEff3


# --------------------------
# --- 1 channel mobility ---
# --------------------------

# Columns on .csv file
rxPackets11 = []
rxPacketsSum11 = []
rxPackets12 = []
rxPacketsSum12 = []
rxPackets13 = []
rxPacketsSum13 = []

thSum1 = []

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
        	rxPackets11.append(float(row[4]))
#print len(rxPackets1), rxPackets1
rxPacketsSum11 = np.zeros((num_seeds))
for i in range(len(rxPackets11)):
    rxPacketsSum11[i//(len(rxPackets11)/num_seeds)] += rxPackets11[i]
#print len(rxPacketsSum11), rxPacketsSum11

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
            rxPackets12.append(float(row[4]))
#print len(rxPackets1), rxPackets1
rxPacketsSum12 = np.zeros((num_seeds))
for i in range(len(rxPackets12)):
    rxPacketsSum12[i//(len(rxPackets12)/num_seeds)] += rxPackets12[i]
#print len(rxPacketsSum12), rxPacketsSum12

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
            rxPackets13.append(float(row[4]))
#print len(rxPackets1), rxPackets1
rxPacketsSum13 = np.zeros((num_seeds))
for i in range(len(rxPackets13)):
    rxPacketsSum13[i//(len(rxPackets13)/num_seeds)] += rxPackets13[i]
#print len(rxPacketsSum13), rxPacketsSum13

# Throughput [Mbit/s]
thSum1 = np.concatenate([rxPacketsSum11, rxPacketsSum12, rxPacketsSum13]) * 1472 * 8 / (simulationTime * 1000000.0);
#print len(thSum1), thSum1

# Bandwidth [MHz]
bSum1 = 160

# Spectral Efficiency: 1*160MHz is utilized
specEff1m = thSum1/bSum1
#print len(specEff1), specEff1


# -----------------------
# --- static mobility ---
# -----------------------

# Columns on .csv file
rxPackets21 = []
rxPacketsSum21 = []
b21 = []
bSum21 = []
rxPackets22 = []
rxPacketsSum22 = []
b22 = []
bSum22 = []
rxPackets23 = []
rxPacketsSum23 = []
b23 = []
bSum23 = []

thSum2 = []
bSum2 = []

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
            if (reader.line_num-5)%110 != 1 and (reader.line_num-5)%110 != 2:
                rxPackets21.append(float(row[4]))
#print len(rxPackets2), rxPackets2
rxPacketsSum21 = np.zeros((num_seeds))
for i in range(len(rxPackets21)):
    rxPacketsSum21[i//(len(rxPackets21)/num_seeds)] += rxPackets21[i]
#print len(rxPacketsSum21), rxPacketsSum21

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
            if (reader.line_num-5)%110 != 1 and (reader.line_num-5)%110 != 2:
                rxPackets22.append(float(row[4]))
#print len(rxPackets2), rxPackets2
rxPacketsSum22 = np.zeros((num_seeds))
for i in range(len(rxPackets22)):
    rxPacketsSum22[i//(len(rxPackets22)/num_seeds)] += rxPackets22[i]
#print len(rxPacketsSum22), rxPacketsSum22

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
            if (reader.line_num-5)%110 != 1 and (reader.line_num-5)%110 != 2:
                rxPackets23.append(float(row[4]))
#print len(rxPackets2), rxPackets2
rxPacketsSum23 = np.zeros((num_seeds))
for i in range(len(rxPackets23)):
    rxPacketsSum23[i//(len(rxPackets23)/num_seeds)] += rxPackets23[i]
#print len(rxPacketsSum23), rxPacketsSum23

# Throughput [Mbit/s]
thSum2 = np.concatenate([rxPacketsSum21, rxPacketsSum22, rxPacketsSum23]) * 1472 * 8 / (simulationTime * 1000000.0);
#print len(thSum2), thSum2

# Bandwidth [MHz] 10 elements with average bandwidth utilized
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
            if (reader.line_num-5)%110 == 2:
                b21.append(float(row[1]))
                b21.append(float(row[3]))
                b21.append(float(row[5]))
#print len(b2), b2
bSum21 = np.zeros((num_seeds))
for i in range(len(b21)):
    bSum21[i//(len(b21)/num_seeds)] += b21[i]
#print len(bSum21), bSum21

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
            if (reader.line_num-5)%110 == 2:
                b22.append(float(row[1]))
                b22.append(float(row[3]))
                b22.append(float(row[5]))
#print len(b2), b2
bSum22 = np.zeros((num_seeds))
for i in range(len(b22)):
    bSum22[i//(len(b22)/num_seeds)] += b22[i]
#print len(bSum22), bSum22

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
            if (reader.line_num-5)%110 == 2:
                b23.append(float(row[1]))
                b23.append(float(row[3]))
                b23.append(float(row[5]))
#print len(b2), b2
bSum23 = np.zeros((num_seeds))
for i in range(len(b23)):
    bSum23[i//(len(b23)/num_seeds)] += b23[i]
#print len(bSum23), bSum23

bSum2 = np.concatenate([bSum21, bSum22, bSum23])
#print len(bSum2), bSum2

# Spectral Efficiency
specEff2m = thSum2/bSum2
#print len(specEff2), specEff2


# ------------------------
# --- dynamic mobility ---
# ------------------------

# Columns on .csv file
rxPackets31 = []
rxPacketsSum31 = []
b31 = []
bSum31 = []
rxPackets32 = []
rxPacketsSum32 = []
b32 = []
bSum32 = []
rxPackets33 = []
rxPacketsSum33 = []
b33 = []
bSum33 = []

thSum3 = []
bSum3 = []

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
            if (reader.line_num-6)%1684 > 1575:
                rxPackets31.append(float(row[4]))
#print len(rxPackets3), rxPackets3
rxPacketsSum31 = np.zeros((num_seeds))
for i in range(len(rxPackets31)):
	rxPacketsSum31[i//(len(rxPackets31)/num_seeds)] += rxPackets31[i]
#print len(rxPacketsSum31), rxPacketsSum31

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
            if (reader.line_num-6)%1684 > 1575:
                rxPackets32.append(float(row[4]))
#print len(rxPackets3), rxPackets3
rxPacketsSum32 = np.zeros((num_seeds))
for i in range(len(rxPackets32)):
    rxPacketsSum32[i//(len(rxPackets32)/num_seeds)] += rxPackets32[i]
#print len(rxPacketsSum32), rxPacketsSum32

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
            if (reader.line_num-6)%1684 > 1575:
                rxPackets33.append(float(row[4]))
#print len(rxPackets3), rxPackets3
rxPacketsSum33 = np.zeros((num_seeds))
for i in range(len(rxPackets33)):
    rxPacketsSum33[i//(len(rxPackets33)/num_seeds)] += rxPackets33[i]
#print len(rxPacketsSum33), rxPacketsSum33

# Throughput [Mbit/s]
thSum3 = np.concatenate([rxPacketsSum31, rxPacketsSum32, rxPacketsSum33]) * 1472 * 8 / (simulationTime * 1000000.0);
#print len(thSum3), thSum3

# Bandwidth [MHz] 10 elements with average bandwidth utilized
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
            if 1 <= (reader.line_num-6)%1684 <= 3 or (1 <= ((reader.line_num-10)%1684)%112 <= 3 and ((reader.line_num-10)%1684)//112 != 14):
                b31.append(float(row[1]))
#print len(b3), b3
bSum31 = np.zeros((num_seeds))
for i in range(len(b31)):
	bSum31[i//(len(b31)/num_seeds)] += b31[i]
#print len(bSum31), bSum31

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
            if 1 <= (reader.line_num-6)%1684 <= 3 or (1 <= ((reader.line_num-10)%1684)%112 <= 3 and ((reader.line_num-10)%1684)//112 != 14):
                b32.append(float(row[1]))
#print len(b3), b3
bSum32 = np.zeros((num_seeds))
for i in range(len(b32)):
    bSum32[i//(len(b32)/num_seeds)] += b32[i]
#print len(bSum32), bSum32

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
            if 1 <= (reader.line_num-6)%1684 <= 3 or (1 <= ((reader.line_num-10)%1684)%112 <= 3 and ((reader.line_num-10)%1684)//112 != 14):
                b33.append(float(row[1]))
#print len(b3), b3
bSum33 = np.zeros((num_seeds))
for i in range(len(b33)):
    bSum33[i//(len(b33)/num_seeds)] += b33[i]
#print len(bSum33), bSum33

bSum3 = np.concatenate([bSum31, bSum32, bSum33])/simulationTime
#print len(bSum3), bSum3

# Spectral Efficiency
specEff3m = thSum3/bSum3
#print len(specEff3), specEff3


data_a = [specEff1, specEff2, specEff3]
data_b = [specEff1m, specEff2m, specEff3m]

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
plt.ylim(1.34, 2.66)
#plt.tight_layout()
#plt.title('Spectrum Efficiency [bit/s/Hz]')
plt.savefig('figSe.pdf', bbox_inches='tight')