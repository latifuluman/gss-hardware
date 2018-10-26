int calibrationTime = 30;       
long unsigned int lowIn;        
long unsigned int pause = 10000; 
boolean lockLow = true;
boolean takeLowTime; 
#define pirPin 12
#define ledPin 9
 
 
/////////////////////////////
//SETUP
void setup()
{
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);
 
  //give the sensor some time to calibrate
  //Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  //Serial.println(" done");
  //Serial.println("SENSOR ACTIVE");
  delay(50);
  Serial.println();
}
void loop()
{ 
  if(digitalRead(pirPin) == HIGH)
  {
    digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
    if(lockLow)
    { 
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;           
      //Serial.println("---");
      //Serial.print("motion detected at ");
      //Serial.print(millis()/1000);
      //Serial.println(" sec");
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
      Serial.println("<COMMAND_BY_MASTER>:SEND2SERVER");
      delay(100);
      //Serial.println("motion ended at ");      //output
      //Serial.print((millis() - pause)/1000);
      //Serial.println(" sec");
    }
  }
}
