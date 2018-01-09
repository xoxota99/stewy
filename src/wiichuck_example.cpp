#include <Wire.h>
//#include <i2c_t3.h>
#include "Nunchuk.h"
#define LED 13  //LED pin.

//From https://github.com/hughpyle/machinesalem-arduino-libs/tree/master/nunchuk

// Nunchuck for input
Nunchuk nc = Nunchuk();


void setup()
{  
  //Turn on the LED (basic power indicator)
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  
  // Initialize the serial port
  Serial.begin(115200);
  
  // Initialize the Nunchuk.
//  nc.initPower(A2,A3);
  nc.begin();
}

void loop()
{
  // Read the current state
  nc.read();

  nc.

  Serial.print(nc.isOk() ? "OK\t":"N/A\t");
  Serial.print("Buttons (C/Z): ");
  Serial.print( nc.getButtonC() );
  Serial.print( "/" );
  Serial.print( nc.getButtonZ() );
  Serial.print( "\t" );
  
  Serial.print("Joystick (X/Y): ");
  Serial.print( nc.getJoyX(), DEC );
  Serial.print( "/" );
  Serial.print( nc.getJoyY(), DEC );
  Serial.print( "\t" );
  
  Serial.print("Tilt (X/Y/X): ");
  Serial.print( nc.getTiltX(), DEC );
  Serial.print( "/" );
  Serial.print( nc.getTiltY(), DEC );
  Serial.print( "/" );
  Serial.print( nc.getTiltZ(), DEC );
  Serial.print( "\t" );

  Serial.print("Accel (All/X/Y/X): ");
  Serial.print( nc.getAccel(), DEC );
  Serial.print( "/" );
  Serial.print( nc.getAccelX(), DEC );
  Serial.print( "/" );
  Serial.print( nc.getAccelY(), DEC );
  Serial.print( "/" );
  Serial.print( nc.getAccelZ(), DEC );
  Serial.println();
  
  // Wait a short while
  delay(50);
}


