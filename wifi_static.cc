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


#define ENDC    "\033[0m"
#define ERROR   "\033[91m"
#define OKGREEN	"\033[92m"
#define WARNING	"\033[93m"
#define OKBLUE	"\033[94m"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("wifin");


uint64_t nPacketsSent = 0;


void SourceUdpTrace (Ptr<const Packet> pkt)
{
  nPacketsSent++;
}



void handler (int x, ApplicationContainer serverAppA, ApplicationContainer serverAppB)
{
  std::cout << "At time 5s handler called with argument arg0 = " << x << std::endl;
  // Parameters from here can be modified:
  // https://www.nsnam.org/doxygen/classns3_1_1_yans_wifi_phy.html

  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (40));
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelNumber", UintegerValue (52));
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/TxPowerStart", DoubleValue (16));
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/TxPowerEnd", DoubleValue (16));
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval", TimeValue (NanoSeconds (1600)));


  uint64_t totalPacketsThroughputA = DynamicCast<UdpServer> (serverAppA.Get (0))->GetReceived ();
  uint64_t totalPacketsThroughputB = DynamicCast<UdpServer> (serverAppB.Get (0))->GetReceived ();

  uint64_t totalPacketsLossA = DynamicCast<UdpServer> (serverAppA.Get (0))->GetLost (); // added
  uint64_t totalPacketsLossB = DynamicCast<UdpServer> (serverAppB.Get (0))->GetLost (); // added

  std::cout << "ReceivedA = " << totalPacketsThroughputA << std::endl; // added
  std::cout << "LossA = " << totalPacketsLossA << std::endl; // added

  std::cout << "ReceivedB = " << totalPacketsThroughputB << std::endl; // added
  std::cout << "LossB = " << totalPacketsLossB << std::endl; // added
}



void handler2 (int x, ApplicationContainer serverAppA, ApplicationContainer serverAppB)
{
  std::cout << "At time 10s handler2 called with argument arg0 = " << x << std::endl;



  uint64_t totalPacketsThroughputA = DynamicCast<UdpServer> (serverAppA.Get (0))->GetReceived ();
  uint64_t totalPacketsThroughputB = DynamicCast<UdpServer> (serverAppB.Get (0))->GetReceived ();

  uint64_t totalPacketsLossA = DynamicCast<UdpServer> (serverAppA.Get (0))->GetLost (); // added
  uint64_t totalPacketsLossB = DynamicCast<UdpServer> (serverAppB.Get (0))->GetLost (); // added

  std::cout << "ReceivedA = " << totalPacketsThroughputA << std::endl; // added
  std::cout << "LossA = " << totalPacketsLossA << std::endl; // added

  std::cout << "ReceivedB = " << totalPacketsThroughputB << std::endl; // added
  std::cout << "LossB = " << totalPacketsLossB << std::endl; // added
}



void new_application (uint16_t& index, uint32_t payloadSize, double simulationTime,
					  NodeContainer staNodes, NodeContainer apNode, std::string dataRate_str,
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



void compute_channels (int dataRateSumA, int& channelNumberA, int& channelWidthA,
					   int dataRateSumB, int& channelNumberB, int& channelWidthB,
					   int dataRateSumC, int& channelNumberC, int& channelWidthC)
{
  if (dataRateSumA < 65)
  {
  	channelWidthA = 20;
  	channelNumberA = 36;
  }
  else if (dataRateSumA < 130)
  {
    channelWidthA = 40;
  	channelNumberA = 38;
  }
  else if (dataRateSumA < 272)
  {
    channelWidthA = 80;
  	channelNumberA = 42;
  }
  else
  {
    channelWidthA = 160;
  	channelNumberA = 50;
  }
  channelWidthB = 20;
  channelNumberB = 100;
  if (dataRateSumC < 65)
  {
  	channelWidthC = 20;
  	channelNumberC = 161;
  }
  else if (dataRateSumC < 130)
  {
    channelWidthC = 40;
  	channelNumberC = 159;
  }
  else if (dataRateSumC < 272)
  {
    channelWidthC = 80;
  	channelNumberC = 155;
  }
  else
  {
    channelWidthC = 160;
  	channelNumberC = 144;
  }
}



int main (int argc, char *argv[])
{
  uint32_t payloadSize = 1472; //bytes (UDP)
  double simulationTime = 20; //seconds 15
  int seed = 1;
  std::string csvFileName = "testFile.csv";
  std::string band = "AX_5"; // AC_5, AX_2.4 or AX_5
  std::string phyModel = "spectrum"; // whether "spectrum" or "yans"
  bool constantMcs = 1; // 0 Minstrel or 1 constant
  bool enablePcap = 0;
  // Network A
  int channelNumberA = 42; // channel number A
  int channelWidthA = 20; //whether 20 or 40 or 80 or 160 MHz
  int mcs = 5; //-1 indicates an unset value, 0 to 11
  int giA = 1600; //whether 800 or 1600 or 3200 ns
  int txPowerA = 2; //dBm
  double xA = 0.0;//3.0; //meters
  double yA = 3.0; //meters
  std::string dataRateA_old = "10Mb/s";
  // Network B
  int channelNumberB = 114; // channel number B
  int channelWidthB = 20; //whether 20 or 40 or 80 or 160 MHz
  //int mcsB = 5; //-1 indicates an unset value, 0 to 11
  int giB = 1600; //whether 800 or 1600 or 3200 ns
  int txPowerB = -4; //dBm
  double xB = -9.0;//-16.0; //meters
  double yB = 4.0; //meters
  std::string dataRateB_old = "10Mb/s";
  // Network C
  int channelNumberC = 155; // channel number B
  int channelWidthC = 40; //whether 20 or 40 or 80 or 160 MHz
  //int mcsC = 5; //-1 indicates an unset value, 0 to 11
  int giC = 1600; //whether 800 or 1600 or 3200 ns
  int txPowerC = 18; //dBm
  double xC = 8.0;//-3.0; //meters
  double yC = -4.5;//-5.0; //meters
  std::string dataRateC_old = "10Mb/s";
  int nStaA = 1;
  int nStaB = 1;
  int nStaC = 1;
  double x_max = 20.0;
  double y_max = 10.0;
  double z_max = 3.0;

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
  cmd.AddValue ("mcs", "if set, limit testing to a specific MCS A", mcs);
  cmd.AddValue ("giA", "Guard interval A", giA);
  cmd.AddValue ("txPowerA", "Transmission power A", txPowerA);
  cmd.AddValue ("xA", "Distance in meters xA", xA);
  cmd.AddValue ("yA", "Distance in meters yA", yA);
  cmd.AddValue ("dataRateA_old", "Data rate A", dataRateA_old);
  // Network B
  cmd.AddValue ("channelNumberB", "Channel number B", channelNumberB);
  cmd.AddValue ("channelWidthB", "Channel width B", channelWidthB);
  //cmd.AddValue ("mcsB", "if set, limit testing to a specific MCS B", mcsB);
  cmd.AddValue ("giB", "Guard interval B", giB);
  cmd.AddValue ("txPowerB", "Transmission power B", txPowerB);
  cmd.AddValue ("xB", "Distance in meters xB", xB);
  cmd.AddValue ("yB", "Distance in meters yB", yB);
  cmd.AddValue ("dataRateB_old", "Data rate B", dataRateB_old);
  // Network B
  cmd.AddValue ("channelNumberC", "Channel number C", channelNumberC);
  cmd.AddValue ("channelWidthC", "Channel width C", channelWidthC);
  //cmd.AddValue ("mcsC", "if set, limit testing to a specific MCS C", mcsC);
  cmd.AddValue ("giC", "Guard interval C", giC);
  cmd.AddValue ("txPowerC", "Transmission power C", txPowerC);
  cmd.AddValue ("xC", "Distance in meters xC", xC);
  cmd.AddValue ("yC", "Distance in meters yC", yC);
  cmd.AddValue ("dataRateC_old", "Data rate C", dataRateC_old);
  cmd.AddValue ("nStaA", "Number Stations A", nStaA);
  cmd.AddValue ("nStaB", "Number Stations B", nStaB);
  cmd.AddValue ("nStaC", "Number Stations C", nStaC);
  cmd.Parse (argc, argv);

  // Set the PRNG seed
  RngSeedManager::SetSeed (seed);

  // Set random throughput for each flow in the the 3 slices
  Ptr<UniformRandomVariable> dataRateA_ptr = CreateObject<UniformRandomVariable> ();
  dataRateA_ptr->SetAttribute ("Min", DoubleValue (80)); //80
  dataRateA_ptr->SetAttribute ("Max", DoubleValue (101)); //101
  std::vector<int> dataRateA(nStaA);
  std::vector<std::string> dataRateA_str(nStaA);
  for (int i = 0; i < nStaA; i++)
  {
    dataRateA[i] = (int) dataRateA_ptr->GetValue();
    dataRateA_str[i] = std::to_string(dataRateA[i]) + "Mb/s";
  }
  Ptr<UniformRandomVariable> dataRateB_ptr = CreateObject<UniformRandomVariable> ();
  dataRateB_ptr->SetAttribute ("Min", DoubleValue (30)); //30
  dataRateB_ptr->SetAttribute ("Max", DoubleValue (51)); //51
  std::vector<int> dataRateB(nStaB);
  std::vector<std::string> dataRateB_str(nStaB);
  for (int i = 0; i < nStaB; i++)
  {
    dataRateB[i] = (int) dataRateB_ptr->GetValue();
    dataRateB_str[i] = std::to_string(dataRateB[i]) + "Kb/s";
  }
  Ptr<UniformRandomVariable> dataRateC_ptr = CreateObject<UniformRandomVariable> ();
  dataRateC_ptr->SetAttribute ("Min", DoubleValue (20)); //20
  dataRateC_ptr->SetAttribute ("Max", DoubleValue (41)); //41
  std::vector<int> dataRateC(nStaC);
  std::vector<std::string> dataRateC_str(nStaC);
  for (int i = 0; i < nStaC; i++)
  {
    dataRateC[i] = (int) dataRateC_ptr->GetValue();
    dataRateC_str[i] = std::to_string(dataRateC[i]) + "Mb/s";
  }

  // Set positions for STAs
  Ptr<UniformRandomVariable> x_ptr = CreateObject<UniformRandomVariable> ();
  x_ptr->SetAttribute ("Min", DoubleValue (0));
  x_ptr->SetAttribute ("Max", DoubleValue (x_max));
  std::vector<double> x(nStaA+nStaB+nStaC);
  Ptr<UniformRandomVariable> y_ptr = CreateObject<UniformRandomVariable> ();
  y_ptr->SetAttribute ("Min", DoubleValue (0));
  y_ptr->SetAttribute ("Max", DoubleValue (y_max));
  std::vector<double> y(nStaA+nStaB+nStaC);
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
    x[i] = x_ptr->GetValue ();
    y[i] = y_ptr->GetValue ();
  }

  // Compute Channels according to Naive algorithm
  int dataRateSumA = 0;
  for(std::vector<int>::iterator it = dataRateA.begin(); it != dataRateA.end(); ++it)
    dataRateSumA += *it;
  int dataRateSumB = 0;
  for(std::vector<int>::iterator it = dataRateB.begin(); it != dataRateB.end(); ++it)
    dataRateSumB += *it;
  int dataRateSumC = 0;
  for(std::vector<int>::iterator it = dataRateC.begin(); it != dataRateC.end(); ++it)
    dataRateSumC += *it;
  //std::cout << "ciaooa1: " << dataRateSumA << "\n";
  //std::cout << "ciaooa2: " << dataRateSumB << "\n";
  //std::cout << "ciaooa3: " << dataRateSumC << "\n";
  compute_channels (dataRateSumA, channelNumberA, channelWidthA,
  					dataRateSumB, channelNumberB, channelWidthB,
  					dataRateSumC, channelNumberC, channelWidthC);



  std::cout << "ciaooa1: " << channelNumberA << channelWidthA << "\n";
  std::cout << "ciaooa2: " << channelNumberB << channelWidthB << "\n";
  std::cout << "ciaooa3: " << channelNumberC << channelWidthC << "\n";




  // Create nStaA + nStaB + nStaC STAs node objects and 1 AP node object
  NodeContainer staNodes;
  staNodes.Create (nStaA + nStaB + nStaC);
  NodeContainer apNode;
  apNode.Create (1);

  NodeContainer nodes = NodeContainer(staNodes,apNode);

  // Create a phy helper
  SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default ();
  YansWifiPhyHelper yansPhy = YansWifiPhyHelper::Default ();
  Ptr<HybridBuildingsPropagationLossModel> lossModel = CreateObject<HybridBuildingsPropagationLossModel> ();
  
  if (phyModel == "spectrum")
  {
  	// Create the channel
  	Ptr<MultiModelSpectrumChannel> channel = CreateObject<MultiModelSpectrumChannel> ();

  	//Ptr<HybridBuildingsPropagationLossModel> lossModel = CreateObject<HybridBuildingsPropagationLossModel> ();
  	//Ptr<LogDistancePropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> ();

  	//Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel> ();
  	//lossModel->SetFrequency (5.180e9);
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
      oss << "VhtMcs" << mcs;
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
      oss << "HeMcs" << mcs;
      // Equivalent to "DataMode", StringValue ("HeMcs1"), "ControlMode", StringValue ("HeMcs1")
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
    // The default ReferenceLoss is calculated at 5 GHz
    // Config::SetDefault ("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue (40.046));
    if (constantMcs == 1)
    {
      oss << "HeMcs" << mcs;
      // Equivalent to "DataMode", StringValue ("HeMcs1"), "ControlMode", StringValue ("HeMcs1")
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

  /*
  std::ostringstream oss;
  oss << "HeMcs" << mcs;
  // Equivalent to "DataMode", StringValue ("HeMcs1"), "ControlMode", StringValue ("HeMcs1")
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (oss.str ()), "ControlMode", StringValue (oss.str ()));
  */

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
                 //"EnableBeaconJitter", BooleanValue (false),
                 //"BeaconGeneration", BooleanValue (false),
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
                 //"BeaconGeneration", BooleanValue (true),
                 //"EnableBeaconJitter", BooleanValue (false),
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
                 //"BeaconGeneration", BooleanValue (true),
                 //"EnableBeaconJitter", BooleanValue (false),
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
                 //"EnableBeaconJitter", BooleanValue (false),
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
                 //"EnableBeaconJitter", BooleanValue (false),
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
                 //"EnableBeaconJitter", BooleanValue (false),
                 "Ssid", SsidValue (ssid));
    apDeviceC = wifi.Install (yansPhy, mac, apNode.Get(0));
  }
  else
  {
    std::cout << "Wrong phyModel value!" << std::endl;
    return 0;
  }

  // Set channel width
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth",
  //             UintegerValue (channelWidthA));

 
  for (int i = 0; i < nStaA; i++)
    Config::Set ("/NodeList/" + std::to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelWidth",
                 UintegerValue (channelWidthA));
  Config::Set ("/NodeList/" + std::to_string(nStaA+nStaB+nStaC) + "/DeviceList/0/$ns3::WifiNetDevice/Phy/ChannelWidth",
               UintegerValue (channelWidthA)); ///NodeList/3/DeviceList 3?
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



  // Set guard interval
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
  //             TimeValue (NanoSeconds (gi)));
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

  // Set rts cts
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/RemoteStationManager/RtsCtsThreshold",
  			   UintegerValue (100));
  // Set MPDU buffer size
  // Present in he-wifi-network.cc but unknown instruction
  //Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/MpduBufferSize",
  //             UintegerValue (useExtendedBlockAck ? 256 : 64));

  /* Create building */
  Ptr<Building> b = CreateObject <Building> ();
  b->SetBoundaries (Box (0.0, x_max, 0.0, y_max, 0.0, z_max));
  b->SetBuildingType (Building::Residential);
  b->SetExtWallsType (Building::ConcreteWithWindows);
  b->SetNFloors (1);
  b->SetNRoomsX (1);
  b->SetNRoomsY (1);

  /* Setting mobility model */
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // Set position for AP
  positionAlloc->Add (Vector (10.0, 5.0, 2.9));
  // Set position for STAs
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
    positionAlloc->Add (Vector (x[i], y[i], 1.5));
  /*
  for (int i = 0; i < nStaA; i++)
    positionAlloc->Add (Vector (10+xA+i, 5+yA, 1.0));
  for (int i = 0; i < nStaB; i++)
    positionAlloc->Add (Vector (10+xB+i, 5+yB, 1.0)); //i/4
  for (int i = 0; i < nStaC; i++)
    positionAlloc->Add (Vector (10+xC+i, 5+yC, 1.0));
    */
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

  /* Internet stack */
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
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  /* Setting applications */
  uint16_t index = 1;
  std::vector<ApplicationContainer> clientAppA(nStaA), serverAppA(nStaA);
  std::vector<ApplicationContainer> clientAppB(nStaB), serverAppB(nStaB);
  std::vector<ApplicationContainer> clientAppC(nStaC), serverAppC(nStaC);

  for (int i = 0; i < nStaA; i++)
    new_application (index, payloadSize, simulationTime, staNodes, apNode,
                     dataRateA_str[i], apInterfaceA, clientAppA[i], serverAppA[i]);

  for (int i = 0; i < nStaB; i++)
    new_application (index, payloadSize, simulationTime, staNodes, apNode,
                     dataRateB_str[i], apInterfaceB, clientAppB[i], serverAppB[i]);

  for (int i = 0; i < nStaC; i++)
    new_application (index, payloadSize, simulationTime, staNodes, apNode,
                     dataRateC_str[i], apInterfaceC, clientAppC[i], serverAppC[i]);

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

  //Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::OnOffApplication/Tx",MakeCallback(&SourceUdpTrace));



  //Simulator::Schedule(Seconds(5), &handler, 40, serverAppA, serverAppB);
  //Simulator::Schedule(Seconds(10), &handler2, 40, serverAppA, serverAppB);



  time_t timeNow = time(0);
  char* ctimeNow =ctime(&timeNow);
  std::cout << OKBLUE <<"Simulation started!  Time: " << ctimeNow << ENDC;

 /*
  // NetAnim
  AnimationInterface anim ("animation.xml");
  for (int i = 0; i < nStaA; ++i)
    {
      anim.UpdateNodeDescription (staNodes.Get(i), "staA");
      anim.UpdateNodeColor (staNodes.Get(i), 255, 255, 0);
    }
  for (int i = 0; i < nStaB; ++i)
    {
      anim.UpdateNodeDescription (staNodes.Get(nStaA+i), "staB");
      anim.UpdateNodeColor (staNodes.Get(nStaA+i), 0, 0, 255);
    }
  for (int i = 0; i < nStaC; ++i)
    {
      anim.UpdateNodeDescription (staNodes.Get (nStaA+nStaB+i), "staC"); // Optional
      anim.UpdateNodeColor (staNodes.Get (nStaA+nStaB+i), 255, 0, 0); // Optional 
    }
  anim.UpdateNodeDescription (apNode.Get(0), "AP");
  anim.UpdateNodeColor (apNode.Get(0), 128, 128, 128);
  for (int i = 0; i < nStaA+nStaB+nStaC+1; ++i)
    {
      anim.UpdateNodeSize (i,0.3,0.3);
    }
 */

  Simulator::Run ();

  // Activate/deactivate the histograms and the per-probe detailed stats
  std::cout << OKBLUE <<"Writing to file: FlowMonitorFile.xml"<< ENDC << std::endl;
  flowMonitor->SerializeToXmlFile("FlowMonitorFile.xml", false, false);

  /* Show results */
  std::vector<uint64_t> totalPacketsRxA(nStaA), totalPacketsLossA(nStaA);
  std::vector<uint64_t> totalPacketsRxB(nStaB), totalPacketsLossB(nStaB);
  std::vector<uint64_t> totalPacketsRxC(nStaC), totalPacketsLossC(nStaC);

  for (int i = 0; i < nStaA; i++)
  {
    totalPacketsRxA[i] = DynamicCast<UdpServer> (serverAppA[i].Get (0))->GetReceived ();
    totalPacketsLossA[i] = DynamicCast<UdpServer> (serverAppA[i].Get (0))->GetLost ();
  }
  
  for (int i = 0; i < nStaB; i++)
  {
    totalPacketsRxB[i] = DynamicCast<UdpServer> (serverAppB[i].Get (0))->GetReceived ();
    totalPacketsLossB[i] = DynamicCast<UdpServer> (serverAppB[i].Get (0))->GetLost ();
  }

  for (int i = 0; i < nStaC; i++)
  {
    totalPacketsRxC[i] = DynamicCast<UdpServer> (serverAppC[i].Get (0))->GetReceived ();
    totalPacketsLossC[i] = DynamicCast<UdpServer> (serverAppC[i].Get (0))->GetLost ();
  }



  std::vector<uint32_t> txPackets_unsort(nStaA+nStaB+nStaC);
  std::vector<uint32_t> rxPackets_unsort(nStaA+nStaB+nStaC);
  std::vector<double> latency_unsort(nStaA+nStaB+nStaC);
  std::vector<uint32_t> txPackets(nStaA+nStaB+nStaC);
  std::vector<uint32_t> rxPackets(nStaA+nStaB+nStaC);
  std::vector<double> latency(nStaA+nStaB+nStaC);

  std::vector<uint32_t> destPorts(nStaA+nStaB+nStaC);

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
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
    txPackets[destPorts[i]-5001] = txPackets_unsort[i];
    rxPackets[destPorts[i]-5001] = rxPackets_unsort[i];
    latency[destPorts[i]-5001] = latency_unsort[i];
  }
/*
  std::vector<double> pRx(nStaA+nStaB+nStaC);
  std::vector<double> lossss(nStaA+nStaB+nStaC);
  for (int i = 0; i < nStaA+nStaB+nStaC; i++)
  {
    lossss[i] = lossModel->GetLoss (staNodes.Get(i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    std::cout << "ciaoo: " << lossss[i] << "\n";
  }





for (int i = 0; i < nStaA; i++)
  {
    pRx[i] = txPowerA - lossModel->GetLoss (staNodes.Get(i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    std::cout << "ciaooa: " << pRx[i] << "\n";
  }
  for (int i = 0; i < nStaB; i++)
  {
    pRx[nStaA+i] = txPowerB - lossModel->GetLoss (staNodes.Get(nStaA+i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    std::cout << "ciaoob: " << pRx[nStaA+i] << "\n";
  }
  for (int i = 0; i < nStaC; i++)
  {
    pRx[nStaA+nStaB+i] = txPowerC - lossModel->GetLoss (staNodes.Get(nStaA+nStaB+i)->GetObject<MobilityModel> (), apNode.Get(0)->GetObject<MobilityModel> ());
    std::cout << "ciaooc: " << pRx[nStaA+nStaB+i] << "\n";
  }
*/




  timeNow = time(0);
  ctimeNow =ctime(&timeNow);
  std::cout << OKBLUE << "Simulation finished! Time: " << ctimeNow << ENDC;
  Simulator::Destroy ();

  /*
  std::vector<double> throughputA(nStaA);
  std::vector<double> throughputB(nStaB);
  std::vector<double> throughputC(nStaC);

  for (int i = 0; i < nStaA; i++)
  {
  	throughputA[i] = totalPacketsRxA[i] * payloadSize * 8 / (simulationTime * 1000000.0); //Mbbit/s
  	std::cout << "ThroughputA" << i+1 << " = " << throughputA[i] << " Mbit/s" << std::endl;
  	std::cout << "ReceivedA" << i+1 << " = " << totalPacketsRxA[i] << std::endl;
  	std::cout << "LossA" << i+1 << " = " << totalPacketsLossA[i] << std::endl;
  }

  for (int i = 0; i < nStaB; i++)
  {
    throughputB[i] = totalPacketsRxB[i] * payloadSize * 8 / (simulationTime * 1000000.0); //Mbbit/s
    std::cout << "ThroughputB" << i+1 << " = " << throughputB[i] << " Mbit/s" << std::endl;
    std::cout << "ReceivedB" << i+1 << " = " << totalPacketsRxB[i] << std::endl;
    std::cout << "LossB" << i+1 << " = " << totalPacketsLossB[i] << std::endl;
  }

  for (int i = 0; i < nStaC; i++)
  {
    throughputC[i] = totalPacketsRxC[i] * payloadSize * 8 / (simulationTime * 1000000.0); //Mbbit/s
    std::cout << "ThroughputC" << i+1 << " = " << throughputC[i] << " Mbit/s" << std::endl;
    std::cout << "ReceivedC" << i+1 << " = " << totalPacketsRxC[i] << std::endl;
    std::cout << "LossC" << i+1 << " = " << totalPacketsLossC[i] << std::endl;
  }

  std::cout << "Sent = " << nPacketsSent << std::endl; // added
  */

  /* Writing to file csvFileName.csv */
  std::cout << OKBLUE <<"Writing to file: " << csvFileName << ENDC << std::endl;
  // std::ofstream out (csvFileName.c_str ()); // Use it to overwrite the file
  std::ofstream out (csvFileName.c_str (), std::ios::app);
  out << "channelNumberA, channelWidthA, channelNumberB, channelWidthB, channelNumberC, channelWidthC" << std::endl;

  out << channelNumberA << "," << channelWidthA << ","
  	  << channelNumberB << "," << channelWidthB << ","
  	  << channelNumberC << "," << channelWidthC << std::endl;
  
  for (int i = 0; i < nStaA; i++)
  {
    out << dataRateA[i] << "," << x[i] << "," << y[i] << ","
    << txPackets[i] << "," << rxPackets[i] << "," << latency[i]
    << std::endl;
  }
  for (int i = 0; i < nStaB; i++)
  {
    out << dataRateB[i] << "," << x[nStaA+i] << "," << y[nStaA+i] << ","
    << txPackets[nStaA+i] << "," << rxPackets[nStaA+i] << "," << latency[nStaA+i] << std::endl;
  }
  for (int i = 0; i < nStaC; i++)
  {
    out << dataRateC[i] << "," << x[nStaA+nStaB+i] << "," << y[nStaA+nStaB+i] << ","
    << txPackets[nStaA+nStaB+i] << "," << rxPackets[nStaA+nStaB+i] << "," << latency[nStaA+nStaB+i] << std::endl;
  }

  //out << throughputA[0] << "," << throughputB[0] << std::endl;
  out.close ();

  return 0;
}