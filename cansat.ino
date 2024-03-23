#include <SPI.h>
#include <SD.h>
#include<cmath>
#include <CanSatKit.h>

using namespace CanSatKit;

BMP280 bmp;

const int lm35_pin = A0;

const int moisture1_pin = A2; //na prawo od złącza programowania
const int moisture2_pin = A3; //na lewo od złącza programowania

const int chipSelect = 11;
int counter = 1;
bool led_state = false;
const int led_pin = 13;

//float del;
//float pt, pp=0;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);

Frame frame;

float lm35_raw_to_temperature(int raw)
{
  float voltage = raw * 3.3 / (std::pow(2,12));
  float temperature = 100.0 * voltage;

  return temperature;
}

void setup() 
{
  pinMode(moisture1_pin,INPUT);
  pinMode(moisture2_pin,INPUT);
  SerialUSB.begin(9600);

  while (!Serial) {
    ; 
  }

  SerialUSB.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) 
  {
    SerialUSB.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  SerialUSB.println("card initialized.");

  if(!bmp.begin())
  {
    SerialUSB.println("BMP init failed!");
    while(1);
  }
  else
  {
    SerialUSB.println("BMP init success!");
  }

  bmp.setOversampling(16);    //ustawienie rozdzielczosci czujnika cisnienia
  analogReadResolution(12);   //ustawienie rozdzielczosci czujnika temperatury

  pinMode(led_pin, OUTPUT);

  radio.begin();
}

void loop() 
{
  double T, P;
  int raw=analogRead(lm35_pin);
  float m1=analogRead(moisture1_pin);
  float m2=analogRead(moisture2_pin);
  float temperature=lm35_raw_to_temperature(raw);     
  String dataString = "";
  
  bmp.measureTemperatureAndPressure(T,P);
  dataString+= String(counter);
  dataString+= (". ");
  dataString+= String(temperature);
  dataString+= (" deg C, ");
  dataString+= String(P);
  dataString+= (" hPa, ");
  dataString+= String(T);
  dataString+= (" deg C");

  //pt=float(P);
  //del=abs(pt-pp);

  //if(del<=0.1)
  //{
    dataString+= (", ");
    dataString+= String(m1);
    dataString+= (" %, ");
    dataString+= String(m2);
    dataString+= (" %");
  //}
              

  File dataFile = SD.open("dane.txt", FILE_WRITE);
  if (dataFile) 
  {
    dataFile.println(dataString);
    dataFile.flush();
    dataFile.close();
    //SerialUSB.println(dataString);
  }
  else 
  {
    SerialUSB.println("error opening datalog.txt");
  }

  digitalWrite(led_pin, led_state);
  led_state = !led_state;

  frame.print(counter);
  frame.print(". ");
  frame.print(" T1: ");
  frame.print(temperature);
  frame.print(" deg C ");
  frame.print(" P: ");
  frame.print(P);
  frame.print(" hPa ");
  frame.print(" T2: ");
  frame.print(T);
  frame.print(" deg C ");

  //delay(500);

  //if(del<=0.1)
  //{
    //frame.print(counter);
    //frame.print(". ");
    frame.print(" MP: ");
    frame.print(m1);
    //frame.print(" %");
    frame.print(" ML: ");
    frame.print(m2);
    //frame.print(" %");
  //}

  radio.transmit(frame);
  SerialUSB.println(frame);
  frame.clear();
  
  counter++;
  //pp=pt;

  delay(1000);
}
