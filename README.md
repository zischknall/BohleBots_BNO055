# BohleBots_BNO055

## Description
This library is intended for the BNO055 9-axis absolute orientation sensor. This library makes using the BNO055 as a fast and simple compass possible. It provides a functions for reading the heading and the heading relative to a point. It also provides functions for detecting an impact as well as saving and loading sensor offsets. 

## Public Functions
`getHeading()` outputs the current heading as a 16-bit signed integer.

`getImpact()` returns true if a High-G interrupt occurred.

`isCalibrated()` returns true if everything on the BNO055 is on the highest calibration level.

`serialPrintCalibStat()` prints the current calibration levels via serial. Established serial connection required.

`saveOffsets(address)` gets current offset data from BNO055 and saves it to EEPROM at the specified address. Offset data consists of 22 bytes.

`loadOffsets(address)` gets offset data saved in EEPROM at the specified address and writes it to the BNO055.

**REQUIRED** `startBNO(impact, forward)` enables you to use the compass by enabling High-G interrupt and putting it into NDOF fusion output mode. Impact accepts an integer from 0-255 and regulates the threshold for the impact detection. Forward accepts a bool and enables interrupt pin forwarding when true.

## New with v1.1

### New Functions
`setReference()` saves the current heading as a reference for the relative heading

`getRLHeading()` outputs current heading relative to the reference point (from 180 to -180) as a 16-bit signed integer

### Faster average execution times
`getImpact()` **Before v1.1:** 400µs **Now:** 240µs
`getHeading()` **Before v1.1:** 600µs **Now:** 290µs **Adafruit Library:** 1130µs
`isCalibrated()` **Before v1.1:** 1000µs **Now:** 240µs
`saveOffsets()` **Before v1.1:** 4500µs **Now:** 1400µs
`loadOffsets()` **Before v1.1:** 46000µs **Now:** 37400µs
