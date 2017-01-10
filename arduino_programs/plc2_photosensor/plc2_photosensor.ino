// National Autonomous University of Mexico 
// Paulo Contreras Flores
// paulo.contreras.flores@gmail.com

#include <Ethernet2.h>
#include <SPI.h>
#include <Mudbus.h>
#include <avr/pgmspace.h>
#define FOTO_PIN  0  // 10ktherm & 10k resistor as divider.

Mudbus Mb;

//Lamps
int Out1 = 2;
int Out2 = 3;
int Out3 = 4;

void setup()
{
  byte mac[] = { 0x00, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Define MAc address
  byte ip[] = { 192, 168, 1, 120 };                      // Define IP address
  byte gateway[] = { 192, 168, 1, 254 };
  byte subnet[] = { 255, 255, 255, 0 };                 // Define Subnet mask
  Ethernet.begin(mac, ip, subnet);                      // Assign MAC, IP, and subnet mask

  pinMode(Out1, OUTPUT);               // assign Pin 1 as Output
  pinMode(Out2, OUTPUT);               // assign Pin 2 as Output
  pinMode(Out3, OUTPUT);               // assign Pin 3 as Output

}


void loop()
{
  Mb.Run(); // start MbTcp subroutine

  // fotoresistor
  double Temp = analogRead(0);  // Read sensor
  Mb.R[0] = Temp;

  // Digital OUT 1 (as  Modbus Coil 1)
  if   (Mb.C[0] == HIGH) {
    digitalWrite(Out1, HIGH);
  }
  else {
    digitalWrite(Out1, LOW);
  }

  // Digital OUT 2 (as  Modbus Coil 2)
  if   (Mb.C[1] == HIGH) {
    digitalWrite(Out2, HIGH);
  }
  else {
    digitalWrite(Out2, LOW);
  }

  // Digital OUT 3 (as  Modbus Coil 3)
  if   (Mb.C[2] == HIGH) {
    digitalWrite(Out3, HIGH);
  }
  else {
    digitalWrite(Out3, LOW);
  }
}
