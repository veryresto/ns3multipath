/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ProxyScriptExample");

int
main (int argc, char *argv[])
{
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer server, proxy, router, mobile;
  server.Create (1);
  proxy.Create (1);
  router.Create (2);
  mobile.Create (1);

  NodeContainer serverProxy = NodeContainer(server.Get(0), proxy.Get(0));
  NodeContainer proxyRouterUp = NodeContainer(proxy.Get(0), router.Get(0));
  NodeContainer proxyRouterDown = NodeContainer(proxy.Get(0), router.Get(1));
  NodeContainer routerMobileUp = NodeContainer(router.Get(0), mobile.Get(0));
  NodeContainer routerMobileDown = NodeContainer(router.Get(1), mobile.Get(0));

  PointToPointHelper p2pServerProxy;
  p2pServerProxy.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
  p2pServerProxy.SetChannelAttribute("Delay", StringValue ("2ms"));

  PointToPointHelper p2pProxyRouterUp;
  p2pProxyRouterUp.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
  p2pProxyRouterUp.SetChannelAttribute("Delay", StringValue ("2ms"));

  PointToPointHelper p2pProxyRouterDown;
  p2pProxyRouterDown.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
  p2pProxyRouterDown.SetChannelAttribute("Delay", StringValue ("2ms"));

  PointToPointHelper p2pRouterMobileUp;
  p2pRouterMobileUp.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
  p2pRouterMobileUp.SetChannelAttribute("Delay", StringValue ("2ms"));

  PointToPointHelper p2pRouterMobileDown;
  p2pRouterMobileDown.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
  p2pRouterMobileDown.SetChannelAttribute("Delay", StringValue ("2ms"));

  NetDeviceContainer chanServerProxy;
  chanServerProxy = p2pServerProxy.Install(serverProxy);

  NetDeviceContainer chanProxyRouterUp;
  chanProxyRouterUp = p2pProxyRouterUp.Install(proxyRouterUp);

  NetDeviceContainer chanProxyRouterDown;
  chanProxyRouterDown = p2pProxyRouterDown.Install(proxyRouterDown);

  NetDeviceContainer chanRouterMobileUp;
  chanRouterMobileUp = p2pRouterMobileUp.Install(routerMobileUp);

  NetDeviceContainer chanRouterMobileDown;
  chanRouterMobileDown = p2pRouterMobileDown.Install(routerMobileDown);

  InternetStackHelper stack;
  stack.Install (server);
  stack.Install (proxy);
  stack.Install (router);
  stack.Install (mobile);

  Ipv4AddressHelper address;

  address.SetBase ("192.168.5.0", "255.255.255.0");
  Ipv4InterfaceContainer IPServerProxy = address.Assign (chanServerProxy);

  address.SetBase ("192.168.17.0", "255.255.255.0");
  Ipv4InterfaceContainer IPProxyRouterUp = address.Assign (chanProxyRouterUp);

  address.SetBase ("192.168.18.0", "255.255.255.0");
  Ipv4InterfaceContainer IPProxyRouterDown = address.Assign (chanProxyRouterDown);

  address.SetBase ("192.168.60.0", "255.255.255.0");
  Ipv4InterfaceContainer IPRouterMobileUp = address.Assign (chanRouterMobileUp);

  address.SetBase ("192.168.90.0", "255.255.255.0");
  Ipv4InterfaceContainer IPRouterMobileDown = address.Assign (chanRouterMobileDown);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
  p2pServerProxy.EnablePcapAll("UDP");

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (mobile.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (IPRouterMobileUp.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (server.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
