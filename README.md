# BohleBots_BNO055

## Description
This library is intended for the BNO055 9-axis absolute orientation sensor. With this library you can use your BNO055 as a fast and simple compass. It provides functions for reading the heading of the compass and circumventing the automatic recalibration of the sensor on impacts. It was designed to be simple and faster than the Adafruit library for the BNO055.

## Functions
**`startBNO(impact, forward)`** enables you to use the compass by enabling High-G interrupt and putting it into NDOF fusion output mode. Impact accepts an integer from 0-255 and regulates the threshold for the impact detection. Forward accepts a bool and enables interrupt pin forwarding when true.

`getHeadingAuto(address)` outputs the current heading as an 16-bit signed integer and automatically reloads the calibration offsets stored in the specified eeprom address if an impact occurred.

`getRLHeadingAuto(address)` outputs the current heading relative to the reference point (from 180 to -180) as an 16-bit signed integer and automatically reloads the calibration offsets stored in the specified eeprom address if an impact occurred.

`getHeading()` outputs the current heading as an 16-bit signed integer.

`getImpact()` returns true if an High-G interrupt (impact) occurred.

`isCalibrated()` returns true if everything on the BNO055 is on the highest calibration level.

`serialPrintCalibStat()` prints the current calibration levels via serial. Established serial connection required.

`saveOffsets(address)` gets current offset data from BNO055 and saves it to EEPROM at the specified address. Offset data consists of 22 bytes.

`loadOffsets(address)` gets offset data saved in EEPROM at the specified address and writes it to the BNO055.

`setReference()` saves the current point as a reference for the relative heading.

`getRLHeading()` outputs current heading relative to the reference point (from 180 to -180) as an 16-bit signed integer.

### Average execution times
**`getHeading()`:** 290µs **Adafruit Library:** 1130µs
