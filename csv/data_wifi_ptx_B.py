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

txPowerB1 = [20, 20]


# --------------
# --- static ---
# --------------

txPowerB2 = [20, 20]


# ---------------
# --- dynamic ---
# ---------------

# Columns on .csv file
txPowerB3 = []

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
            if (reader.line_num-6)%1684 == 2 or (((reader.line_num-10)%1684)%112 == 2 and ((reader.line_num-10)%1684)//112 != 14):
                txPowerB3.append(float(row[4]))
#print len(txPowerB3), txPowerB3

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
            if (reader.line_num-6)%1684 == 2 or (((reader.line_num-10)%1684)%112 == 2 and ((reader.line_num-10)%1684)//112 != 14):
                txPowerB3.append(float(row[4]))
#print len(txPowerB3), txPowerB3

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
            if (reader.line_num-6)%1684 == 2 or (((reader.line_num-10)%1684)%112 == 2 and ((reader.line_num-10)%1684)//112 != 14):
                txPowerB3.append(float(row[4]))
#print len(txPowerB3), txPowerB3


# --------------------------
# --- 1 channel mobility ---
# --------------------------

txPowerB1m = [20, 20]


# -----------------------
# --- static mobility ---
# -----------------------

txPowerB2m = [20, 20]


# ------------------------
# --- dynamic mobility ---
# ------------------------

# Columns on .csv file
txPowerB3m = []

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
            if (reader.line_num-6)%1684 == 2 or (((reader.line_num-10)%1684)%112 == 2 and ((reader.line_num-10)%1684)//112 != 14):
                txPowerB3m.append(float(row[4]))
#print len(txPowerB3m), txPowerB3m

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
            if (reader.line_num-6)%1684 == 2 or (((reader.line_num-10)%1684)%112 == 2 and ((reader.line_num-10)%1684)//112 != 14):
                txPowerB3m.append(float(row[4]))
#print len(txPowerB3m), txPowerB3m

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
            if (reader.line_num-6)%1684 == 2 or (((reader.line_num-10)%1684)%112 == 2 and ((reader.line_num-10)%1684)//112 != 14):
                txPowerB3m.append(float(row[4]))
#print len(txPowerB3m), txPowerB3m


data_a = [txPowerB1, txPowerB2, txPowerB3]
data_b = [txPowerB1m, txPowerB2m, txPowerB3m]

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
plt.ylim(-1, 21)
#plt.tight_layout()
#plt.title('Transmission Power Slice B [dBm]')
plt.savefig('figPtxB.pdf', bbox_inches='tight')