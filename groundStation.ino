#include <CanSatKit.h>

using namespace CanSatKit;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,                  // frequency in MHz
            Bandwidth_125000_Hz,    // bandwidth - check with CanSat regulations to set allowed value
            SpreadingFactor_9,      // see provided presentations to determine which setting is the best
            CodingRate_4_8);        // see provided presentations to determine which setting is the best

void setup() 
{
  SerialUSB.begin(115200);
  radio.begin();
}

void loop() 
{
  char data[256];   //empty space for received frame

  radio.receive(data);    
  
  SerialUSB.print("Received (RSSI = ");   //print signal level
  SerialUSB.print(radio.get_rssi_last());
  SerialUSB.print("): ");
  SerialUSB.print(data);    //print received message
  SerialUSB.println(" Koniec");
}