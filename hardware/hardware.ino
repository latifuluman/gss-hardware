#include "SoftwareSerial.h"

//Web Service Settings
String ssid = "OmerRecep";
String password = "Omert242490*";


//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 5;       
 
//the time when the sensor outputs a low impulse
long unsigned int lowIn;        
 
//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 1000;  // bu süreyi arttır.
 
boolean lockLow = true;
boolean takeLowTime;
boolean lockSendingRemoteServer = false; 
 
int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;
int redLed = 6;
int greenLed = 9;
int count = 0;
SoftwareSerial esp(10, 11);// RX, TX

void sendRemoteServer()
{
  manipulateLeds(HIGH,HIGH);
  String line_1 = "POST /api/sensor/add/stimulus HTTP/1.1\r\n";
  String line_2 = "Host: gss-backend.herokuapp.com\r\n";
  String line_3 = "Content-Type: application/x-www-form-urlencoded\r\n";
  String line_4 = "Content-Length: 209\r\n\r\n";
  String line_5 = "token=";
  String line_6 = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.";
  String line_7 = "eyJnZXJpYXRyaWNfaWQiOiI1YmE4ZjNlYzFiN";
  String line_8 = "GY0YTI0Y2NiMzcyYzIiLCJzZW5zb3JfbG9jYXR";
  String line_9 = "pb25faWQiOiI1YmE4ZjdiZGI2ODJiZTFiMWMxM";
  String line_10= "jNlNzMifQ.iEfYFrSSBv84-D7eOH";
  String line_11 = "_W2Y30C5D_yMTp7jISfbMzmks";
  count += line_1.length()+line_2.length()+line_3.length()+line_4.length()+line_5.length()+line_6.length()+line_7.length()+line_8.length()+line_9.length()+line_10.length()+line_11.length();

 
SEND_REMOTE:
    String cmd= "AT+CIPSTART=\"TCP\",\"gss-backend.herokuapp.com\",80";
    Serial.println("Remote Server Started");
    esp.println(cmd);//start a TCP connection.
    delay(1000);
    if(esp.find("OK"))
    {
      manipulateLeds(LOW,HIGH);
      Serial.println("TCP connection ready");
      delay(1000);
      manipulateLeds(LOW,LOW);
      lockSendingRemoteServer = false;
    }
    else
    {
      manipulateLeds(HIGH,LOW);
      Serial.println("TCP connection failed");
      delay(500);
      manipulateLeds(LOW,LOW);
      goto SEND_REMOTE;
    }
    
  delay(1000);

  esp.println("AT+CIPSEND=364");
  delay(1000);
  count = 0;
  if(esp.find(">")) 
  {
    manipulateLeds(LOW,HIGH); 
    Serial.println("Sending.."); 
    esp.print(line_1);
    esp.print(line_2);
    esp.print(line_3);
    esp.print(line_4);
    esp.print(line_5);
    esp.print(line_6);
    esp.print(line_7);
    esp.print(line_8);
    esp.print(line_9);
    esp.print(line_10);
    esp.println(line_11);

    esp.println("AT+CIPCLOSE");
    delay(1000);
    
    if(esp.find("OK"))
    {
      manipulateLeds(LOW,HIGH); 
      Serial.println("Connection Closed");
      delay(500);
      manipulateLeds(LOW,LOW);
      delay(500);
    }
    else
    {
      manipulateLeds(HIGH,LOW);
      Serial.println("Connection Closing Failed");
      delay(1000);
      manipulateLeds(LOW,LOW);
    }
  }
}
void reset() 
{
  esp.println("AT");
  delay(1000);
  if(esp.find("OK"))
  { 
    Serial.println("Module Ready");
    manipulateLeds(LOW,HIGH);
  }
  else
  {
    Serial.println("Module is not ready");
    manipulateLeds(HIGH,LOW);
  }

  delay(2000);
  
}
void connectWifi() 
{
  reset();
  esp.println("AT+CIPMUX=0");
  esp.println("AT+CWMODE=1");
CONNECT_START:
  Serial.println("Connect WIFI Starting");
  String cmd = "AT+CWJAP=\"" +ssid+"\",\"" + password + "\"";
  Serial.println(cmd);
  esp.println(cmd);
  delay(4000);
  if(esp.find("OK"))
  { 
    Serial.println("Connected!"); 
    manipulateLeds(LOW,HIGH);
  }
  else
  {
    Serial.println("Not Connected");
    manipulateLeds(HIGH,LOW);
    goto CONNECT_START;
  }
}
void manipulateLeds(int redStatus,int greenStatus)
{
  digitalWrite(redLed, redStatus);
  digitalWrite(greenLed, greenStatus);
}

void setup()
{
  esp.begin(115200);
  Serial.begin(9600);
  connectWifi();
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  digitalWrite(pirPin, LOW);

  manipulateLeds(LOW,HIGH);
  delay(50);
  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    manipulateLeds(HIGH,HIGH);
    delay(500);
    manipulateLeds(LOW,LOW);
    delay(500);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  manipulateLeds(LOW,HIGH);
  delay(50);
}

void loop()
{ 
  if(lockSendingRemoteServer == false)
  {
    //Serial.println("Lock Sending remote false");
    if(digitalRead(pirPin) == HIGH)
    {
      digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
      if(lockLow)
      { 
        //makes sure we wait for a transition to LOW before any further output is made:
        lockLow = false;           
        Serial.println("---");
        Serial.print("motion detected at ");
        manipulateLeds(LOW,LOW);
        Serial.print(millis()/1000);
        Serial.println(" sec");
        delay(50);
      }        
      takeLowTime = true;
    }
    if(digitalRead(pirPin) == LOW)
    {      
      digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state
      if(takeLowTime)
      {
          lowIn = millis();          //save the time of the transition from high to LOW
          takeLowTime = false;       //make sure this is only done at the start of a LOW phase
      }
      //if the sensor is low for more than the given pause,
      //we assume that no more motion is going to happen
      if(!lockLow && millis() - lowIn > pause)
      { 
        //makes sure this block of code is only executed again after
        //a new motion sequence has been detected
        lockLow = true;
        // Jwtyi gönder
        lockSendingRemoteServer = true;
        sendRemoteServer();
        Serial.print("motion ended at ");      //output
        Serial.print((millis() - pause)/1000);
        Serial.println(" sec");
        delay(5000);
        manipulateLeds(LOW,HIGH);
      }
    }
  }
}
