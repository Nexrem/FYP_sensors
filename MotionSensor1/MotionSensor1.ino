//#include <ThingSpeak.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 0xD4, 0x28, 0xB6, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network
 
int inputPin = 2;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status


// ThingSpeak Set up
char thingSpeakAddress[] = "api.thingspeak.com";
String thingtweetAPIKey = "4A7JNXU99X5OJGC1";
//unsigned long myChannelNumber = 404697;
//String writeAPIKey = "SJCZ65ABE049F2BF";
//const int updateThingSpeakInterval = 16 * 1000; // Time Interval in miliseconds to update ThingSpeak

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;
int messageCounter = 0;

EthernetClient client;
 
void setup() {
  pinMode(inputPin, INPUT);     // declare sensor as input
 
  Serial.begin(9600);

  
  startEthernet();

  delay(1000);

  updateTwitterStatus("Test started-");

  delay(1000);
}
 
void loop(){
  
val = digitalRead(inputPin);  // read input value
  if (val == HIGH && !client.connected()) {            // check if the input is HIGH
    if (pirState == LOW) {       // we have just turned on
      Serial.println("Motion detected!");
      updateTwitterStatus("detected motion ");
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH){       // we have just turned of
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }

  if (!client.connected() && lastConnected)
  {
    Serial.println("...disconnected.");
    Serial.println();
    
    client.stop();
  }

  if (failedCounter > 3 ) 
  {
    startEthernet();
    failedCounter = 0;
    }
  
  lastConnected = client.connected();

  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
}

void updateTwitterStatus(String tsData)
{
  if (client.connect(thingSpeakAddress, 80)&& tsData.length()>0)
  { 
    messageCounter++;
    // Create HTTP POST Data
    tsData = "api_key="+thingtweetAPIKey+"&status="+tsData+String(messageCounter);
            
    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void startEthernet()
{
  
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  
  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else
  {
    Serial.println("Arduino connected to network using DHCP");
    Serial.println();
  }
  
  delay(1000);
}

/*
void updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}
*/
