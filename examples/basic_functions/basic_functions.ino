#include <BohleBots_BNO055.h>
#include <Wire.h> //include wire.h for i2c communication

BNO bno;  //create bno from the Class BNO

void setup()
{  
  Wire.begin(); //Start I2C communication as master
  Wire.setClock(400000);  //set I2C to fast mode at 400kHz
  Serial.begin(115200); //Begin Serial Communication for debugging
  while(!Serial);	//wait for a serial connection being fully established
  bno.startBNO(200, false); //enables high_g interrupts and puts the compass into fusion output mode NDOF. First parameter controls the threshold for the interrupt (0-255), the second one enables INT pin forwarding.
  while(!bno.isCalibrated())	//wait until everything is fully calibrated once....
  {
  bno.serialPrintCalibStat();	//print the current calibration levels via serial
  delay(1000);
  }
  Serial.println("Fully Calibrated!");  
  bno.saveOffsets(100);  //....then save the Offsets into the eeprom at the specified address 
}

void loop() 
{
  if(bno.getImpact()) //check if an high_g event occured (impact)
  {
    Serial.println("Impact Detected!");
    bno.loadOffsets(100);  //if yes load the calibration values saved in eeprom onto the bno
    while(bno.getHeading()==1);	//discard the first output from the bno because its most likely junk
  }
  Serial.println(bno.getHeading(), DEC);  //give out the current heading
  delay(1000);
}
