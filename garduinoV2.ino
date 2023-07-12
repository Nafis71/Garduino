#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLmxyInna3"
#define BLYNK_TEMPLATE_NAME "CONTROL LED"
#define BLYNK_AUTH_TOKEN "3hpvtK-9oNGfj-YFNTQhtUH2aCXt-UYU"
#define DHTPIN D4     //dht input pin
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Rupicious 1311"; //need to change
char pass[] = "123456789"; //need to change
SoftwareSerial ard(D7,D8); //Rx Tx
DHT dht(DHTPIN,DHT11);
BlynkTimer timer; 
int soil_map_low = 900;
int soil_map_high = 240;
int water_map_low = 130;
int water_map_high = 332;
int pirStatus = 0;
int autoPilot = 0;


BLYNK_WRITE(V13)
{
  int value = param.asInt();
  autoPilotE(value);
}

BLYNK_WRITE(V2)    //humidifier automation
{
  double humidData = param.asDouble();
  if(humidData < 50.00 || humidData < 70.00)
  {
    if(autoPilot == 1)
    {
      humidifier(1);
    }
  }
  else
  {
    if(autoPilot == 1)
    {
      humidifier(0);
    }
  }
}

BLYNK_WRITE(V5)  //manual heater
{
  int value = param.asInt();
  if(value == 0)
  {
    if(autoPilot == 1)
    {
      eventAutoPilot();
    }
    else
    {
      Blynk.virtualWrite(V5, 0);
      digitalWrite(D1,LOW);
      Blynk.virtualWrite(V11,"Turned On"); 
    }
  }
  else
  {
     if(autoPilot == 1)
    {
      eventAutoPilot();
    }
    else
    {   
      Blynk.virtualWrite(V5, 1);
      digitalWrite(D1,HIGH);
      Blynk.virtualWrite(V11,"Turned Off"); 
    }
}
}

BLYNK_WRITE(V6)  //manual humidifier
{
  int value = param.asInt();
  if(value == 0)
  {
    if(autoPilot == 1)
    {
      eventAutoPilot();
    }
    else
    {
     humidifier(1);
    }
   
  }
  else
  {
    if(autoPilot == 1)
    {
      eventAutoPilot();
    }
    else
    {
      humidifier(0);        
    }
  }
}



BLYNK_WRITE(V0) //manual pump
{
  int value = param.asInt();
  if(value == 0)
  {
    digitalWrite(D3,LOW);

  }
  else
  {
    digitalWrite(D3,HIGH);

  }

}

BLYNK_WRITE(V9)
{
  int value = param.asInt();
  if(value == 1)
  {
    pirStatus = 1; //turning on PIR
  }  
  else
  {
    pirStatus = 0; //turning off PIR
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(D5,INPUT);
  pinMode(D6,INPUT);
  digitalWrite(D1,HIGH);
  digitalWrite(D2,HIGH);
  digitalWrite(D3,HIGH);
  dht.begin();
  ard.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
   timer.setInterval(1000L, sendTempHumid);
  timer.setInterval(1000L, soilMoisture); 
  timer.setInterval(400L,waterLevel);
  timer.setInterval(50L,pir);
}

void humidifier(int value)
{
  if(value == 1)
  {
    digitalWrite(D2,LOW); //humdifier turned on
    Blynk.virtualWrite(V6, 0);
    Blynk.virtualWrite(V12,"Turned On");    
  }
  else
  {
    digitalWrite(D2,HIGH); //humdifier turned off
    Blynk.virtualWrite(V6, 1);
    Blynk.virtualWrite(V12,"Turned Off");
  }
}
void sendTempHumid()
{
   float temp = dht.readTemperature();
  float humid = dht.readHumidity();
  if (isnan(humid) || isnan(temp))                                     // Checking sensor working
  {                                   
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Blynk.virtualWrite(V1,temp);
  Blynk.virtualWrite(V2,humid);
    if(temp <26.00)
  {
    if(autoPilot == 1)
    {
     digitalWrite(D1,LOW); //heater turned on
     Blynk.virtualWrite(V5, 0);
     Blynk.virtualWrite(V11,"Turned On"); 
    }
 
  }
  else
  {
    if(autoPilot == 1)
    {
      digitalWrite(D1,HIGH); //heater turned off
      Blynk.virtualWrite(V5, 1);
      
      Blynk.virtualWrite(V11,"Turned Off");
    }
  }
    if(humid < 60.00 || humid < 70.00)
  {
    if(autoPilot == 1)
    {
      digitalWrite(D2,LOW); //heater turned on
      Blynk.virtualWrite(V5, 0);
      Blynk.virtualWrite(V12,"Turned On"); 
    }    
  }
  else
  {
    if(autoPilot == 1)
    {
      digitalWrite(D2,HIGH); //heater turned off
      Blynk.virtualWrite(V5, 0);
      Blynk.virtualWrite(V12,"Turned Off"); 
    }
  }
}

void soilMoisture()
{
  int moisture = analogRead(A0);
  int percentage = map(moisture, soil_map_low, soil_map_high, 0, 100);
  Blynk.virtualWrite(V4, percentage);
   if(percentage < 21)
    { 
      if(autoPilot == 1)
      {
        Blynk.virtualWrite(V7, "Critical");
        Blynk.logEvent("soil_moisture_critical");
        Blynk.virtualWrite(V0, 0);
        digitalWrite(D3,LOW);   
       
      
      }
      else
      {
         Blynk.virtualWrite(V7, "Critical");
         Blynk.logEvent("soil_moisture_critical");          
      }
    }
    else if (percentage < 41 && percentage >= 21)
    {
       Blynk.virtualWrite(V7, "Moderate");          
       Blynk.logEvent("soil_moisture_moderate"); 
    }
    else
    {
      if(autoPilot == 1)
      {
         Blynk.virtualWrite(V7, "Good");
          Blynk.virtualWrite(V0, 1);
          digitalWrite(D3,HIGH); //turning off motor    
      }
      else
      {
        Blynk.virtualWrite(V7, "Good");
      }
    }
}
void waterLevel()
{
  int level = 0;
  while(ard.available()>0)
  {
  level = ard.parseInt();
  if(ard.read()=='\n')
  {
      Serial.print(level);
      Serial.print("\n");
  }
  }
  int percentage = map(level, water_map_low, water_map_high, 0, 100);
  Blynk.virtualWrite(V3, percentage);
  if(percentage <20 )
  {
    Blynk.virtualWrite(V8, "Empty");

 
  }
  else if(percentage >= 20 && percentage <=40)
  {
    Blynk.virtualWrite(V8, "Low");  
  }
  else if(percentage >= 41 && percentage <=60)
  {
    Blynk.virtualWrite(V8, "Half");
  }
  else if(percentage >= 81 && percentage <=100)
  {
    Blynk.virtualWrite(V8, "Full");
  }
}
void pir()
{
  if(pirStatus == 0)
  {
    int read = digitalRead(D5);
    if(read)
    {
      Blynk.virtualWrite(V10, "Intrusion detected!");
      Blynk.logEvent("intrusion");
    }
    else
    {
      Blynk.virtualWrite(V10, "Scanning...");
    }
  }
  else
  {
    Blynk.virtualWrite(V10, "Sensor is turned off");
  }
}
void autoPilotE(int value)
{
  if(value == 1)
  {
    autoPilot = 1;
    Blynk.virtualWrite(V14,"Engaged!");  
  }
  else
  {
    autoPilot = 0;  
    Blynk.virtualWrite(V14,"Disengaged!");  
  }
}
void eventAutoPilot()
{
  Blynk.logEvent("auto_pilot");
}
BLYNK_CONNECTED() {
    Blynk.syncVirtual(V13);
    Blynk.syncVirtual(V5);
}
void loop()
{
  Blynk.run();
  timer.run();
  
}
