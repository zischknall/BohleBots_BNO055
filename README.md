# BohleBots_BNO055

## Public Functions
`getHeading()` outputs the current heading as a 16-bit signed integer.

`getImpact()` returns true if a High-G interrupt occurred.

`isCalibrated()` returns true if everything on the BNO055 is on the highest calibration level.

`serialPrintCalibStat()` prints the current calibration levels via serial. Established serial connection required.

`saveOffsets(address)` gets current offset data from BNO055 and saves it to EEPROM at the specified address. Offset data consists of 22 bytes.

`loadOffsets(address)` gets offset data saved in EEPROM at the specified address and writes it to the BNO055.

**REQUIRED** `startBNO(impact, forward)` enables you to use the compass by enabling High-G interrupt and putting it into NDOF fusion output mode. Impact accepts an integer from 0-255 and regulates the threshold for the impact detection. Forward accepts a bool and enables interrupt pin forwarding when true.

### Sice v1.1
`setReference()` saves the curren heading as a reference for the relative heading

`getRLHeading()` outputs current heading relative to the reference point (from 180 to -180) as a 16-bit signed integer

## TO-DO
- [x] Add interrupt pin forwarding
- [x] Add heading relative to a point
- [x] Customisable EEPROM address
