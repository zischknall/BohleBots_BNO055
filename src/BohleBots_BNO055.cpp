/*
BohleBots_BNO055.cpp - Library for the BNO055 as simple compass in Robotics
Copyright (C) 2018  Tobias "Zischknall" Rath

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Arduino.h"
#include "Wire.h"
#include "EEPROM.h"
#include "stdint.h"
#include "BohleBots_BNO055.h"

/***** Public Functions *****/

BNO::BNO()
{
}

int16_t BNO::getHeading()	//reads the lSB and MSB of the EUL_HEADING and outputs the combined value
{
	int16_t heading = 0;
	heading = readRegister16(EUL_HEADING_LSB_ADDR);
	return (heading/16);
}

int16_t BNO::getRoll()
{
	int16_t roll = 0;
	roll = readRegister16(EUL_ROLL_LSB_ADDR);
	return (roll/16);
}

int16_t BNO::getPitch()
{
	int16_t pitch = 0;
	pitch = readRegister16(EUL_PITCH_LSB_ADDR);
	return (pitch/16);
}

bool BNO::getImpact()	//reads the INT_STA Register to check if a High_G event occurred
{
	return (readRegister(INT_STA_ADDR)&B00100000)>>5;
}

bool BNO::isCalibrated()	//Gets the latest calibration values and does a bitwise and to return a true if everything is fully calibrated
{
	getCalibStat(&_calibData);
	if((_calibData.sys & _calibData.gyr & _calibData.acc & _calibData.mag) == 3) return true;
	return false;
}

uint8_t BNO::getCalibration()
{
	getCalibStat(&_calibData);
	uint8_t tmp = 0;
	tmp += _calibData.sys<<6;
	tmp += _calibData.gyr<<4;
	tmp += _calibData.acc<<2;
	tmp += _calibData.mag;
	return tmp;
}

void BNO::serialPrintCalibStat()	//gets the latest calibration values and prints them via serial
{
	getCalibStat(&_calibData);
	Serial.print("CALIB_STAT_SYSTEM:\t");  Serial.println(_calibData.sys, DEC);
	Serial.print("CALIB_STAT_GYR:\t");  Serial.println(_calibData.gyr, DEC);
	Serial.print("CALIB_STAT_ACC:\t");  Serial.println(_calibData.acc, DEC);
	Serial.print("CALIB_STAT_MAG:\t");  Serial.println(_calibData.mag, DEC);
}

void BNO::saveOffsets(unsigned int address)	//saves offset structure into eeprom at byte 100 upwards
{
	getOffsets(&_offsetData);
	EEPROM.put(address, _offsetData);
#ifdef ESP32
	EEPROM.commit();
#endif
	_offsetCache = _offsetData;
	_cached = true;
}

void BNO::loadOffsets(unsigned int address)	//loads offsets structure from eeprom at byte 100 upwards and loads the offsets onto the compass
{
	if(!_cached)
	{
	EEPROM.get(address, _offsetData);
	setOffsets(&_offsetData);
	_offsetCache = _offsetData;
	_cached = true;
	}
	else
	{
	setOffsets(&_offsetCache);
	}
}

void BNO::startBNO(uint8_t impact, bool forward)	//enables High_g Interrupt and puts the Compass into NDOF fusion mode
{
	//Enable High-G Interrupt
	writeRegister(PAGE_ID_ADDR, 0);
	writeRegister(OPR_MODE_ADDR, OPR_MODE_CONFIG);
	delay(19);
	writeRegister(PAGE_ID_ADDR, 1);

	writeRegister(INT_EN_ADDR, B00100000);
	writeRegister(INT_MSK_ADDR, B00000000);
	writeRegister(ACC_INT_SETTINGS_ADDR, B01100000);
	writeRegister(ACC_HG_DURATION_ADDR, 1);
	writeRegister(ACC_HG_THRES_ADDR, impact);
	if(forward)
	{
	       	writeRegister(INT_MSK_ADDR, B00100000);
	}else
	{
		writeRegister(INT_MSK_ADDR, B00000000);
	}

	//Change Operation mode
	writeRegister(PAGE_ID_ADDR, 0);

	writeRegister(OPR_MODE_ADDR, OPR_MODE_NDOF);
	delay(19);

	uint8_t sysStatus = readRegister(SYS_STATUS_ADDR);
	if(sysStatus != 5)
	{
		Serial.print("SYS_STATUS:\t");  Serial.println(sysStatus, DEC);
		Serial.println("SYS_STATUS should be 5!");
	}
}

void BNO::setReference()
{
	_reference = getHeading();
}

int16_t BNO::getRLHeading()
{
	return (((getHeading()-_reference)+540)%360)-180;
}

int16_t BNO::getHeadingAuto(unsigned int addr)
{
	if(getImpact())
	{
		loadOffsets(addr);
		while(getHeading()==1);
	}
	return getHeading();
}

int16_t BNO::getRLHeadingAuto(unsigned int addr)
{
	if(getImpact())
	{
		loadOffsets(addr);
		while(getHeading()==1);
	}
	return getRLHeading();
}

/***** Private Functions *****/

inline void BNO::writePhase(uint8_t regaddr)	//Write Phase needed to tell from which address we want to read
{
	Wire.beginTransmission(BNO_ADDR);
	Wire.write(regaddr);
	Wire.endTransmission();
}

inline uint8_t BNO::readRegister(uint8_t regaddr)	//reads byte from a register
{
	uint8_t value = 0;
	writePhase(regaddr);
	Wire.requestFrom(BNO_ADDR, 1, (int)true);
	while(Wire.available() < 1);
	value = Wire.read();
	return value;
}

inline uint16_t BNO::readRegister16(uint8_t regaddr)
{
	uint16_t value = 0;
	uint8_t tmp = 0;
	writePhase(regaddr);
	Wire.requestFrom(BNO_ADDR, 2, (int)true);
	while(Wire.available() < 2);
	tmp = Wire.read();
	value = Wire.read()<<8;
	value += tmp;
	return value;
}

inline void BNO::writeRegister(uint8_t regaddr, uint8_t value)	//writes byte to a register
{
	Wire.beginTransmission(BNO_ADDR);
	Wire.write(regaddr);
	Wire.write(value);
	Wire.endTransmission();
}

void BNO::getOffsets(struct calibOffsets *ptr)
{
	uint8_t tmp = 0;
	writePhase(ACC_OFFSET_X_LSB_ADDR);
	Wire.requestFrom(BNO_ADDR, 22, (int)true);
	while(Wire.available() < 22);

	//ACCEL OFFSETS
	tmp = Wire.read();
	ptr->acc_x = Wire.read()<<8;
	ptr->acc_x += tmp;

	tmp = Wire.read();
	ptr->acc_y = Wire.read()<<8;
	ptr->acc_y += tmp;

	tmp = Wire.read();
	ptr->acc_z = Wire.read()<<8;
	ptr->acc_z += tmp;

	//MAG OFFSETS
	tmp = Wire.read();
	ptr->mag_x = Wire.read()<<8;
	ptr->mag_x += tmp;

	tmp = Wire.read();
	ptr->mag_y = Wire.read()<<8;
	ptr->mag_y += tmp;

	tmp = Wire.read();
	ptr->mag_z = Wire.read()<<8;
	ptr->mag_z += tmp;

	//GYRO OFFSETS
	tmp = Wire.read();
	ptr->gyr_x = Wire.read()<<8;
	ptr->gyr_x += tmp;

	tmp = Wire.read();
	ptr->gyr_y = Wire.read()<<8;
	ptr->gyr_y += tmp;

	tmp = Wire.read();
	ptr->gyr_z = Wire.read()<<8;
	ptr->gyr_z += tmp;

	//ACCEL RADIUS
	tmp = Wire.read();
	ptr->acc_rad = Wire.read()<<8;
	ptr->acc_rad += tmp;

	//MAG RADIUS
	tmp = Wire.read();
	ptr->mag_rad = Wire.read()<<8;
	ptr->mag_rad += tmp;
}

void BNO::setOffsets(struct calibOffsets *ptr)	//writes given offset structure into the compass
{
	writeRegister(OPR_MODE_ADDR, OPR_MODE_CONFIG);
	delay(19);

	Wire.beginTransmission(BNO_ADDR);
	Wire.write(ACC_OFFSET_X_LSB_ADDR);
	Wire.write((ptr->acc_x)&B11111111);
	Wire.write((ptr->acc_x)>>8);
	Wire.write((ptr->acc_y)&B11111111);
	Wire.write((ptr->acc_y)>>8);
	Wire.write((ptr->acc_z)&B11111111);
	Wire.write((ptr->acc_z)>>8);

	Wire.write((ptr->mag_x)&B11111111);
	Wire.write((ptr->mag_x)>>8);
	Wire.write((ptr->mag_y)&B11111111);
	Wire.write((ptr->mag_y)>>8);
	Wire.write((ptr->mag_z)&B11111111);
	Wire.write((ptr->mag_z)>>8);

	Wire.write((ptr->gyr_x)&B11111111);
	Wire.write((ptr->gyr_x)>>8);
	Wire.write((ptr->gyr_y)&B11111111);
	Wire.write((ptr->gyr_y)>>8);
	Wire.write((ptr->gyr_z)&B11111111);
	Wire.write((ptr->gyr_z)>>8);

	Wire.write((ptr->acc_rad)&B11111111);
	Wire.write((ptr->acc_rad)>>8);
  	Wire.write((ptr->mag_rad)&B11111111);
  	Wire.write((ptr->mag_rad)>>8);
	Wire.endTransmission();

	writeRegister(OPR_MODE_ADDR, OPR_MODE_NDOF);
	delay(7);
}

void BNO::getCalibStat(struct calibStat *ptr)	//gets current calibration status
{
	uint8_t tmp = readRegister(CALIB_STAT_ADDR);
	ptr->sys = (tmp&B11000000)>>6;
	ptr->gyr = (tmp&B00110000)>>4;
	ptr->acc = (tmp&B00001100)>>2;
	ptr->mag = tmp&B00000011;
}
