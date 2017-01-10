// National Autonomous University of Mexico 
// Paulo Contreras Flores
// paulo.contreras.flores@gmail.com

#include <Ethernet2.h>
#include <SPI.h>
#include <Mudbus.h>
#include <avr/pgmspace.h>


Mudbus Mb;

//Lamps
int Out2 = 2;
int Out3 = 3;
int Out4 = 4;
int Out5 = 5;
int Out6 = 6;
int Out7 = 7;

void setup() {
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };  // Define MAC address
  byte ip[] = { 192, 168, 1, 110 };                     // Define IP address
  byte gateway[] = { 192, 168, 1, 254 };
  byte subnet[] = { 255, 255, 255, 0 };                 // Define Subnet mask
  Ethernet.begin(mac, ip, gateway, subnet);             // Assign MAC, IP, and subnet mask

  pinMode(Out2, OUTPUT);               // assign Pin 2 as Output
  pinMode(Out3, OUTPUT);               // assign Pin 3 as Output
  pinMode(Out4, OUTPUT);               // assign Pin 4 as Output
  pinMode(Out5, OUTPUT);               // assign Pin 5 as Output
  pinMode(Out6, OUTPUT);               // assign Pin 6 as Output
  pinMode(Out7, OUTPUT);               // assign Pin 7 as Output



}

void loop() {
  
  Mb.Run(); // start MbTcp subroutine


  // Digital OUT 2 (as  Modbus Coil 1)
  if   (Mb.C[0] == HIGH) {
    digitalWrite(Out2, HIGH);
  }
  else {
    digitalWrite(Out2, LOW);
  }


  // Digital OUT 3 (as  Modbus Coil 2)
  if   (Mb.C[1] == HIGH) {
    digitalWrite(Out3, HIGH);
  }
  else {
    digitalWrite(Out3, LOW);
  }

  // Digital OUT 4 (as  Modbus Coil 3)
  if   (Mb.C[2] == HIGH) {
    digitalWrite(Out4, HIGH);
  }
  else {
    digitalWrite(Out4, LOW);
  }

    // Digital OUT 5 (as  Modbus Coil 4)
  if   (Mb.C[3] == HIGH) {
    digitalWrite(Out5, HIGH);
  }
  else {
    digitalWrite(Out5, LOW);
  }

    // Digital OUT 6 (as  Modbus Coil 5)
  if   (Mb.C[4] == HIGH) {
    digitalWrite(Out6, HIGH);
  }
  else {
    digitalWrite(Out6, LOW);
  }

  // Digital OUT 7 (as  Modbus Coil 6)
  if   (Mb.C[5] == HIGH) {
    digitalWrite(Out7, HIGH);
  }
  else {
    digitalWrite(Out7, LOW);
  }
}
