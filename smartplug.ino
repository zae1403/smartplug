#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <WidgetTimeInput.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>

PZEM004Tv30 pzem(12,14); // Software Serial pin 12 (RX) & 14 (TX)

char auth[] = "k1AmJ0srEulNCszvWQSiEoITJkRJsGJf";
char ssid[] = "XII SIJA 2";
char pass[] = "smkn26jkt";

String currentDate;
String currentTime;
String startTime;
String stopTime;
BlynkTimer timer;
WidgetRTC rtc;

BLYNK_WRITE(V100) {
  TimeInputParam t(param);

  // Process start time

  if (t.hasStartTime())
  {
    startTime = String(t.getStartHour())  + ":" + t.getStartMinute()  + ":" + t.getStartSecond();
    Serial.println("Start: " + startTime);

  }
  else if (t.isStartSunrise())
  {
    Serial.println("Start at sunrise");
  }
  else if (t.isStartSunset())
  {
    Serial.println("Start at sunset");
  }
  else
  {
    // Do nothing
  }

  // Process stop time

  if (t.hasStopTime())
  {
    stopTime = String(t.getStopHour()) + ":" + t.getStopMinute() + ":" + t.getStopSecond();

    Serial.println("Stop: " + stopTime);
  }
  else if (t.isStopSunrise())
  {
    Serial.println("Stop at sunrise");
  }
  else if (t.isStopSunset())
  {
    Serial.println("Stop at sunset");
  }
  else
  {
    // Do nothing: no stop time was set
  }

  // Process timezone
  // Timezone is already added to start/stop time

  Serial.println(String("Time zone: ") + t.getTZ());

  // Get timezone offset (in seconds)
  Serial.println(String("Time zone offset: ") + t.getTZ_Offset());

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)

  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      Serial.println(String("Day ") + i + " is selected");
    }
  }


  Serial.println();
}

void timerBlynk()
{
  //TEMPO
  currentTime = String(hour()) + ":" + minute() + ":" + second();
  currentDate = String(day()) + " " + month() + " " + year();

  // Send time to the App
  Blynk.virtualWrite(V101, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V102, currentDate);

  delay(1000);
  if (currentTime == startTime){
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(13, HIGH);
    digitalWrite(15, HIGH);
}
else if (currentTime == stopTime){
    digitalWrite(0, LOW);
    digitalWrite(2, LOW);
    digitalWrite(13, LOW);
    digitalWrite(15, LOW);
    }
}

void pzemSensor()
{
  float voltage = pzem.voltage();
   if(voltage != NAN){
       Serial.print("Voltage: ");
       Serial.print(voltage);
       Serial.println("V");
       Blynk.virtualWrite(V11, voltage);
   } else {
       Serial.println("Error reading voltage");
   }

   float current = pzem.current();
   if(current != NAN){
       Serial.print("Current: ");
       Serial.print(current);
       Serial.println("A");
       Blynk.virtualWrite(V12, current);
   } else {
       Serial.println("Error reading current");
   }

   float power = pzem.power();
   if(current != NAN){
       Serial.print("Power: ");
       Serial.print(power);
       Serial.println("W");
       Blynk.virtualWrite(V13, power);
   } else {
       Serial.println("Error reading power");
   }

   float energy = pzem.energy();
   if(current != NAN){
       Serial.print("Energy: ");
       Serial.print(energy,3);
       Serial.println("kWh");
       Blynk.virtualWrite(V14, energy);
   } else {
       Serial.println("Error reading energy");
   }

   float frequency = pzem.frequency();
   if(current != NAN){
       Serial.print("Frequency: ");
       Serial.print(frequency, 1);
       Serial.println("Hz");
   } else {
       Serial.println("Error reading frequency");
   }

   float pf = pzem.pf();
   if(current != NAN){
       Serial.print("PF: ");
       Serial.println(pf);
   } else {
       Serial.println("Error reading power factor");
   }

  int price = energy*1467.28;
  Blynk.virtualWrite(V20, price);

   Serial.println();
   delay(2000);
}

void setup() {
   Serial.begin(115200);
   Blynk.begin(auth,ssid, pass);
  rtc.begin();
  pinMode(0, OUTPUT); //SAIDAS
  pinMode(2, OUTPUT); //SAIDAS
  pinMode(13, OUTPUT); //SAIDAS
  pinMode(15, OUTPUT); //SAIDAS
}

void loop() {
  Blynk.run();
  timer.run();
  timerBlynk();
  pzemSensor();
}
