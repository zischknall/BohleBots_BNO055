# BohleBots_BNO055

## Description
This library is intended for the BNO055 9-axis absolute orientation sensor. With this library you can use your BNO055 as a fast and simple compass. It provides functions for reading the heading of the compass and circumventing the automatic recalibration of the sensor on impacts. It was designed to be simple and faster than the Adafruit library for the BNO055.

## Functions
**`startBNO(impact, forward)`** enables high-G interrupts (impact detection) and puts the BNO055 into NDOF fusion mode. Impact accepts an integer from 0-255 and regulates the threshold for the impact detection (higher means more G). `forward` accepts a bool and forwards interrupts to the INT pin if true. **Required to use the compass (call in `setup()`)**

`getHeadingAuto(address)` outputs the current heading as an 16-bit signed integer and automatically reloads the calibration offsets stored at the specified EEPROM address if an impact occurred.

`getRLHeadingAuto(address)` outputs the current heading relative to the set reference point (from 180 to -180) as an 16-bit signed integer and automatically reloads the calibration offsets stored at the specified EEPROM address if an impact occurred.

`getHeading()` outputs the current heading as an 16-bit signed integer.

`getRoll()` outputs the current roll as an 16-bit signed integer.

`getPitch()` outputs the current pitch as an 16-bit signed integer.

`getImpact()` returns true if an high-G interrupt (impact) occurred.

`isCalibrated()` returns true if every sensors on the BNO055 has reached complete calibration.

`getCalibration()` returns all for calibration values in one byte, each taking two bytes.

`serialPrintCalibStat()` prints the current calibration levels via serial. Established serial connection required.

`saveOffsets(address)` gets current calibration offset values from the BNO055 and saves them to the specified EEPROM address. Offset data consists of 22 bytes.

`loadOffsets(address)` gets calibration offset values saved at the specified EEPROM address and writes them onto the BNO055.

`setReference()` saves the current orientation as a reference point for the relative heading.

`getRLHeading()` outputs the current heading relative to the reference point (from 180 to -180) as an 16-bit signed integer.

### Average execution times
**`getHeading()`:** 290µs **Adafruit Library:** 1130µs
