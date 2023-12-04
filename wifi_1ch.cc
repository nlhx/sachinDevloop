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
 *         |     <- 1 AP
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

NS_LOG_COMPONENT_DEFINE ("wifi_1ch");


// function to create a new C/S application
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


// main function
int main (int argc, char *argv[])
{
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
  int nStaA = 3;                        // number of stations A
  int nStaB = 100;                      // number of stations B
  int nStaC = 5;                        // number of stations C
  // Network
  int channelNumber = 50; // channel number
  int channelWidth = 160; // 20, 40, 80 or 160 MHz
  int mcs = 6;            // from 0 to 11 (-1 = unset value)
  int gi = 800;           // 800, 1600 or 3200 ns
  int txPower = 18;       // dBm
  std::string dataRateA_old = "1Kb/s";
  std::string dataRateB_old = "5Kb/s";
  std::string dataRateC_old = "2Kb/s";

  CommandLine cmd;
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("seed", "Seed", seed);
  cmd.AddValue ("csvFileName", "Name of the .csv file", csvFileName);
  cmd.AddValue ("band", "AC_5, AX_2.4 or AX_5", band);
  cmd.AddValue ("phyModel", "PHY layer model", phyModel);
  cmd.AddValue ("constantMcs", "0 Minstrel or 1 constant", constantMcs);
  cmd.AddValue ("enablePcap", "Enable/disable pcap file generation", enablePcap);
  cmd.AddValue ("nStaA", "Number Stations A", nStaA);
  cmd.AddValue ("nStaB", "Number Stations B", nStaB);
  cmd.AddValue ("nStaC", "Number Stations C", nStaC);
  // Network
  cmd.AddValue ("channelNumber", "Channel number", channelNumber);
  cmd.AddValue ("channelWidth", "Channel width", channelWidth);
  cmd.AddValue ("mcs", "if set, limit testing to a specific MCS", mcs);
  cmd.AddValue ("gi", "Guard interval", gi);
  cmd.AddValue ("txPower", "Transmission power", txPower);
  // Network A
  cmd.AddValue ("dataRateA_old", "Data rate A", dataRateA_old);
  // Network B
  cmd.AddValue ("dataRateB_old", "Data rate B", dataRateB_old);
  // Network C
  cmd.AddValue ("dataRateC_old", "Data rate C", dataRateC_old);

  cmd.Parse (argc, argv);

  // Set the PRNG seed
  RngSeedManager::SetSeed (seed);

  // Set random throughput for each flow in the the 3 slices
  Ptr<UniformRandomVariable> dataRateA_ptr = CreateObject<UniformRandomVariable> ();
  dataRateA_ptr->SetAttribute ("Min", DoubleValue (80));
  dataRateA_ptr->SetAttribute ("Max", DoubleValue (101));
  std::vector<int> dataRateA(nStaA);
  std::vector<std::string> dataRateA_str(nStaA);
  for (int i = 0; i < nStaA; i++)
  {
  	dataRateA[i] = (int) dataRateA_ptr->GetValue();
    dataRateA_str[i] = std::to_string(dataRateA[i]) + "Mb/s";
  }
  Ptr<UniformRandomVariable> dataRateB_ptr = CreateObject<UniformRandomVariable> ();
  dataRateB_ptr->SetAttribute ("Min", DoubleValue (30));
  dataRateB_ptr->SetAttribute ("Max", DoubleValue (51));
  std::vector<int> dataRateB(nStaB);
  std::vector<std::string> dataRateB_str(nStaB);
  for (int i = 0; i < nStaB; i++)
  {
    dataRateB[i] = (int) dataRateB_ptr->GetValue();
    dataRateB_str[i] = std::to_string(dataRateB[i]) + "Kb/s";
  }
  Ptr<UniformRandomVariable> dataRateC_ptr = CreateObject<UniformRandomVariable> ();
  dataRateC_ptr->SetAttribute ("Min", DoubleValue (20));
  dataRateC_ptr->SetAttribute ("Max", DoubleValue (41));
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

  // Create nStaA + nStaB + nStaC STAs node objects and 1 AP node object
  NodeContainer staNodes;
  staNodes.Create (nStaA + nStaB + nStaC);
  NodeContainer apNode;
  apNode.Create (1);

  // Create a phy helper
  SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default ();
  YansWifiPhyHelper yansPhy = YansWifiPhyHelper::Default ();
  
  if (phyModel == "spectrum")
  {
  	// Create the channel
  	Ptr<MultiModelSpectrumChannel> channel = CreateObject<MultiModelSpectrumChannel> ();

  	Ptr<HybridBuildingsPropagationLossModel> lossModel = CreateObject<HybridBuildingsPropagationLossModel> ();
  	channel->AddPropagationLossModel (lossModel);
  	Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
  	channel->SetPropagationDelayModel (delayModel);
  	spectrumPhy.SetErrorRateModel ("ns3::NistErrorRateModel");
  	spectrumPhy.SetChannel (channel);
  	spectrumPhy.Set ("TxPowerStart", DoubleValue (txPower));
  	spectrumPhy.Set ("TxPowerEnd", DoubleValue (txPower));
  }
  else if (phyModel == "yans")
  {
    // Create the channel
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
    yansPhy.SetChannel (channel.Create ());
  	yansPhy.Set ("TxPowerStart", DoubleValue (txPower));
  	yansPhy.Set ("TxPowerEnd", DoubleValue (txPower));
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
      oss << "HeMcs" << mcs;
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
  NetDeviceContainer apDeviceA;
  Ssid ssid;

  if (phyModel == "spectrum")
  {
  	// Network
  	ssid = Ssid ("networkA");
  	spectrumPhy.Set ("ChannelNumber", UintegerValue (channelNumber));
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaA; i++)
      staDeviceA[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i));
    for (int i = 0; i < nStaB; i++)
      staDeviceB[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA));
    for (int i = 0; i < nStaC; i++)
      staDeviceC[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA + nStaB));
  	mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
  	apDeviceA = wifi.Install (spectrumPhy, mac, apNode.Get(0));
  }
  else if (phyModel == "yans")
  {
    // Network
    ssid = Ssid ("networkA");
    yansPhy.Set ("ChannelNumber", UintegerValue (channelNumber));
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid));
    for (int i = 0; i < nStaA; i++)
      staDeviceA[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i));
    for (int i = 0; i < nStaB; i++)
      staDeviceB[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA));
    for (int i = 0; i < nStaC; i++)
      staDeviceC[i] = wifi.Install (spectrumPhy, mac, staNodes.Get (i + nStaA + nStaB));
    mac.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
    apDeviceA = wifi.Install (yansPhy, mac, apNode.Get(0));
  }
  else
  {
    std::cout << "Wrong phyModel value!" << std::endl;
    return 0;
  }

  // Set channel width
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth",
               UintegerValue (channelWidth));

  // Set guard interval
  Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
               TimeValue (NanoSeconds (gi)));

  // Set rts cts
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

  // Set mobility model
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

  // Internet stack
  InternetStackHelper stack;
  stack.Install (apNode);
  stack.Install (staNodes);

  Ipv4AddressHelper address;
  address.SetBase ("192.168.1.0", "255.255.255.0");
  std::vector<Ipv4InterfaceContainer> staInterfaceA(nStaA);
  for (int i = 0; i < nStaA; i++)
    staInterfaceA[i] = address.Assign (staDeviceA[i]);
  //Ipv4InterfaceContainer apInterfaceA = address.Assign (apDeviceA);

  //address.SetBase ("192.168.2.0", "255.255.255.0");
  std::vector<Ipv4InterfaceContainer> staInterfaceB(nStaB);
  for (int i = 0; i < nStaB; i++)
    staInterfaceB[i] = address.Assign (staDeviceB[i]);
  //Ipv4InterfaceContainer apInterfaceB = address.Assign (apDeviceB);

  //address.SetBase ("192.168.3.0", "255.255.255.0");
  std::vector<Ipv4InterfaceContainer> staInterfaceC(nStaC);
  for (int i = 0; i < nStaC; i++)
    staInterfaceC[i] = address.Assign (staDeviceC[i]);
  //Ipv4InterfaceContainer apInterfaceC = address.Assign (apDeviceC);
  Ipv4InterfaceContainer apInterfaceA = address.Assign (apDeviceA);

  // Flow monitor
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  // Setting applications
  uint16_t index = 1;
  std::vector<ApplicationContainer> clientAppA(nStaA), serverAppA(nStaA);
  std::vector<ApplicationContainer> clientAppB(nStaB), serverAppB(nStaB);
  std::vector<ApplicationContainer> clientAppC(nStaC), serverAppC(nStaC);

  for (int i = 0; i < nStaA; i++)
    new_application (index, payloadSize, simulationTime, staNodes, apNode,
                     dataRateA_str[i], apInterfaceA, clientAppA[i], serverAppA[i]);

  for (int i = 0; i < nStaB; i++)
    new_application (index, payloadSize, simulationTime, staNodes, apNode,
                     dataRateB_str[i], apInterfaceA, clientAppB[i], serverAppB[i]);

  for (int i = 0; i < nStaC; i++)
    new_application (index, payloadSize, simulationTime, staNodes, apNode,
                     dataRateC_str[i], apInterfaceA, clientAppC[i], serverAppC[i]);

  if (enablePcap)
  {
  	if (phyModel == "spectrum")
  	{
  		spectrumPhy.EnablePcap ("AP_A", apDeviceA.Get (0));
  		spectrumPhy.EnablePcap ("STA_A", staDeviceA[0].Get (0));
  		//spectrumPhy.EnablePcap ("AP_B", apDeviceB.Get (0));
  		spectrumPhy.EnablePcap ("STA_B", staDeviceB[0].Get (0));
      //spectrumPhy.EnablePcap ("AP_C", apDeviceC.Get (0));
      spectrumPhy.EnablePcap ("STA_C", staDeviceC[0].Get (0));
  	}
  	else if (phyModel == "yans")
  	{
  		yansPhy.EnablePcap ("AP_A", apDeviceA.Get (0));
  		yansPhy.EnablePcap ("STA_A", staDeviceA[0].Get (0));
  		//yansPhy.EnablePcap ("AP_B", apDeviceB.Get (0));
  		yansPhy.EnablePcap ("STA_B", staDeviceB[0].Get (0));
      //yansPhy.EnablePcap ("AP_C", apDeviceC.Get (0));
      yansPhy.EnablePcap ("STA_C", staDeviceC[0].Get (0));
  	}
  	else
  	{
  		std::cout << "Wrong phyModel value!" << std::endl;
  		return 0;
  	}
  }

  Simulator::Stop (Seconds (simulationTime + 2));

  time_t timeNow = time(0);
  char* ctimeNow =ctime(&timeNow);
  std::cout << OKBLUE <<"Simulation started!  Time: " << ctimeNow << ENDC;

  Simulator::Run ();

  // Activate/deactivate the histograms and the per-probe detailed stats
  std::cout << OKBLUE <<"Writing to file: FlowMonitorFile.xml"<< ENDC << std::endl;
  flowMonitor->SerializeToXmlFile("FlowMonitorFile.xml", false, false);

  // Show results
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

  timeNow = time(0);
  ctimeNow =ctime(&timeNow);
  std::cout << OKBLUE << "Simulation finished! Time: " << ctimeNow << ENDC;
  Simulator::Destroy ();

  // Writing to file csvFileName.csv
  std::cout << OKBLUE <<"Writing to file: " << csvFileName << ENDC << std::endl;
  // std::ofstream out (csvFileName.c_str ()); // Use it to overwrite the file
  std::ofstream out (csvFileName.c_str (), std::ios::app);
  
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

  out.close ();

  return 0;
}