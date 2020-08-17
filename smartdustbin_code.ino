
#include <HX711.h>
#include <Servo.h>
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
SoftwareSerial serial_connection(6,7); //RX=pin 10, TX=pin 11
TinyGPSPlus gps;
#define DT A0
#define SCK A1
#define trigPin D1
#define echoPin D2

#define sw 2
long duration;
int distance;
long sample=0;
float val=0;
long count=0;
int servoPin1 = 3; 
int servoPin2 = 5; 
// Create a servo object 
Servo Servo2; 
Servo Servo1;

unsigned long readCount(void)
{
  unsigned long Count;
  unsigned char i;
  pinMode(DT, OUTPUT);
  digitalWrite(DT,HIGH);
  digitalWrite(SCK,LOW);
  Count=0;
  pinMode(DT, INPUT);
  while(digitalRead(DT));
  for (i=0;i<24;i++)
  {
    digitalWrite(SCK,HIGH);
    Count=Count<<1;
    digitalWrite(SCK,LOW);
    if(digitalRead(DT)) 
    Count++;
  }
  digitalWrite(SCK,HIGH);
  Count=Count^0x800000;
  digitalWrite(SCK,LOW);
  return(Count);
}

void setup()
{
  Serial.begin(9600);
  serial_connection.begin(9600);
  Serial.println("GPS Start");
  pinMode(SCK, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.print("    Weight ");
  lcd.setCursor(0,1);
  lcd.print(" Measurement ");
  delay(1000);
  lcd.clear();
  calibrate();
  Servo1.attach(servoPin1);
  Servo2.attach(servoPin2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop()
{
    digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(50);
  count= readCount();
  int w=(((count-sample)/val)-2*((count-sample)/val));
  Serial.print("weight:");
  Serial.print((int)w);
  Serial.println("g");
  lcd.setCursor(0,0);
  lcd.print("Weight            ");
  lcd.setCursor(0,1);
  lcd.print(w);
  lcd.print("g             ");
  Servo1.write(0);
  Servo2.write(0);
  if (w>200)
  {

   Servo1.write(90); 

   Servo2.write(180); 
   delay(3000);
   
   
  } 

  if(digitalRead(sw)==0)
  {
    val=0;
    sample=0;
    w=0;
    count=0;
    calibrate();
  }
}

void calibrate()
{
    lcd.clear();
  lcd.print("Calibrating...");
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  for(int i=0;i<100;i++)
  {
    count=readCount();
    sample+=count;
    Serial.println(count);
  }
  sample/=100;
  Serial.print("Avg:");
  Serial.println(sample);
  lcd.clear();
  lcd.print("Put 100g & wait");
  count=0;
  while(count<1000)
  {
    count=readCount();
    count=sample-count;
    Serial.println(count);
  }
  lcd.clear();
  lcd.print("Please Wait....");
  delay(2000);
  for(int i=0;i<100;i++)
  {
    count=readCount();
    val+=sample-count;
    Serial.println(sample-count);
  }
  val=val/100.0;
  val=val/100.0;      
  lcd.clear();
   while(serial_connection.available())
  {
    gps.encode(serial_connection.read());
  }
  if(gps.location.isUpdated())
  {
    Serial.println("Satellite Count:");
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
    Serial.println(gps.location.lng(), 6);
    Serial.println("Speed MPH:");
    Serial.println(gps.speed.mph());
    Serial.println("Altitude Feet:");
    Serial.println(gps.altitude.feet());
    Serial.println("");
  }
}
