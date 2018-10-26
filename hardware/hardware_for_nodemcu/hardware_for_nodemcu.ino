#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
const char* ssid = "OmerRecep";
const char* password = "Omert242490*";
String token = "token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJnZXJpYXRyaWNfaWQiOiI1YmE4ZjNlYzFiNGY0YTI0Y2NiMzcyYzIiLCJzZW5zb3JfbG9jYXRpb25faWQiOiI1YmE4ZjdiZGI2ODJiZTFiMWMxMjNlNzMifQ.iEfYFrSSBv84-D7eOH_W2Y30C5D_yMTp7jISfbMzmks";

String command = "";
void setup () {
  Serial.begin(9600);
  delay(500);
  Serial.println("Setup is starting");
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting..");
  }
  Serial.println("Setup is ending");
}
 
void loop() {
  if (WiFi.status() == WL_CONNECTED) 
  { 
      command = Serial.readString();
      if(command.length() > 10)
      {
        Serial.println("command is greater than 10");
        Serial.println(command);
        delay(100);
      }
      
      if(command == "<COMMAND_BY_MASTER>:SEND2SERVER\r\n")
      {
        Serial.println("Command is okey");
        HTTPClient http;  
        http.begin("http://gss-backend.herokuapp.com/api/sensor/add/stimulus");  //Specify request destination
        http.addHeader("Content-Type","application/x-www-form-urlencoded");
        int httpCode = http.POST(token);
        Serial.println("Http Code :"+String(httpCode));                                                              
        if (httpCode > 0) 
        { 
          String payload = http.getString();   //Get the request response payload
          Serial.println(payload);                     //Print the response payload
        }
        http.end();   //Close connection
      }
    
  }
}
