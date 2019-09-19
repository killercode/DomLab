/*
Programming the sketch for the ESP8266:

DIP SWITCH
 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 |OFF|OFF|OFF|OFF|ON |ON |OFF|N/A|

RX/TX SWITCH TX00
*/

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

// handler for MQTT messages
void callback(char* topic, byte* payload, unsigned int length);

// WIFI Setup
WiFiClient wifiClient;
#define MQTT_SERVER "192.168.1.96"  //you MQTT IP Address
const char* ssid = "MEO-KILLERCODE2";
const char* password = "ericeira";
String inString = "";

// MQTT Channel Subscription
char const* switchTopic1 = "/house/switch1/";
char const* switchTopic2 = "/house/switch2/";
char const* switchTopic3 = "/house/switch3/";
char const* switchTopic4 = "/house/switch4/";
char const* switchTopic5 = "/house/switch5/";
char const* switchTopic6 = "/house/switch6/";
char const* switchTopic7 = "/house/switch7/";
char const* switchTopic8 = "/house/switch8/";

PubSubClient client(MQTT_SERVER, 1883, callback, wifiClient);

//generate unique name from MAC addr
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5)
    {
      result += ':';
    }
  }
  return result;
}

// If wifi connection is down, reconnect
void reconnectWifi() {

  //attempt to connect to the wifi if connection is lost
  if(WiFi.status() != WL_CONNECTED)
  {
    //debug printing
    Serial.print("!:Connecting to " + String(ssid) + ";");
    

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
    }

    //print out some more debug once connected
    Serial.print("!:WiFi connected;");  
  }
}

void reconnectMQTT()
{
  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if(WiFi.status() == WL_CONNECTED)
  {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) 
    {
      delay(500);
      Serial.print("!:Attempting MQTT connection...;");

      // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
      clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);

      //if connected, subscribe to the topic(s) we want to be notified about
      //EJ: Delete "mqtt_username", and "mqtt_password" here if you are not using any 
      if (client.connect((char*) clientName.c_str(),"local-user", "mypw")) 
      {  
        //EJ: Update accordingly with your MQTT account 
        Serial.print("!:MQTT Connected;");
        client.subscribe(switchTopic1);
        client.subscribe(switchTopic2);
        client.subscribe(switchTopic3);
        client.subscribe(switchTopic4);
        client.subscribe(switchTopic5);
        client.subscribe(switchTopic6);
        client.subscribe(switchTopic7);
        client.subscribe(switchTopic8);
        //EJ: Do not forget to replicate the above line if you will have more than the above number of relay switches
      }

      //otherwise print failed for debugging
      else
      {
        Serial.print("!:Failed;");
      }
    }
  }
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3])  ;
}

void setup() 
{
  //start the serial line for debugging
  Serial.begin(115200);
  delay(100);

  //start wifi subsystem
  WiFi.begin(ssid, password);
  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnectWifi();
  
  //wait a bit before starting the main loop
  delay(2000);
  ArduinoOTA.onStart([]() 
  {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) 
    {
      type = "sketch";
    } 
    else 
    { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.print("!:Start updating " + type + ";");
  });
  
  ArduinoOTA.onEnd([]() 
  {
    Serial.print("!:End;");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) 
  {
    Serial.printf("!:Progress: %u%%\r;", (progress / (total / 100)) );
  });
  
  ArduinoOTA.onError([](ota_error_t error) 
  {
    Serial.printf("!:Error[%u]: ;", error);
    if (error == OTA_AUTH_ERROR) 
    {
      Serial.print("!:Auth Failed;");
    } 
    else if (error == OTA_BEGIN_ERROR) 
    {
      Serial.print("!:Begin Failed;");
    } 
    else if (error == OTA_CONNECT_ERROR) 
    {
      Serial.print("!:Connect Failed;");
    } 
    else if (error == OTA_RECEIVE_ERROR) 
    {
      Serial.print("!:Receive Failed;");
    } 
    else if (error == OTA_END_ERROR) 
    {
      Serial.print("!:End Failed;");
    }
  });
  
  ArduinoOTA.begin();
       

  reconnectMQTT();
  Serial.print("!:Ready;");
  Serial.print("!:" + IpAddress2String(WiFi.localIP()) + ";");
}

void loop()
{
 ArduinoOTA.handle();

  //reconnect if connection is lost
  if(WiFi.status() != 3)
  {
    reconnectWifi();
  }
  if (!client.connected() && WiFi.status() == 3) 
  {
    reconnectMQTT();
  }

  //maintain MQTT connection
  client.loop();

  // keep device aware of possible firmware updates
 

  //MUST delay to allow ESP8266 WIFI functions to run
  delay(10); 

  while (Serial.available()) 
  {  
    char inChar = Serial.read(); 

    if ((inChar != '\n') && (inChar != '\r'))
    {
      inString += inChar;
    }

    if (inString.length() > 255)
    {
      Serial.flush();
      inString = "";
    }

    if (inChar == ';') 
    {
      
      if ( inString.indexOf('>') >= 0 )
      {
        
        int cstart =1;
        int cend = inString.indexOf(':');
        int csize = cend - cstart;

        int sstart = inString.indexOf(':')+1;
        int send = inString.indexOf(';');
        int ssize = send - sstart;

        char cbuff[csize];
        char sbuff[ssize];
        
        String channel = inString.substring(cstart, cend);
        String action = inString.substring(sstart, send);
        
        
        
        channel.toCharArray(cbuff, csize+1);
        action.toCharArray(sbuff, ssize+1);
        
        Serial.print("!:CHANNEL - " + String(cbuff) + ";");
        delay(10);
        Serial.print("!:ACTION - " + String(sbuff) + ";");
        delay(10);
        client.publish(cbuff, sbuff);
      }
      else
      {
        Serial.print("!ESP received a wrong command: [" + inString + "];");
        Serial.flush();
      }
      inString = "";
    }
    
  }
  
}

void callback(char* topic, byte* payload, unsigned int length) 
{

  //convert topic to string to make it easier to work with
  String topicStr = topic; 
  //EJ: Note:  the "topic" value gets overwritten everytime it receives confirmation (callback) message from MQTT

  //Print out some debugging info
  //digitalWrite(switchPin1, HIGH);
  Serial.print("<" + topicStr + ":" + String((char)payload[0]) + ";" );
  //client.publish("/house/switchConfirm1/", "1");
  
}