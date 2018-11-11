/*
 * 
 *    This example sketch shows you how to use the library with the getHeadingAuto(address) function, which detects impacts and loads offsets values from the provided eeprom address automatically.
 *    Keep in mind, that you still need to manually save correct offset values into eeprom with saveOffsets(address).
 *
 */

#include <BohleBots_BNO055.h>
#include <Wire.h> //include wire.h for i2c communication

BNO bno;  //create bno from the Class BNO

void setup()
{  
  Wire.begin(); //Start I2C communication as master
  Wire.setClock(400000);  //set I2C to fast mode at 400kHz
  Serial.begin(115200); //Begin Serial Communication for debugging
  while(!Serial);  //wait for a serial connection being fully established
  bno.startBNO(200, false); //enables high_g interrupts and puts the compass into fusion output mode NDOF. First parameter controls the threshold for the interrupt (0-255), the second one enables INT pin forwarding.
  while(!bno.isCalibrated())  //wait until everything is fully calibrated once....
  {
  bno.serialPrintCalibStat(); //print the current calibration levels via serial
  delay(1000);
  }
  Serial.println("Fully Calibrated!");  
  bno.saveOffsets(100);  //....then save the Offsets into the eeprom at the specified address 
  Serial.println("Type \"setRef\" to set a reference point!");
}

void loop() 
{
  Serial.println(bno.getRLHeadingAuto(100), DEC);  //give out the current heading relative to the reference point (this function automatically reloads the offset values stored at the eeprom address provided)
  delay(1000);
}

void serialEvent() // this gets called if a serial message is receivec
{
  if(Serial.readString()=="setRef") bno.setReference(); //read the message into a string, if its "setRef" call setReference(), which sets the current point in space as a reference point
}
