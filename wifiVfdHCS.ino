#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Ticker.h>

#include "VFD.h"

void displayTime();
void displayDate();
void getTemperature();

const char *ssid     = "ssid";
const char *password = "password";
char outStr[13];

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

OneWire oneWire(D2);
DallasTemperature Temp(&oneWire);

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};



//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

#define INTERVALLO 4000
#define DATE_DELAY 20000     // Delay before date is displayed (loop exectutions)
unsigned long t0, dt;
uint16_t loopCounter = 0;
float tc = 0;
float decimal = 0;


void setup()
{

  Serial.begin(115200);
  Vfd.init();
  Vfd.write("Connecting  ");    // display

  WiFi.begin(ssid, password);

   
   while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print ( "." );
  }

  Serial.println();
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());

  
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);

  Temp.begin();

  t0 = millis();
}


void loop()
{

dt = millis() - t0; 
    
    if ( dt >= DATE_DELAY ) {
       t0 = millis();
       
       do{
       dt = millis() - t0;
       displayDate(); 
       }while (dt < INTERVALLO);
       
       t0 = millis();
       }
       
    
    displayTime(); 
 
}



void displayTime(){
  
  timeClient.update();
 
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
 
  sprintf(outStr, "  %02d-%02d-%02d  ", currentHour, currentMinute, currentSecond);
  Vfd.write(outStr);
         
}

void displayDate(){
  
    timeClient.update();

    time_t epochTime = timeClient.getEpochTime();
  //Serial.println(timeClient.getFormattedTime());

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  String weekDay = weekDays[timeClient.getDay()];
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = months[currentMonth-1];
  int currentYear = (ptm->tm_year+1900)%100;
  getTemperature();
  

  sprintf(outStr,"%s,%02d   ",weekDay, monthDay);
  Vfd.write(outStr); 
  delay(2000);
 

  sprintf(outStr,"%s,%02d ",currentMonthName,currentYear);
  Vfd.write(outStr);
  delay(2000);

  sprintf(outStr,"Temp,%04.2f  ",tc);
  Vfd.write(outStr);
  delay(2000);
}

void getTemperature()
{
    Temp.requestTemperatures();
    tc = Temp.getTempCByIndex(0);
    decimal = tc*100;
}
