#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <HTTPUpdateServer.h>
#include <WiFi.h>
#include "MS5837.h"
#include <Wire.h>
MS5837 sensor;
int floo = 7;
float chuan = 3.0;
float h = 0; // do cao

float t; int k = 0;
float fist[2];
WiFiUDP UDP;
char packet[2000];
#define UDP_PORT 2205
IPAddress myIP;
const char* ssid     = "SSA_LAB";
const char* password = "12345687990";
const char* ssid1     = "Wifi free";
const char* password1 = "999999999";
const char* host = "SSALAB-datalogger";
MDNSResponder mdns;
WebServer server(80);
HTTPUpdateServer httpUpdater;


bool client_connect = false;
#include <TaskScheduler.h>

Scheduler runner;
void task_loger();
Task TareaLED(50, TASK_FOREVER, &task_loger);//chu ky la 10ms tuong duong voi 100 Hz
int clientA = 0;
int test;
#include <Wire.h>
const uint8_t led = 2;

//=============================================================================//

int Sensor();
void setup() {
  // put your setup code here, to run once:
  

  Serial.begin(115200);
  Serial.println("start..........");

  runner.addTask(TareaLED);
  TareaLED.enable();

  AccessPoint();
  delay(2000);
  UDP.begin(UDP_PORT);

  Wire.begin();

    while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }
}
int tmp;
int main(float fist, float second){
    tmp = fist - second;
    if(tmp == chuan){
      return 1;
    }
    if(tmp == -(chuan)){
      return -1;
    }
    return 0;
}
//=======================================================================//
int i =0;
void loop(){
  sensor.read();
  task_loger();
  Serial.print(sensor.pressure());
  Serial.print("\t");
  float currentAltitude = (1-pow((sensor.pressure()/1013.25),.190284))*145366.45*.3048;
  if(i==0){
    fist[i] = currentAltitude;
  }
  i++;
  int floorChange = main(currentAltitude, fist[0]);
    floo += floorChange;
    Serial.print("loop");
    Serial.print(floo);
    Serial.print("\t");
    if (floorChange != 0) 
    {
    fist[0] = currentAltitude;
    }
    runner.execute();
    Serial.print("out_loop");
    Serial.print(floo);
    Serial.print("\t");
}

//==========================================================================
void task_loger(){
  
  if(client_connect)
    { 
      Serial.print("send");
      Serial.println(floo);


      String S2="[{ \"floo\":\""+String(floo)+"\"   }]";
      char charBuf[S2.length()+1];
      S2.toCharArray(charBuf, S2.length()+1);
    // Send return packet
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.println(String(charBuf));
      UDP.endPacket();
    }
    sendUDP();

}
//=============================================================================
void sendUDP()
{
    int packetSize = UDP.parsePacket();
    if (packetSize) 
    {
      int len = UDP.read(packet, packetSize+1);
      if (len > 0)
      {
        packet[len] = '\0';
      }
      if(Serial)
      {
        Serial.print(F("Packet received: "));
        Serial.println(packet);
      }
      String dataget ="";
      for (int i =1;i<len;i++)
      {
        dataget+=packet[i];
      }
      if (packet[0]=='S'&&packet[1]=='T'&&packet[2]=='O'&&packet[3]=='P')
      {
         client_connect = false;

      }
      if (packet[0]=='S'&&packet[1]=='T'&&packet[2]=='A'&&packet[3]=='R'&&packet[4]=='T')
      {
                client_connect = true;  
                if(client_connect)
                {    
                String S2="[{ \"toadox\":\""+String (floo)+"\"   }]";
                char charBuf[S2.length()+1];
                S2.toCharArray(charBuf, S2.length()+1);
                // Send return packet
                UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
                UDP.println(String(charBuf));
                UDP.endPacket();
                }
      }
      }
}       


void AccessPoint()
{
  WiFi.mode(WIFI_AP);

    while (WiFi.softAP(ssid1, password1) == false) 
  {
   digitalWrite(2,HIGH);
    //Serial.print(".");
    delay(5);
    digitalWrite(led,LOW);
    delay(50);
  }
  myIP = WiFi.softAPIP();  
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(myIP);
  if (mdns.begin(host)) Serial.println("MDNS ok"); //cai này dùng cho chế độ AP
  httpUpdater.setup(&server);
  server.begin();
}
//=====================================================================================




