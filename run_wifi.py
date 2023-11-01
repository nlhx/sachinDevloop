#####################################
# Author: Matteo Nerini             #
# Email:  m.nerini20@imperial.ac.uk #
# Date:   June 2020                 #
#####################################

# Import
from datetime import datetime
import subprocess
import csv

# Options
num_scenarios = 1           # how many scenarios are simulated
num_seeds = 20              # how many times each scenario is simulated
notes = "6-100-2"           # notes to be written in the .csv file
ns3_script = "wifi"         # ns3 script to be launched

timestamp = datetime.now().strftime('%Y-%m-%d_%H:%M:%S')
filebase = "/testfile_" + timestamp
csv_file_py = "." + filebase + '.csv'
csv_file_ns3 = "./scratch" + filebase + '.csv'


def main():
    print "Creating file ", csv_file_py
    print "Simulating", num_scenarios, "scenario(s) with", num_seeds, "seed(s) each"
    print
    arg_list = []
    for i in range(num_scenarios):
        for j in range(num_seeds):
            args = {}
            args["simulationTime"] = 15
            args["seed"] = j + 1
            args["csvFileName"] = csv_file_ns3
            args["band"] = "AX_5"
            args["phyModel"] = "spectrum"
            args["constantMcs"] = 1
            args["channelNumberA"] = 42 # Network A
            args["channelWidthA"] = 20
            args["mcs"] = 0
            args["giA"] = 800
            args["txPowerA"] = 2
            #args["dataRateA"] = "2Mb/s"
            args["channelNumberB"] = 114 # Network B
            args["channelWidthB"] = 20
            #args["mcsB"] = 5
            args["giB"] = 1600
            args["txPowerB"] = -4
            #args["dataRateB"] = "10Kb/s"
            args["channelNumberC"] = 155 # Network C
            args["channelWidthC"] = 40
            #args["mcsC"] = 5
            args["giC"] = 800
            args["txPowerC"] = 18
            #args["dataRateC"] = "1Mb/s"
            args["nStaA"] = 1
            args["nStaB"] = 1
            args["nStaC"] = 1
            arg_list.append(args)
    with open(csv_file_py, 'w') as file:
        writer = csv.writer(file)
        writer.writerow(["scenarios", num_scenarios])
        writer.writerow(["seeds per scenario", num_seeds])
        writer.writerow(["notes", ns3ScriptName + ". " + notes])
        writer.writerow(["arg_list[0]", arg_list[0]])
        writer.writerow(["arg_list[" + str(len(arg_list)-1) + "]", arg_list[len(arg_list)-1]])
    for i in range(0, len(arg_list)):
        simulate(arg_list[i])
        print


def get_arguments(arg):
    atribute_names = [
    	" --simulationTime=",
    	" --seed=",
    	" --csvFileName=",
    	" --band=",
    	" --phyModel=",
        " --constantMcs=",
    	" --channelNumberA=", # Network A
        " --channelWidthA=",
        " --mcs=",
        " --giA=",
        " --txPowerA=",
        #" --dataRateA=",
        " --channelNumberB=", # Network B
        " --channelWidthB=",
        #" --mcsB=",
        " --giB=",
        " --txPowerB=",
        #" --dataRateB=",
        " --channelNumberC=", # Network C
        " --channelWidthC=",
        #" --mcsC=",
        " --giC=",
        " --txPowerC=",
        #" --dataRateC=",
        " --nStaA=",
        " --nStaB=",
        " --nStaC="
        ]
    atribute_values = [
    	str(arg["simulationTime"]),
    	str(arg["seed"]),
    	str(arg["csvFileName"]),
    	str(arg["band"]),
    	str(arg["phyModel"]),
        str(arg["constantMcs"]),
    	str(arg["channelNumberA"]), # Network A
    	str(arg["channelWidthA"]),
        str(arg["mcs"]),
        str(arg["giA"]),
        str(arg["txPowerA"]),
        #str(arg["dataRateA"]),
        str(arg["channelNumberB"]), # Network B
        str(arg["channelWidthB"]),
        #str(arg["mcsB"]),
        str(arg["giB"]),
        str(arg["txPowerB"]),
        #str(arg["dataRateB"]),
        str(arg["channelNumberC"]), # Network C
        str(arg["channelWidthC"]),
        #str(arg["mcsC"]),
        str(arg["giC"]),
        str(arg["txPowerC"]),
        #str(arg["dataRateC"]),
        str(arg["nStaA"]),
        str(arg["nStaB"]),
        str(arg["nStaC"])
        ]
    arguments = ""
    for s in range(0, len(atribute_names)):
        arguments += atribute_names[s]
        arguments += atribute_values[s]
    return arguments


def simulate(arg):
    arguments = get_arguments(arg)
    print "Calling the ns3 script '" + ns3_script + ".cc'"
    subprocess.call('(cd ..; ./waf --run "' + ns3_script + arguments+ '")', shell=True)


if __name__ == "__main__":
    main()