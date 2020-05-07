#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/gnuplot.h"
//#include "ns3/wifi/model/dca-txop.h"

#define PI 3.14159265


using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("ee597proj");


int main (int argc, char *argv[])
{
  bool shortGuardInterval = false;
  uint32_t nWifi = 50;//20 stations implemted randomly
  uint32_t nAp = 1;//only 1 Acess Point
  double simutime = 50;
  uint32_t cwmin = 127; //
  uint32_t cwmax = 1023; //


  

  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("nAp","Access point",nAp);
  cmd.AddValue ("simutime", "simulation time", simutime);
  cmd.AddValue ("cwmin", "Minimum contention window size", cwmin);
  cmd.AddValue ("cwmax", "Maximum contention window size", cwmax);
  cmd.Parse (argc, argv);

   // Time::SetResolution (Time::NS);
 // LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
 //  LogComponentEnable ("UdpServer", LOG_LEVEL_ALL);


  //disable RTS and CTS all the time
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("999999"));
  //disable fragmentation
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold",StringValue ("99999990"));

  Config::SetDefault ("ns3::WifiRemoteStationManager::MaxSsrc", UintegerValue (10000)); //backoff phase for RTS；
  Config::SetDefault ("ns3::WifiRemoteStationManager::MaxSlrc", UintegerValue (10000)); //backoff phase for packet (using 8 it stay at 1023 twice as per standard)
 
  //Config::SetDefault ("ns3::Txop::MinCw", UintegerValue (cwmin)); //set min contention window
  //Config::SetDefault ("ns3::Txop::MaxCw", UintegerValue (cwmax)); //set max contention window
  //Ptr<Txop>m_b = CreateObject<Txop>();
  //m_b->SetMinCw(cwmin);
  //m_b->SetMaxCw(cwmax);










  if (true) {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO); 
  }
  uint32_t payloadsize = 1023;
       
       

    //create a number of “station” nodes as specified
    //also create access point for all the stations to transmit the packet
          
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode;
  wifiApNode.Create (1);   //only a AP
  

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
    
  phy.SetChannel (channel.Create ());

  
  phy.Set ("ShortGuardEnabled", BooleanValue (shortGuardInterval));
  


  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);//choose wifi standard
    
  StringValue datarate = StringValue ("OfdmRate54Mbps"); // set up transmission rate
  StringValue Datarate = StringValue ("OfdmRate6Mbps");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", datarate, "ControlMode", Datarate);
  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns3-80211a");
  //STA nodes
  mac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);
  //AP station

  mac.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));

  NetDeviceContainer apDevice;
  apDevice = wifi.Install (phy, mac, wifiApNode);
  
  for (uint16_t i=0; i<=nWifi-1; i++)
  {
     Ptr<Node> node = wifiStaNodes.Get(i);
     Ptr<NetDevice> dev = node->GetDevice(0);
     Ptr<WifiNetDevice> wifi_dev = DynamicCast<WifiNetDevice>(dev);
     Ptr<WifiMac> mac1 = wifi_dev->GetMac();
     PointerValue ptr;

     mac1->GetAttribute("Txop", ptr);
     Ptr<Txop> dca = ptr.Get<Txop>();

     dca->SetMinCw(cwmin);
     dca->SetMaxCw(cwmax);

/*
     Ptr<EdcaParameterSet> edca;
     mac1->GetAttribute("EdcaParameterSet", ptr);
     edca = ptr.Get<EdcaParameterSet>();

     edca->SetVoCWmin(cwmin);
     edca->SetVoCWmax(cwmax);

     edca->SetViCWmin(cwmin);
     edca->SetViCWmax(cwmax);

     edca->SetBeCWmin(cwmin);
     edca->SetBeCWmax(cwmax);

     edca->SetBkCWmin(cwmin);
     edca->SetBkCWmax(cwmax);
*/
   }



  // now we need to set up mobility
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

  positionAlloc->Add (Vector (0.0, 0.0, 0.0));

  float rho=0.01; //radius

  for (double i=0; i<nWifi; i++)
  {
    /*https://github.com/ns3-hust/IEEE80211MAC/blob/master/NS3仿真验证/infrastructure.cc
     */
      
      
      double theta = i*2*PI/nWifi;
      positionAlloc->Add (Vector (rho * cos(theta), rho * sin(theta), 0.0));
  }
  mobility.SetPositionAllocator (positionAlloc);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.Install (wifiApNode);
  mobility.Install (wifiStaNodes);

  /* Internet stack*/
  InternetStackHelper stack;//协议栈
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("192.168.1.0", "255.255.255.0");
    /*
  Ipv4InterfaceContainer staNodeInterface;
  Ipv4InterfaceContainer apNodeInterface;

  staNodeInterface = address.Assign (staDevices);
  apNodeInterface = address.Assign (apDevice);
     */
   
  Ipv4InterfaceContainer ApInterface = address.Assign(apDevice);
  Ipv4InterfaceContainer StaInterface = address.Assign(staDevices);

  //UDP flow
  uint16_t port = 8000;
  UdpServerHelper server (port); 
  ApplicationContainer serverApp;
  serverApp = server.Install (wifiApNode.Get (0));
  serverApp.Start (Seconds (0.0));
  serverApp.Stop (Seconds (simutime + 1));

  UdpClientHelper client (ApInterface.GetAddress (0), port);
  client.SetAttribute ("MaxPackets", UintegerValue (4294967295u));//Data packets
  client.SetAttribute ("Interval", TimeValue (Time ("0.001")));//Packets arriving interval
  client.SetAttribute ("PacketSize", UintegerValue (payloadsize));//length of packets
  ApplicationContainer clientApps[nWifi];
  for (uint16_t i=0; i<=nWifi-1; i++)
  {
      clientApps[i] = client.Install (wifiStaNodes.Get (i));
      clientApps[i].Start (Seconds (1.0));
      clientApps[i].Stop (Seconds (simutime + 1));
  }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//Flow monitor
// Ptr<FlowMonitor> flowMonitor;
// FlowMonitorHelper flowHelper;
// flowMonitor = flowHelper.InstallAll();
//
  FlowMonitorHelper flowhelper;
  Ptr<FlowMonitor> monitor = flowhelper.InstallAll();
  Simulator::Stop (Seconds (simutime + 1));
     

  Simulator::Run ();

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowhelper.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  double totalthroughput = 0.0;
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
  {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);


      //https://groups.google.com/forum/#!topic/ns-3-users/35LR0PhEoHk by Mahmud Islam
      
      std::cout<<"Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress<<"\n";
      std::cout<<"Tx Packets = " << iter->second.txPackets<<"\n";
      std::cout<<"Rx Packets = " << iter->second.rxPackets<<"\n";
      std::cout<<"Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024 /1024  << " Mbps\n";
      totalthroughput = totalthroughput + iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024 /1024;
  }
  double Averagethr = totalthroughput/nWifi;
  std::cout <<"Station number:" <<nWifi<<std::endl;
  std::cout <<"Maximum contention window:" <<cwmax<<std::endl;
  std::cout <<"Minimum contention window:" <<cwmin<<std::endl;
  std::cout <<"total throughput:" <<totalthroughput<< "Mbps\n"<<std::endl;
  std::cout <<"average throughput:" <<Averagethr<< "Mbps\n"<<std::endl;
  Simulator::Destroy ();
 
  return 0;

}
