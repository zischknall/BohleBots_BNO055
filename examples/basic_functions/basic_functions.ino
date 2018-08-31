#include <BohleBots_BNO055.h>
#include <Wire.h> //include wire.h for i2c communication

BNO bno;  //create bno from the Class BNO

void setup()
{  
  Wire.begin(); //Start I2C communication as master
  Wire.setClock(400000);  //set I2C to fast mode at 400kHz
  Serial.begin(115200); //Begin Serial Communication for debugging
  while(!Serial);	//wait for a serial connection being fully established
  bno.startBNO(200, false); //enables high_g interrupts and puts the compass into fusion output mode NDOF. First parameter controls the threshold for the interrupt (0-255), the second one enables Int pin forwarding.
  while(!bno.isCalibrated())	//Wait until everything is fully calibrated once....
  {
  bno.serialPrintCalibStat();
  delay(1000);
  }
  Serial.println("Fully Calibrated!");  
  bno.saveOffsets(100);  //....then save the Offsets into the eeprom at the specified address 
}

void loop() 
{
  if(bno.getImpact())
  {
    Serial.println("Impact Detected!");
    bno.loadOffsets(100);  //if a high_g interrupt occured reload the calibration values from the eeprom
  }
  Serial.println(bno.getHeading(), DEC);  //give out the current heading
  delay(1000);
}
