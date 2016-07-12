#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>

String readTime;  //stores POSIX time format
unsigned long lastEpoch = 0;
long lastMillis = 0;

//lines below are for NTP
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP udp;  // A UDP instance to let us send and receive packets over UDP

// Update these with values suitable for your network.
const char* ssid = "ASSIST";
const char* password = "akihabara@1969";
const char* mqtt_server = "192.168.10.103";

char myIpString[24];  //used to store IPAddress as array of char

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[100];
int value = 0;

char sensordata[100];                     //A 30 byte character array to hold incoming data from sensors
byte sensor_bytes_received = 0;          //We need to know how many characters bytes have been received

String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product

char payload[300];  //stores MQTT message to be send
String stringPayload; //used to mount MQTT message

int WIFI_LED = 2; //port where WiFi LED is conected on ESP8266

void setup() {
  pinMode(WIFI_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();                 //starts WiFi interface
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);   //define routine to treat MQTT received message (not used in this case)
  udp.begin(localPort);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  /* exemples of how data are received from ESP8266
  PlantsMonitor,DHT1,Humidity,%,POSIX,27.30,M,/n
  PlantsMonitor,DHT1,Temperature,F,POSIX,90.68,M,/n
  PlantsMonitor,UVsensor,UV,mw/cm^2,POSIX,-6.79,M,/n
  PlantsMonitor,SoilMoistureSensor,SoilMoisture,H,POSIX,534,M,/n
  PlantsMonitor,SoilMoistureSensor,SoilMoisture,state,POSIX,HUMID,E,/n
  */

  if (Serial.available() > 0) {  
    sensor_bytes_received = Serial.readBytesUntil(10, sensordata, 100);  //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received
    sensordata[sensor_bytes_received] = 0;                  //we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer
    sensor_string_complete = true;
    sensorstring = String(sensordata);                                
    //Serial.println(sensorstring);
    get_posix();
    //Serial.println(readTime);
  }

  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    print_EC_data();                                //then call this function 
    sensorstring = "";                                //clear the string

    for (int i=0 ; i < 100 ; i++){ //limpa array sensordata
      sensordata[i]=' ';
    }
    
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }
}


