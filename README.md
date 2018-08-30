# BohleBots_BNO055

## Public Functions
`getHeading()` outputs the current heading as a 16-bit signed integer.

`getImpact()` returns true if a High-G interrupt occurred.

`isCalibrated()` returns true if everything on the BNO055 is on the highest calibration level.

`serialPrintCalibStat()` prints the current calibration levels via serial. Established serial connection required.

`saveOffsets()` gets current offset data from BNO055 and saves it to EEPROM at address 100 upwards. Offset data consists of 22 bytes.

`loadOffsets()` gets offset data saved in EEPROM at address 100 upwards and writes it to the BNO055.

**REQUIRED** `startBNO()` enables you to use the compass by enabling High-G interrupt and putting it into NDOF fusion output mode.

## TO-DO
- [ ] Add interrupt pin forwarding
- [ ] Add heading relative to a point
- [ ] Customisable EEPROM address
