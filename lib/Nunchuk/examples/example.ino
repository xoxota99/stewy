#include <Wire.h>
#include <Nunchuk.h>


// Nunchuck for input
Nunchuk nc = Nunchuk();


void setup()
{  
  // Initialize the serial port
  Serial.begin(115200);
  
  // Initialize the Nunchuk.
  nc.begin();
}

void loop()
{
  // Read the current state
  nc.read();

  Serial.print( nc.getAccel(), DEC );
  Serial.print( "\t" );
  Serial.print( nc.getAccelX(), DEC );
  Serial.print( "\t" );
  Serial.print( nc.getAccelY(), DEC );
  Serial.print( "\t" );
  Serial.print( nc.getAccelZ(), DEC );
  Serial.print( "\n" );
  
  // Wait a short while
  delay(50);
}


