# 5G Network Slicing for Wi-Fi Networks

## Table of Contents
- [Authors](#authors)
- [Introduction](#introduction)
- [Description](#description)
- [Usage](#usage)
- [Contribute](#contribute)

## Authors

* [Matteo Nerini](https://www.imperial.ac.uk/people/m.nerini20)
* [David Palma](https://www.ntnu.edu/employees/david.palma)

## Introduction

The aim of this project is to provide a general framework to develop and test network slicing techniques for Wi-Fi networks. A Wi-Fi (IEEE 802.11ax) indoor scenario. Multiple Stations (STAs) are connected to one Access Point (AP). A resource allocation algorithm can be tested in three steps:
* Configure the environment (number of Wi-Fi STAs, required throughput in each slice,...) through the interface script.
* Write in c++ a dynamic algorithm and define: the channel bandwidth, channel number, MCS, gi and tx power. The algorithm have access to run-time Key Performance Indicators (KPIs) of the network to take smart decision. There is no limit on the kind of algorithm which can be implemented, also Machine Learning (ML) techniques are supported.
* Finally, when the simulation time is over, the performance (throughput, losses and latency) is reported in a .csv file.

<p align="center"> 
<img src="https://github.com/matteonerini/5g-network-slicing-for-wifi-networks/blob/main/figs/scenario.png" width="50%">
</p>

If you use these codes or any modified part of them, please cite our paper: M. Nerini, and D. Palma "5G Network Slicing for Wi-Fi Networks".

```
@INPROCEEDINGS{ner20,
    AUTHOR="Matteo Nerini and David Palma",
    TITLE="5G Network Slicing for Wi-Fi Networks",
    BOOKTITLE="IM 2021 () ",
    ADDRESS="",
    DAYS="17-21",
    MONTH="may",
    YEAR="2021",
    KEYWORDS="Network Slicing ; Wireless and Mobile Networks; Simulation",
    URL="http://XXXXX/211412.pdf"
}
```

## Description

This repository contains *ns-3* scripts which simulate the considered Wi-Fi scenario.

The project is structured as follows:
* ```Folder1```: contains ...
* ```Folder2```: contains ... 
* ```Folder3```: contains the documentation related to ...

The code to run simulations is organized as follows:

* ```folder1orfile1```: ...

## Usage

Follow the following steps to be able to use our codes:
* Install the *ns-3* network simulator following the instructions from its main website [nsnam.org](https://www.nsnam.org/).
* Download this repository as a .zip and extract it.
* Paste its content inside the ```scratch``` directory, which is inside the *ns-3* release directory.

Alternatively, you can download only the specific files you want to use and paste them in the ```scratch``` directory.

To launch a python script, for example ```run_wifi.py```, go in the ```scratch``` directory and type:

```
python ./run_wifi.py
```

### Input files

There is an input file that is required:

* ```main.py```: define parameters.

### Other installations

```
install ns-3
```

## Contribute

Contributions are very welcome! If you want to contribute, or report a bug, please contact Matteo at m.nerini20 [at] imperial.ac.uk.
