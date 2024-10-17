/*
 snippet & Descrption how to get LAN8720 proper working on KC868_A16 with platformio
 Important is to define an older ESP-IDF version in platformio.ini. i have best results with version 4.4.0, the latest arduino core 2 release
 Todo this add this entry to your Platfomio.ini  
 platform = espressif32@4.4.0 
 With ESP-IDF > 5 i get no proper result and/or fast dropped connections. Hope it helps.
*/

// Important to be defined BEFORE including ETH.h for ETH.begin() to work.
// Example RMII LAN8720 (Olimex, etc.)

#define ETH_PHY_TYPE  ETH_PHY_LAN8720
#define ETH_PHY_ADDR  0
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER -1
#define ETH_POWER_PIN  -1
#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT
//#include <Network.h>
#include <ETH.h>
#include <WiFi.h>

IPAddress local_IP(192, 168, 2, 48);
// Set your Gateway IP address
IPAddress gateway(192, 168, 2, 2);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192,168,2,2);   //optional
IPAddress secondaryDNS(8, 8, 8, 8); 
//#include <ETH.h>

bool eth_connected = false;
char lan_ip[16];

// WARNING: onEvent is called from a separate FreeRTOS task (thread)!
void onEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      // The hostname must be set after the interface is started, but needs
      // to be set before DHCP, so set it from the event handler thread.
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED: Serial.println("ETH Connected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("ETH Got IP");
      //Serial.println(ETH);
      eth_connected = true;
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default: break;
  }
}

void testClient(const char *host, uint16_t port) {
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

void lan8720_setup() {
  //Serial.begin(115200);
  Serial.println("\nLAN Setup");
  WiFi.onEvent(onEvent);
  ETH.begin(ETH_PHY_ADDR, ETH_POWER_PIN, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE); // Enable ETH
  
  //ETH.begin(ETH_PHY_TYPE,ETH_PHY_ADDR, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_POWER_PIN, ETH_CLK_MODE); // Enable ETH
  //ETH.begin();
  ETH.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  //ETH.config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000);
  delay(200);
  int xy=0;
  while(!eth_connected && xy++ < 20000) { delay(1);}
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(ETH.localIP());  
  sprintf(lan_ip,"%s", ETH.localIP().toString());
  //eth_connected=true;
}

void lan_loop() {
  if (eth_connected) {
    testClient("google.com", 80);
  }
  delay(10000);
}

/*
// Important to be defined BEFORE including ETH.h for ETH.begin() to work.
// Example RMII LAN8720 (Olimex, etc.)
#define ETH_PHY_TYPE  ETH_PHY_LAN8720
#define ETH_PHY_ADDR  0
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER -1
#define ETH_POWER_PIN  -1
//#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT
#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT

//#define ETH_PHY_POWER 17  
#include <ETH.h>
//#include <WiFi.h>
//#include <WiFiUdp.h>

//WiFiUDP Udp;                      //Create UDP object
unsigned int localUdpPort = 4196; 

IPAddress local_IP(192, 168, 2, 48);
// Set your Gateway IP address
IPAddress gateway(192, 168, 2, 2);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192,168,2,2);   //optional
IPAddress secondaryDNS(8, 8, 8, 8); 
bool eth_connected = false;

// WARNING: onEvent is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      // The hostname must be set after the interface is started, but needs
      // to be set before DHCP, so set it from the event handler thread.
      ETH.setHostname("RexA16");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED: Serial.println("ETH Connected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:
  Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case IP_EVENT_ETH_LOST_IP:
      Serial.println("ETH Lost IP");
      //eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
     // eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default: break;
  }
}

void lan8720_setup() {
  //Serial.begin(115200);
  Serial.println("\nLAN Setup");
  WiFi.onEvent(WiFiEvent);
  ETH.begin(ETH_PHY_ADDR, ETH_POWER_PIN, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE); // Enable ETH
  //ETH.begin(ETH_PHY_TYPE,ETH_PHY_ADDR, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_POWER_PIN, ETH_CLK_MODE); // Enable ETH
  //ETH.begin();
  ETH.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  //ETH.config(config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000);
  delay(200);
  while(!ETH.localIP()) delay(1);
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(ETH.localIP());  
    Udp.begin(localUdpPort);
}

*/