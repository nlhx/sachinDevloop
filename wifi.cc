/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *
 * Author: Matteo Nerini
 * Email:  m.nerini20@imperial.ac.uk
 * Date:   June 2020
 *
 *
 * Network Topology:
 *
 *  (xA,yA)             (xB,yB)             (xC,yC)
 *     *                   *                   *
 *     |    x nStaA        |    x nStaB        |    x nStaC
 *   STA A                STA B               STA C
 *
 *       (0,0)
 *         *
 *         |     <- 3 APs devices in 1 node
 *        AP
 *
 *
 * Building Topology:
 *
 *     ^  -----------------------------
 *   1 |  |                           |
 *   0 |  |                           |  StaA: Random Walk
 *     |  |             o             |  StaB: Constant Position
 *   m |  |              AP           |  StaC: Random Walk
 *     |  |                           |
 *     v  -----------------------------
 *        <--------------------------->
 *                    20 m
 *
 */

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/wifi-net-device.h"
#include "ns3/qos-txop.h"
#include "ns3/wifi-mac.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/buildings-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include <bits/stdc++.h>

#define ENDC    "\033[0m"
#define ERROR   "\033[91m"
#define OKGREEN	"\033[92m"
#define WARNING	"\033[93m"
#define OKBLUE	"\033[94m"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("wifi_dynamic");


// Options
uint32_t payloadSize = 1472;          // bytes (UDP)
double simulationTime = 15;           // seconds
int seed = 1;                         // seed used in the simulation
std::string csvFileName = "test.csv"; // csv file name
std::string band = "AX_5";            // AC_5, AX_2.4 or AX_5
std::string phyModel = "spectrum";    // "spectrum" or "yans"
bool constantMcs = 1;                 // 0 Minstrel or 1 constant
bool enablePcap = 0;                  // 0 no Pcap or 1 Pcap
double x_max = 20.0;                  // meters
double y_max = 10.0;                  // meters
double z_max = 3.0;                   // meters
const int nStaA = 6;                  // number of stations A
const int nStaB = 100;                // number of stations B
const int nStaC = 2;                  // number of stations C


// Additional options needed to initialize the channel object, do not change them!
// Network A
int channelNumberA = 36;  // Channel number A
int channelWidthA = 20;   // 20, 40, 80 or 160 MHz
int mcsA = 5;             // from 0 to 11 (-1 = unset value)
int giA = 800;            // 800, 1600 or 3200 ns
int txPowerA = 20;        // dBm
std::string dataRateA_fixed = "10Mb/s";
// Network B
int channelNumberB = 100; // Channel number B
int channelWidthB = 20;   // 20, 40, 80 or 160 MHz
int mcsB = 5;             // from 0 to 11 (-1 = unset value)
int giB = 800;           // 800, 1600 or 3200 ns
int txPowerB = 20;         // dBm
std::string dataRateB_fixed = "10Mb/s";
// Network C
int channelNumberC = 153; // Channel number B
int channelWidthC = 20;   // 20, 40, 80 or 160 MHz
int mcsC = 5;             // from 0 to 11 (-1 = unset value)
int giC = 800;            // 800, 1600 or 3200 ns
int txPowerC = 20;        // dBm
std::string dataRateC_fixed = "10Mb/s";


// Utility variables definition
std::vector<int> dataRateA(nStaA);
std::vector<int> dataRateB(nStaB);
std::vector<int> dataRateC(nStaC);

int dataRateSumA = 0;
int dataRateSumB = 0;
int dataRateSumC = 0;

std::vector<uint64_t> rxPacketsA_meas(nStaA);
std::vector<uint64_t> rxPacketsB_meas(nStaB);
std::vector<uint64_t> rxPacketsC_meas(nStaC);

std::vector<double> pathLoss(nStaA+nStaB+nStaC);
std::vector<double> rxPower(nStaA+nStaB+nStaC);

std::vector<double> x(nStaA+nStaB+nStaC);
std::vector<double> y(nStaA+nStaB+nStaC);

Ptr<FlowMonitor> flowMonitor;
FlowMonitorHelper flowHelper;

std::vector<uint32_t> txPackets_unsort(nStaA+nStaB+nStaC);
std::vector<uint32_t> rxPackets_unsort(nStaA+nStaB+nStaC);
std::vector<double> latency_unsort(nStaA+nStaB+nStaC);

uint32_t txPackets[2][nStaA+nStaB+nStaC]; // txPackets until t0 and t0-T
uint32_t rxPackets[2][nStaA+nStaB+nStaC]; // rxPackets until t0 and t0-T
double latency[2][nStaA+nStaB+nStaC];     // average latency until t0 and t0-T

std::vector<uint32_t> destPorts(nStaA+nStaB+nStaC);

double probErr[2][nStaA+nStaB+nStaC]; // Pe on last T=[t0-T, t0] and on previous T=[t0-2T, t0-T]


// function to define the parameters which can be set when the script is called
void configure (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("seed", "Seed", seed);
  cmd.AddValue ("csvFileName", "Name of the .csv file", csvFileName);
  cmd.AddValue ("band", "AC_5, AX_2.4 or AX_5", band);
  cmd.AddValue ("phyModel", "PHY layer model", phyModel);
  cmd.AddValue ("constantMcs", "0 Minstrel or 1 constant", constantMcs);
  cmd.AddValue ("enablePcap", "Enable/disable pcap file generation", enablePcap);
  // Network A
  cmd.AddValue ("channelNumberA", "Channel number A", channelNumberA);
  cmd.AddValue ("channelWidthA", "Channel width A", channelWidthA);
  cmd.AddValue ("mcsA", "if set, limit testing to a specific MCS A", mcsA);
  cmd.AddValue ("giA", "Guard interval A", giA);
  cmd.AddValue ("txPowerA", "Transmission power A", txPowerA);
  cmd.AddValue ("dataRateA_fixed", "Data rate A", dataRateA_fixed);
  // Network B
  cmd.AddValue ("channelNumberB", "Channel number B", channelNumberB);
  cmd.AddValue ("channelWidthB", "Channel width B", channelWidthB);
  cmd.AddValue ("mcsB", "if set, limit testing to a specific MCS B", mcsB);
  cmd.AddValue ("giB", "Guard interval B", giB);
  cmd.AddValue ("txPowerB", "Transmission power B", txPowerB);
  cmd.AddValue ("dataRateB_fixed", "Data rate B", dataRateB_fixed);
  // Network C
  cmd.AddValue ("channelNumberC", "Channel number C", channelNumberC);
  cmd.AddValue ("channelWidthC", "Channel width C", channelWidthC);
  cmd.AddValue ("mcsC", "if set, limit testing to a specific MCS C", mcsC);
  cmd.AddValue ("giC", "Guard interval C", giC);
  cmd.AddValue ("txPowerC", "Transmission power C", txPowerC);
  cmd.AddValue ("dataRateC_fixed", "Data rate C", dataRateC_fixed);
  
  //cmd.AddValue ("nStaA", "Number Stations A", nStaA);
  //cmd.AddValue ("nStaB", "Number Stations B", nStaB);
  //cmd.AddValue ("nStaC", "Number Stations C", nStaC);
  cmd.Parse (argc, argv);
}


// function to set the channel number
void set_channel_number()
{
  for (int i = 0; i < nStaA; i++)
    Config::Set ("/NodeList/" + std::to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelNumber",
                 UintegerValue (channelNumberA));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelNumber",
               UintegerValue (channelNumberA)); ///NodeList/3/DeviceList 3?
  for (int i = 0; i < nStaB; i++)
    Config::Set ("/NodeList/" + std::to_string(nStaA+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelNumber",
                 UintegerValue (channelNumberB));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/1/$ns3::WifiNetDevice/Phy/ChannelNumber",
               UintegerValue (channelNumberB));
  for (int i = 0; i < nStaC; i++)
    Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelNumber",
                 UintegerValue (channelNumberC));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/2/$ns3::WifiNetDevice/Phy/ChannelNumber",
               UintegerValue (channelNumberC));
}


// function to set the channel width
void set_channel_width()
{
  for (int i = 0; i < nStaA; i++)
    Config::Set ("/NodeList/" + std::to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelWidth",
                 UintegerValue (channelWidthA));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelWidth",
               UintegerValue (channelWidthA));
  for (int i = 0; i < nStaB; i++)
    Config::Set ("/NodeList/" + std::to_string(nStaA+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelWidth",
                 UintegerValue (channelWidthB));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/1/$ns3::WifiNetDevice/Phy/ChannelWidth",
               UintegerValue (channelWidthB));
  for (int i = 0; i < nStaC; i++)
    Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelWidth",
                 UintegerValue (channelWidthC));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/2/$ns3::WifiNetDevice/Phy/ChannelWidth",
               UintegerValue (channelWidthC));
}


// function to set the tx power
void set_tx_power()
{
  for (int i = 0; i < nStaA; i++)
  {
    Config::Set ("/NodeList/" + std::to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerStart",
                 DoubleValue (txPowerA));
    Config::Set ("/NodeList/" + std::to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerEnd",
                 DoubleValue (txPowerA));
  }
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerStart",
               DoubleValue (txPowerA));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerEnd",
               DoubleValue (txPowerA));
  for (int i = 0; i < nStaB; i++)
  {
    Config::Set ("/NodeList/" + std::to_string(nStaA+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerStart",
                 DoubleValue (txPowerB));
    Config::Set ("/NodeList/" + std::to_string(nStaA+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerEnd",
                 DoubleValue (txPowerB));
  }
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/1/$ns3::WifiNetDevice/Phy/TxPowerStart",
               DoubleValue (txPowerB));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/1/$ns3::WifiNetDevice/Phy/TxPowerEnd",
               DoubleValue (txPowerB));
  for (int i = 0; i < nStaC; i++)
  {
    Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerStart",
                 DoubleValue (txPowerC));
    Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/TxPowerEnd",
                 DoubleValue (txPowerC));
  }
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/2/$ns3::WifiNetDevice/Phy/TxPowerStart",
               DoubleValue (txPowerC));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/2/$ns3::WifiNetDevice/Phy/TxPowerEnd",
               DoubleValue (txPowerC));
}


// function to set the guard interval
void set_guard_interval()
{
  for (int i = 0; i < nStaA; i++)
    Config::Set ("/NodeList/" + std::to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
                 TimeValue (NanoSeconds (giA)));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/0/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
               TimeValue (NanoSeconds (giA)));
  for (int i = 0; i < nStaB; i++)
    Config::Set ("/NodeList/" + std::to_string(nStaA+i) + "/DeviceList/0/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
                 TimeValue (NanoSeconds (giB)));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/1/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
               TimeValue (NanoSeconds (giB)));
  for (int i = 0; i < nStaC; i++)
    Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+i) + "/DeviceList/0/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
                 TimeValue (NanoSeconds (giC)));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/2/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
               TimeValue (NanoSeconds (giC)));
}


// function to set the modulation and coding scheme
void set_mcs()
{
  std::ostringstream ossA;
  ossA << "HeMcs" << mcsA;
  for (int i = 0; i < nStaA; i++)
  {
    Config::Set ("/NodeList/" + std::to_string(i) +
                 "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/DataMode",
                 StringValue (ossA.str ()));
    Config::Set ("/NodeList/" + std::to_string(i) +
                 "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/ControlMode",
                 StringValue (ossA.str ()));
  }
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) +
               "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/DataMode",
               StringValue (ossA.str ()));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) +
               "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/ControlMode",
               StringValue (ossA.str ()));
  std::ostringstream ossB;
  ossB << "HeMcs" << mcsB;
  for (int i = 0; i < nStaB; i++)
  {
    Config::Set ("/NodeList/" + std::to_string(nStaA+i) +
                 "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/DataMode",
                 StringValue (ossB.str ()));
    Config::Set ("/NodeList/" + std::to_string(nStaA+i) +
                 "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/ControlMode",
                 StringValue (ossB.str ()));
  }
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) +
               "/DeviceList/1/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/DataMode",
               StringValue (ossB.str ()));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) +
               "/DeviceList/1/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/ControlMode",
               StringValue (ossB.str ()));
  std::ostringstream ossC;
  ossC << "HeMcs" << mcsC;
  for (int i = 0; i < nStaC; i++)
  {
    Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+i) +
                 "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/DataMode",
                 StringValue (ossC.str ()));
    Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+i) +
                 "/DeviceList/0/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/ControlMode",
                 StringValue (ossC.str ()));
  }
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) +
               "/DeviceList/2/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/DataMode",
               StringValue (ossC.str ()));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) +
               "/DeviceList/2/$ns3::WifiNetDevice/RemoteStationManager/$ns3::ConstantRateWifiManager/ControlMode",
               StringValue (ossC.str ()));
}


// function to create a new C/S application
void new_application (uint16_t& index, NodeContainer staNodes, NodeContainer apNode, std::string dataRate_str,
					  Ipv4InterfaceContainer& apInterface, ApplicationContainer& clientApp, ApplicationContainer& serverApp)
{
  uint16_t port = 5000 + index;
  UdpServerHelper server (port);
  server.SetAttribute("Port", UintegerValue (port));

  serverApp = server.Install (apNode.Get (0));
  serverApp.Start (Seconds (0.0));
  serverApp.Stop (Seconds (simulationTime + 2));

  OnOffHelper client ("ns3::UdpSocketFactory", InetSocketAddress (apInterface.GetAddress (0), port));
  client.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  client.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  client.SetAttribute ("DataRate", StringValue (dataRate_str));
  client.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  clientApp = client.Install (staNodes.Get (index-1));
  clientApp.Start (Seconds (1.0));
  clientApp.Stop (Seconds (simulationTime + 1));
  index ++;
}


// function to update channel widths, channel numbers, GIs, MCSs, and Ptxs for each slice.
void update_channels (int i, Ptr<HybridBuildingsPropagationLossModel> lossModel,
                      NodeContainer staNodes, NodeContainer apNode)
{
  std::cout << "At time " << i << "s update_channels is called" << std::endl;

  // Compute Inputs: rx power through the path loss [dB] (meaningful only when mobility is involved), # of tx packets, # of rx packets and latency

  for (int i = 0; i < nStaA; i++)
  {
    rxPower[i] = txPowerA - lossModel->GetLoss (staNodes.Get(i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    //std::cout << "Received power A: " << rxPower[i] << std::endl;
  }
  for (int i = 0; i < nStaB; i++)
  {
    rxPower[nStaA+i] = txPowerB - lossModel->GetLoss (staNodes.Get(nStaA+i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    //std::cout << "Received power B: " << rxPower[nStaA+i] << std::endl;
  }
  for (int i = 0; i < nStaC; i++)
  {
    rxPower[nStaA+nStaB+i] = txPowerC - lossModel->GetLoss (staNodes.Get(nStaA+nStaB+i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    //std::cout << "Received power C: " << rxPower[nStaA+nStaB+i] << std::endl;
  }
  //flowMonitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = flowMonitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  {
    txPackets_unsort[i->first-1] = i->second.txPackets;
    rxPackets_unsort[i->first-1] = i->second.rxPackets;
    latency_unsort[i->first-1] = i->second.delaySum.ToDouble(Time::MS) / i->second.rxPackets;
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    destPorts[i->first-1] = t.destinationPort;
    //std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << "," << destPorts[i->first-1] << ")\n";
    //std::cout << "  Tx Packets: " << totTxPackets[i->first-1] << "\n";
    //std::cout << "  Rx Packets: " << totRxPackets[i->first-1] << "\n";
    //std::cout << "  Delay Sum: " << totDelay[i->first-1] << "\n";
    //std::cout << "  Delay Sum MS: " << i->second.delaySum.ToDouble(Time::MS) << "\n";
    //std::cout << "  Delay: " << i->second.delaySum / i->second.rxPackets << "\n";
    //std::cout << "  Delay MS: " << i->second.delaySum.ToDouble(Time::MS) / i->second.rxPackets << "\n";
  }
  // Shift txPackets, rxPackets, latency and probErr
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
  	txPackets[1][i] = txPackets[0][i];
  	rxPackets[1][i] = rxPackets[0][i];
  	latency[1][i] = latency[0][i];
  	probErr[1][i] = probErr[0][i];
  }
  // Compute txPackets, rxPackets, latency and probErr
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
    txPackets[0][destPorts[i]-5001] = txPackets_unsort[i];
    rxPackets[0][destPorts[i]-5001] = rxPackets_unsort[i];
    latency[0][destPorts[i]-5001] = latency_unsort[i];
  }
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
  	// probErr[0][i] = (txPackets[0][i] - rxPackets[0][i]) / (double)txPackets[0][i];
    probErr[0][i] = ((txPackets[0][i]-txPackets[1][i]) - (rxPackets[0][i]-rxPackets[1][i])) / (double)(txPackets[0][i]-txPackets[1][i]);
    std::cout << "Error Prob: " << probErr[0][i] << std::endl;
  }

  // Compute Outputs: channel width, channel number, guard interval, mcs, tx power
  
  /*
  *
  *
  * PLACE HERE YOUR CODE TO UPDATE THE CHANNELS' PROPERTIES
  *
  * INPUTS:     - rxPower, txPackets, rxPackets, latency, probErr
  *             - previous values of the channel properties
  *             - any characteristic of the scenario (e.g. nStaX and dataRateSumX, where X is A, B or C)
  *
  * OUTPUTS:    - channelWidthA, channelNumberA, giA, mcsA, txPowerA
  *             - channelWidthB, channelNumberB, giB, mcsB, txPowerB
  *             - channelWidthC, channelNumberC, giC, mcsC, txPowerC
  *
  *
  */
  
  // Set Outputs
  set_channel_number();
  set_channel_width();
  set_tx_power();
  set_guard_interval();
  set_mcs();

  // Write file
  std::ofstream out (csvFileName.c_str (), std::ios::app);
  out << "channelNumber, channelWidth, gi, mcs, txPower" << std::endl;
  out << channelNumberA << "," << channelWidthA << "," << giA << "," << mcsA << "," << txPowerA << std::endl;
  out << channelNumberB << "," << channelWidthB << "," << giB << "," << mcsB << "," << txPowerB << std::endl;
  out << channelNumberC << "," << channelWidthC << "," << giC << "," << mcsC << "," << txPowerC << std::endl;
  for (int i = 0; i < nStaA; i++)
  {
    out << dataRateA[i] << "," << x[i] << "," << y[i] << ","
    << txPackets[0][i] << "," << rxPackets[0][i] << "," << latency[0][i]
    << std::endl;
  }
  for (int i = 0; i < nStaB; i++)
  {
    out << dataRateB[i] << "," << x[nStaA+i] << "," << y[nStaA+i] << ","
    << txPackets[0][nStaA+i] << "," << rxPackets[0][nStaA+i] << "," << latency[0][nStaA+i] << std::endl;
  }
  for (int i = 0; i < nStaC; i++)
  {
    out << dataRateC[i] << "," << x[nStaA+nStaB+i] << "," << y[nStaA+nStaB+i] << ","
    << txPackets[0][nStaA+nStaB+i] << "," << rxPackets[0][nStaA+nStaB+i] << "," << latency[0][nStaA+nStaB+i] << std::endl;
  }
  out.close ();
}


// function to compute the initial channels' properties for each slice
void compute_channels (Ptr<HybridBuildingsPropagationLossModel> lossModel,
                      NodeContainer staNodes, NodeContainer apNode)
{
  std::cout << "At the beginning compute_channels is called" << std::endl;

  // Compute Inputs: rx power through the path loss [dB] (meaningful only when mobility is involved)
  txPowerA = 20; // Maximum power
  txPowerB = 20; // Maximum power
  txPowerC = 20; // Maximum power

  for (int i = 0; i < nStaA; i++)
  {
    rxPower[i] = txPowerA - lossModel->GetLoss (staNodes.Get(i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    //std::cout << "Received power A: " << rxPower[i] << std::endl;
  }
  for (int i = 0; i < nStaB; i++)
  {
    rxPower[nStaA+i] = txPowerB - lossModel->GetLoss (staNodes.Get(nStaA+i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    //std::cout << "Received power B: " << rxPower[nStaA+i] << std::endl;
  }
  for (int i = 0; i < nStaC; i++)
  {
    rxPower[nStaA+nStaB+i] = txPowerC - lossModel->GetLoss (staNodes.Get(nStaA+nStaB+i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    //std::cout << "Received power C: " << rxPower[nStaA+nStaB+i] << std::endl;
  }
  
  // Compute Outputs: channel width, channel number, guard interval, mcs, tx power
  
  /*
  *
  *
  * PLACE HERE YOUR CODE TO COMPUTE THE INITIAL CHANNELS' PROPERTIES
  *
  * INPUTS:     - rxPower
  *             - any characteristic of the scenario (e.g. nStaX and dataRateSumX, where X is A, B or C)
  *
  * OUTPUTS:    - channelWidthA, channelNumberA, giA, mcsA, txPowerA
  *             - channelWidthB, channelNumberB, giB, mcsB, txPowerB
  *             - channelWidthC, channelNumberC, giC, mcsC, txPowerC
  *
  *
  */

  // Set Outputs
  set_channel_number();
  set_channel_width();
  set_tx_power();
  set_guard_interval();
  set_mcs();

  // Write file
  std::ofstream out (csvFileName.c_str (), std::ios::app);
  out << "init_channelNumber, channelWidth, gi, mcs, txPower" << std::endl;
  out << channelNumberA << "," << channelWidthA << "," << giA << "," << mcsA << "," << txPowerA << std::endl;
  out << channelNumberB << "," << channelWidthB << "," << giB << "," << mcsB << "," << txPowerB << std::endl;
  out << channelNumberC << "," << channelWidthC << "," << giC << "," << mcsC << "," << txPowerC << std::endl;
  out.close ();
}


// function main
int main (int argc, char *argv[])
{
  // Define CMD commands
  configure(argc, argv);

  // Set the PRNG seed
  RngSeedManager::SetSeed (seed);

  // Set random throughput for each flow in the the 3 slices
  Ptr<UniformRandomVariable> dataRateA_ptr = CreateObject<UniformRandomVariable> ();
  dataRateA_ptr->SetAttribute ("Min", DoubleValue (80));
  dataRateA_ptr->SetAttribute ("Max", DoubleValue (101));
  std::vector<std::string> dataRateA_str(nStaA);
  for (int i = 0; i < nStaA; i++)
  {
    dataRateA[i] = (int) dataRateA_ptr->GetValue();
    dataRateA_str[i] = std::to_string(dataRateA[i]) + "Mb/s";
  }
  Ptr<UniformRandomVariable> dataRateB_ptr = CreateObject<UniformRandomVariable> ();
  dataRateB_ptr->SetAttribute ("Min", DoubleValue (30));
  dataRateB_ptr->SetAttribute ("Max", DoubleValue (51));
  std::vector<std::string> dataRateB_str(nStaB);
  for (int i = 0; i < nStaB; i++)
  {
    dataRateB[i] = (int) dataRateB_ptr->GetValue();
    dataRateB_str[i] = std::to_string(dataRateB[i]) + "Kb/s";
  }
  Ptr<UniformRandomVariable> dataRateC_ptr = CreateObject<UniformRandomVariable> ();
  dataRateC_ptr->SetAttribute ("Min", DoubleValue (20));
  dataRateC_ptr->SetAttribute ("Max", DoubleValue (41));
  std::vector<std::string> dataRateC_str(nStaC);
  for (int i = 0; i < nStaC; i++)
  {
    dataRateC[i] = (int) dataRateC_ptr->GetValue();
    dataRateC_str[i] = std::to_string(dataRateC[i]) + "Mb/s";
  }

  // Set random positions for STAs
  Ptr<UniformRandomVariable> x_ptr = CreateObject<UniformRandomVariable> ();
  x_ptr->SetAttribute ("Min", DoubleValue (0));
  x_ptr->SetAttribute ("Max", DoubleValue (x_max));
  Ptr<UniformRandomVariable> y_ptr = CreateObject<UniformRandomVariable> ();
  y_ptr->SetAttribute ("Min", DoubleValue (0));
  y_ptr->SetAttribute ("Max", DoubleValue (y_max));
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
    x[i] = x_ptr->GetValue ();
    y[i] = y_ptr->GetValue ();
  }

  // Compute Channels according to initialization algorithm
  for(std::vector<int>::iterator it = dataRateA.begin(); it != dataRateA.end(); ++it)
    dataRateSumA += *it;
  for(std::vector<int>::iterator it = dataRateB.begin(); it != dataRateB.end(); ++it)
    dataRateSumB += *it;
  for(std::vector<int>::iterator it = dataRateC.begin(); it != dataRateC.end(); ++it)
    dataRateSumC += *it;

  // Create nStaA + nStaB + nStaC STAs node objects and 1 AP node object
  NodeContainer staNodes;
  staNodes.Create (nStaA + nStaB + nStaC);
  NodeContainer apNode;
  apNode.Create (1);

  // Create a phy helper
  SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default ();
  YansWifiPhyHelper yansPhy = YansWifiPhyHelper::Default ();
  Ptr<HybridBuildingsPropagationLossModel> lossModel = CreateObject<HybridBuildingsPropagationLossModel> ();
  
  if (phyModel == "spectrum")
  {
  	// Create the channel
  	Ptr<MultiModelSpectrumChannel> channel = CreateObject<MultiModelSpectrumChannel> ();
  	channel->AddPropagationLossModel (lossModel);
  	Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  	channel->SetPropagationDelayModel (delayModel);
  	spectrumPhy.SetErrorRateModel ("ns3::NistErrorRateModel");
  	spectrumPhy.SetChannel (channel);
  	//spectrumPhy.Set ("TxPowerStart", DoubleValue (txPower));
  	//spectrumPhy.Set ("TxPowerEnd", DoubleValue (txPower));
  }
  else if (phyModel == "yans")
  {
    // Create the channel
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
    yansPhy.SetChannel (channel.Create ());
  	//yansPhy.Set ("TxPowerStart", DoubleValue (txPower));
  	//yansPhy.Set ("TxPowerEnd", DoubleValue (txPower));
  }
  else
  {
    std::cout << "Wrong phyModel value!" << std::endl;
    return 0;
  }

  //Create a WifiMacHelper and a WifiHelper
  WifiMacHelper mac;
  WifiHelper wifi;
  std::ostringstream oss;
  if (band == "AC_5")
  {
    wifi.SetStandard (WIFI_PHY_STANDARD_80211ac);
    Config::SetDefault ("ns3::HybridBuildingsPropagationLossModel::Frequency", DoubleValue (5.51e+09));
    if (constantMcs == 0)
    {
      wifi.SetRemoteStationManager ("ns3::MinstrelHtWifiManager");
    }
    else if (constantMcs == 1)
    {
      oss << "VhtMcs" << mcsA;
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                    "DataMode", StringValue (oss.str ()),
                                    "ControlMode", StringValue (oss.str ()));
    }
    else
    {
      std::cout << "Wrong constantMcs value!" << std::endl;
      return 0;
    }
  }
  else if (band == "AX_5")
  {
    wifi.SetStandard (WIFI_PHY_STANDARD_80211ax_5GHZ);
    Config::SetDefault ("ns3::HybridBuildingsPropagationLossModel::Frequency", DoubleValue (5.51e+09));
    if (constantMcs == 1)
    {
      oss << "HeMcs" << mcsA;
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                    "DataMode", StringValue (oss.str ()),
                                    "ControlMode", StringValue (oss.str ()));
    }
    else
    {
      std::cout << "With AX_5, constantMcs must be 1!" << std::endl;
      return 0;
    }
  }
  else if (band == "AX_2.4")
  {
  	wifi.SetStandard (WIFI_PHY_STANDARD_80211ax_2_4GHZ);
  	Config::SetDefault ("ns3::HybridBuildingsPropagationLossModel::Frequency", DoubleValue (2.44e+09));
    if (constantMcs == 1)
    {
      oss << "HeMcs" << mcsA;
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                    "DataMode", StringValue (oss.str ()),
                                    "ControlMode", StringValue (oss.str ()));
    }
    else
    {
      std::cout << "With AX_2.4, constantMcs must be 1!" << std::endl;
      return 0;
    }
  }
  else
  {
    std::cout << "Wrong band value!" << std::endl;
    return 0;
  }
  
  // Declare NetDeviceContainers to hold the container returned by the helper
  std::vector<NetDeviceContainer> staDeviceA(nStaA);
  std::vector<NetDeviceContainer> staDeviceB(nStaB);
  std::vector<NetDeviceContainer> staDeviceC(nStaC);
  NetDeviceContainer apDeviceA, apDeviceB, apDeviceC;
  Ssid ssid;

  if (phyModel == "spectrum")
  {
  	// Network A
  	ssid = Ssid ("networkA");
  	spectrumPhy.Set ("ChannelNumber", UintegerValue (channelNumberA));
    spectrumPhy.Set ("TxPowerStart", DoubleValue (txPowerA));
    spectrumPhy.Set ("TxPowerEnd", DoubleValue (txPowerA));
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaA; i++)
      staDeviceA[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i));
  	mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
  	apDeviceA = wifi.Install (spectrumPhy, mac, apNode.Get(0));

  	// Network B
  	ssid = Ssid ("networkB");
  	spectrumPhy.Set ("ChannelNumber", UintegerValue (channelNumberB));
    spectrumPhy.Set ("TxPowerStart", DoubleValue (txPowerB));
    spectrumPhy.Set ("TxPowerEnd", DoubleValue (txPowerB));
  	mac.SetType ("ns3::StaWifiMac",
  				 "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaB; i++)
      staDeviceB[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA));
  	mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
  	apDeviceB = wifi.Install (spectrumPhy, mac, apNode.Get(0));

    // Network C
    ssid = Ssid ("networkC");
    spectrumPhy.Set ("ChannelNumber", UintegerValue (channelNumberC));
    spectrumPhy.Set ("TxPowerStart", DoubleValue (txPowerC));
    spectrumPhy.Set ("TxPowerEnd", DoubleValue (txPowerC));
    mac.SetType ("ns3::StaWifiMac",
           "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaC; i++)
      staDeviceC[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA + nStaB));
    mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
    apDeviceC = wifi.Install (spectrumPhy, mac, apNode.Get(0));
  }
  else if (phyModel == "yans")
  {
    // Network A
    ssid = Ssid ("networkA");
    yansPhy.Set ("ChannelNumber", UintegerValue (channelNumberA));
    yansPhy.Set ("TxPowerStart", DoubleValue (txPowerA));
    yansPhy.Set ("TxPowerEnd", DoubleValue (txPowerA));
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaA; i++)
      staDeviceA[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i));
    mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
    apDeviceA = wifi.Install (yansPhy, mac, apNode.Get(0));

    // Network B
    ssid = Ssid ("networkB");
    yansPhy.Set ("ChannelNumber", UintegerValue (channelNumberB));
    yansPhy.Set ("TxPowerStart", DoubleValue (txPowerB));
    yansPhy.Set ("TxPowerEnd", DoubleValue (txPowerB));
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaB; i++)
      staDeviceB[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA));
    mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
    apDeviceB = wifi.Install (yansPhy, mac, apNode.Get(0));

    // Network C
    ssid = Ssid ("networkC");
    yansPhy.Set ("ChannelNumber", UintegerValue (channelNumberC));
    yansPhy.Set ("TxPowerStart", DoubleValue (txPowerC));
    yansPhy.Set ("TxPowerEnd", DoubleValue (txPowerC));
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaC; i++)
      staDeviceC[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA + nStaB));
    mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
    apDeviceC = wifi.Install (yansPhy, mac, apNode.Get(0));
  }
  else
  {
    std::cout << "Wrong phyModel value!" << std::endl;
    return 0;
  }

  set_channel_width();
  set_guard_interval();
  set_mcs();

  // Set RTS-CTS
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/RtsCtsThreshold",
               UintegerValue (100));

  // Create building
  Ptr<Building> b = CreateObject <Building> ();
  b->SetBoundaries (Box (0.0, x_max, 0.0, y_max, 0.0, z_max));
  b->SetBuildingType (Building::Residential);
  b->SetExtWallsType (Building::ConcreteWithWindows);
  b->SetNFloors (1);
  b->SetNRoomsX (1);
  b->SetNRoomsY (1);

  // Setting mobility model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // Set position for AP
  positionAlloc->Add (Vector (10.0, 5.0, 2.9));
  // Set position for STAs
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
    positionAlloc->Add (Vector (x[i], y[i], 1.5));
  MobilityHelper mobility;
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (apNode);
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
  							 "Bounds", RectangleValue (Rectangle (0.0, x_max, 0.0, y_max)));
  for (int i = 0; i < nStaA; i++)
  	mobility.Install (staNodes.Get (i));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  for (int i = 0; i < nStaB; i++)
  	mobility.Install (staNodes.Get (nStaA+i));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
  							 "Bounds", RectangleValue (Rectangle (0.0, x_max, 0.0, y_max)));
  for (int i = 0; i < nStaC; i++)
  	mobility.Install (staNodes.Get (nStaA+nStaB+i));
  BuildingsHelper::Install (apNode);
  BuildingsHelper::Install (staNodes);
  BuildingsHelper::MakeMobilityModelConsistent ();

  compute_channels (lossModel, staNodes, apNode);

  // Internet stack
  InternetStackHelper stack;
  stack.Install (apNode);
  stack.Install (staNodes);

  Ipv4AddressHelper address;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  std::vector<Ipv4InterfaceContainer> staInterfaceA(nStaA);
  for (int i = 0; i < nStaA; i++)
    staInterfaceA[i] = address.Assign (staDeviceA[i]);
  Ipv4InterfaceContainer apInterfaceA = address.Assign (apDeviceA);

  address.SetBase ("192.168.2.0", "255.255.255.0");
  std::vector<Ipv4InterfaceContainer> staInterfaceB(nStaB);
  for (int i = 0; i < nStaB; i++)
    staInterfaceB[i] = address.Assign (staDeviceB[i]);
  Ipv4InterfaceContainer apInterfaceB = address.Assign (apDeviceB);

  address.SetBase ("192.168.3.0", "255.255.255.0");
  std::vector<Ipv4InterfaceContainer> staInterfaceC(nStaC);
  for (int i = 0; i < nStaC; i++)
    staInterfaceC[i] = address.Assign (staDeviceC[i]);
  Ipv4InterfaceContainer apInterfaceC = address.Assign (apDeviceC);

  // Flow monitor
  flowMonitor = flowHelper.InstallAll();

  // Setting applications
  uint16_t index = 1;
  std::vector<ApplicationContainer> clientAppA(nStaA), serverAppA(nStaA);
  std::vector<ApplicationContainer> clientAppB(nStaB), serverAppB(nStaB);
  std::vector<ApplicationContainer> clientAppC(nStaC), serverAppC(nStaC);

  for (int i = 0; i < nStaA; i++)
    new_application (index, staNodes, apNode, dataRateA_str[i], apInterfaceA, clientAppA[i], serverAppA[i]);

  for (int i = 0; i < nStaB; i++)
    new_application (index, staNodes, apNode, dataRateB_str[i], apInterfaceB, clientAppB[i], serverAppB[i]);

  for (int i = 0; i < nStaC; i++)
    new_application (index, staNodes, apNode, dataRateC_str[i], apInterfaceC, clientAppC[i], serverAppC[i]);

  if (enablePcap)
  {
  	if (phyModel == "spectrum")
  	{
  		spectrumPhy.EnablePcap ("AP_A", apDeviceA.Get (0));
  		spectrumPhy.EnablePcap ("STA_A", staDeviceA[0].Get (0));
  		spectrumPhy.EnablePcap ("AP_B", apDeviceB.Get (0));
  		spectrumPhy.EnablePcap ("STA_B", staDeviceB[0].Get (0));
  		spectrumPhy.EnablePcap ("AP_C", apDeviceC.Get (0));
  		spectrumPhy.EnablePcap ("STA_C", staDeviceC[0].Get (0));
  	}
  	else if (phyModel == "yans")
  	{
  		yansPhy.EnablePcap ("AP_A", apDeviceA.Get (0));
  		yansPhy.EnablePcap ("STA_A", staDeviceA[0].Get (0));
  		yansPhy.EnablePcap ("AP_B", apDeviceB.Get (0));
  		yansPhy.EnablePcap ("STA_B", staDeviceB[0].Get (0));
  		yansPhy.EnablePcap ("AP_C", apDeviceC.Get (0));
  		yansPhy.EnablePcap ("STA_C", staDeviceC[0].Get (0));
  	}
  	else
  	{
  		std::cout << "Wrong phyModel value!" << std::endl;
  		return 0;
  	}
  }

  Simulator::Stop (Seconds (simulationTime + 2));

  for (int i = 2; i < simulationTime +1; i++)
  {
    Simulator::Schedule(Seconds(i), &update_channels,
                        i, lossModel, staNodes, apNode);
  }

  time_t timeNow = time(0);
  char* ctimeNow =ctime(&timeNow);
  std::cout << OKBLUE <<"Simulation started!  Time: " << ctimeNow << ENDC;

  Simulator::Run ();

  // Activate/deactivate the histograms and the per-probe detailed stats
  std::cout << OKBLUE <<"Writing to file: FlowMonitorFile.xml"<< ENDC << std::endl;
  flowMonitor->SerializeToXmlFile("FlowMonitorFile.xml", false, false);

  // Show results
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = flowMonitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  {
    txPackets_unsort[i->first-1] = i->second.txPackets;
    rxPackets_unsort[i->first-1] = i->second.rxPackets;
    latency_unsort[i->first-1] = i->second.delaySum.ToDouble(Time::MS) / i->second.rxPackets;
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    destPorts[i->first-1] = t.destinationPort;
    //std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << "," << destPorts[i->first-1] << ")\n";
    //std::cout << "  Tx Packets: " << totTxPackets[i->first-1] << "\n";
    //std::cout << "  Rx Packets: " << totRxPackets[i->first-1] << "\n";
    //std::cout << "  Delay Sum: " << totDelay[i->first-1] << "\n";
    //std::cout << "  Delay Sum MS: " << i->second.delaySum.ToDouble(Time::MS) << "\n";
    //std::cout << "  Delay: " << i->second.delaySum / i->second.rxPackets << "\n";
    //std::cout << "  Delay MS: " << i->second.delaySum.ToDouble(Time::MS) / i->second.rxPackets << "\n";
  }
  // Shift txPackets, rxPackets and latency
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
  	txPackets[1][i] = txPackets[0][i];
  	rxPackets[1][i] = rxPackets[0][i];
  	latency[1][i] = latency[0][i];
  }
  // Compute txPackets, rxPackets and latency
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
    txPackets[0][destPorts[i]-5001] = txPackets_unsort[i];
    rxPackets[0][destPorts[i]-5001] = rxPackets_unsort[i];
    latency[0][destPorts[i]-5001] = latency_unsort[i];
  }

  timeNow = time(0);
  ctimeNow =ctime(&timeNow);
  std::cout << OKBLUE << "Simulation finished! Time: " << ctimeNow << ENDC;
  Simulator::Destroy ();

  // Write file csvFileName.csv
  std::cout << OKBLUE <<"Writing to file: " << csvFileName << ENDC << std::endl;
  // std::ofstream out (csvFileName.c_str ()); // Use it to overwrite the file
  std::ofstream out (csvFileName.c_str (), std::ios::app);
  out << "fin_channelNumber, channelWidth, gi, mcs, txPower" << std::endl;
  out << channelNumberA << "," << channelWidthA << "," << giA << "," << mcsA << "," << txPowerA << std::endl;
  out << channelNumberB << "," << channelWidthB << "," << giB << "," << mcsB << "," << txPowerB << std::endl;
  out << channelNumberC << "," << channelWidthC << "," << giC << "," << mcsC << "," << txPowerC << std::endl;
  for (int i = 0; i < nStaA; i++)
  {
    out << dataRateA[i] << "," << x[i] << "," << y[i] << ","
    << txPackets[0][i] << "," << rxPackets[0][i] << "," << latency[0][i]
    << std::endl;
  }
  for (int i = 0; i < nStaB; i++)
  {
    out << dataRateB[i] << "," << x[nStaA+i] << "," << y[nStaA+i] << ","
    << txPackets[0][nStaA+i] << "," << rxPackets[0][nStaA+i] << "," << latency[0][nStaA+i] << std::endl;
  }
  for (int i = 0; i < nStaC; i++)
  {
    out << dataRateC[i] << "," << x[nStaA+nStaB+i] << "," << y[nStaA+nStaB+i] << ","
    << txPackets[0][nStaA+nStaB+i] << "," << rxPackets[0][nStaA+nStaB+i] << "," << latency[0][nStaA+nStaB+i] << std::endl;
  }
  out.close ();

  return 0;
}