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
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 0) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 0);

	int16_t heading = 0;
	heading = readRegister(BNO_ADDR, EUL_HEADING_MSB_ADDR)<<8;
	heading += readRegister(BNO_ADDR, EUL_HEADING_LSB_ADDR);
	return (heading/16)+1;
}

bool BNO::getImpact()	//reads the INT_STA Register to check if a High_G event occurred
{
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 0) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 0);
	return (readRegister(BNO_ADDR, INT_STA_ADDR)&B00100000)>>5;
}

bool BNO::isCalibrated()	//Gets the latest calibration values and does a bitwise and to return a true if everything is fully calibrated
{
	getCalibStat(&_calibData);
	if((_calibData.sys & _calibData.gyr & _calibData.acc & _calibData.mag) == 3) return true;
	return false;
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
}

void BNO::loadOffsets(unsigned int address)	//loads offsets structure from eeprom at byte 100 upwards and loads the offsets onto the compass
{
	EEPROM.get(address, _offsetData);
	setOffsets(&_offsetData);
}

void BNO::startBNO(uint8_t impact, bool forward)	//enables High_g Interrupt and puts the Compass into NDOF fusion mode
{
	//Enable High-G Interrupt
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 0) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 0);
	writeRegister(BNO_ADDR, OPR_MODE_ADDR, OPR_MODE_CONFIG);
	delay(19);
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 1) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 1);

	writeRegister(BNO_ADDR, INT_EN_ADDR, B00100000);
	writeRegister(BNO_ADDR, INT_MSK_ADDR, B00000000);
	writeRegister(BNO_ADDR, ACC_INT_Settings_ADDR, B01100000);
	writeRegister(BNO_ADDR, ACC_HG_DURATION_ADDR, 1);
	writeRegister(BNO_ADDR, ACC_HG_THRES_ADDR, impact);
	if(forward)
	{
	       	writeRegister(BNO_ADDR, INT_MSK_ADDR, B00100000);
	}else
	{
		writeRegister(BNO_ADDR, INT_MSK_ADDR, B00000000);
	}

	//Change Operation mode
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 0) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 0);

	writeRegister(BNO_ADDR, OPR_MODE_ADDR, OPR_MODE_NDOF);
	delay(19);

	uint8_t sysStatus = readRegister(BNO_ADDR, SYS_STATUS_ADDR);
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

int16_t BNO::getRelHeading()
{

}

/***** Private Functions *****/

void BNO::writePhase(uint8_t addr, uint8_t regaddr)	//Write Phase needed to tell from which address we want to read
{
	Wire.beginTransmission(addr);
	Wire.write(regaddr);
	Wire.endTransmission();
}

uint8_t BNO::readRegister(uint8_t addr, uint8_t regaddr)	//reads byte from a register
{
	uint8_t value = 0;
	writePhase(addr, regaddr);
	Wire.requestFrom((int)addr,1,true);
	while(Wire.available())
	{
		value = Wire.read();
	}
	return value;
}

void BNO::writeRegister(uint8_t addr, uint8_t regaddr, uint8_t value)	//writes byte to a register
{
	Wire.beginTransmission(addr);
	Wire.write(regaddr);
	Wire.write(value);
	Wire.endTransmission();
}

void BNO::getOffsets(struct calibOffsets *ptr)
{
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 0) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 0);
  
	ptr->acc_x = readRegister(BNO_ADDR, ACC_OFFSET_X_MSB_ADDR)<<8;
	ptr->acc_x += readRegister(BNO_ADDR, ACC_OFFSET_X_LSB_ADDR);
	ptr->acc_y = readRegister(BNO_ADDR, ACC_OFFSET_Y_MSB_ADDR)<<8;
	ptr->acc_y += readRegister(BNO_ADDR, ACC_OFFSET_Y_LSB_ADDR);
	ptr->acc_z = readRegister(BNO_ADDR, ACC_OFFSET_Z_MSB_ADDR)<<8;
	ptr->acc_z += readRegister(BNO_ADDR, ACC_OFFSET_Z_LSB_ADDR);
  
	ptr->mag_x = readRegister(BNO_ADDR, MAG_OFFSET_X_MSB_ADDR)<<8;
	ptr->mag_x += readRegister(BNO_ADDR, MAG_OFFSET_X_LSB_ADDR);
	ptr->mag_y = readRegister(BNO_ADDR, MAG_OFFSET_Y_MSB_ADDR)<<8;
	ptr->mag_y += readRegister(BNO_ADDR, MAG_OFFSET_Y_LSB_ADDR);
	ptr->mag_z = readRegister(BNO_ADDR, MAG_OFFSET_Z_MSB_ADDR)<<8;
	ptr->mag_z += readRegister(BNO_ADDR, MAG_OFFSET_Z_LSB_ADDR);
  
	ptr->gyr_x = readRegister(BNO_ADDR, GYR_OFFSET_X_MSB_ADDR)<<8;
	ptr->gyr_x += readRegister(BNO_ADDR, GYR_OFFSET_X_LSB_ADDR);
	ptr->gyr_y = readRegister(BNO_ADDR, GYR_OFFSET_Y_MSB_ADDR)<<8;
	ptr->gyr_y += readRegister(BNO_ADDR, GYR_OFFSET_Y_LSB_ADDR);
	ptr->gyr_z = readRegister(BNO_ADDR, GYR_OFFSET_Z_MSB_ADDR)<<8;
	ptr->gyr_z += readRegister(BNO_ADDR, GYR_OFFSET_Z_LSB_ADDR);

	ptr->acc_rad = readRegister(BNO_ADDR, ACC_RADIUS_MSB_ADDR)<<8;
	ptr->acc_rad += readRegister(BNO_ADDR, ACC_RADIUS_LSB_ADDR);

	ptr->mag_rad = readRegister(BNO_ADDR, MAG_RADIUS_MSB_ADDR)<<8;
	ptr->mag_rad += readRegister(BNO_ADDR, MAG_RADIUS_LSB_ADDR);
}

void BNO::setOffsets(struct calibOffsets *ptr)	//writes given offset structure into the compass
{
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 0) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 0);

	writeRegister(BNO_ADDR, OPR_MODE_ADDR, OPR_MODE_CONFIG);
	delay(19);

	writeRegister(BNO_ADDR, ACC_OFFSET_X_MSB_ADDR, (ptr->acc_x)>>8);
	writeRegister(BNO_ADDR, ACC_OFFSET_X_LSB_ADDR, (ptr->acc_x)&B11111111);
	writeRegister(BNO_ADDR, ACC_OFFSET_Y_MSB_ADDR, (ptr->acc_y)>>8);
	writeRegister(BNO_ADDR, ACC_OFFSET_Y_LSB_ADDR, (ptr->acc_y)&B11111111);
	writeRegister(BNO_ADDR, ACC_OFFSET_Z_MSB_ADDR, (ptr->acc_z)>>8);
	writeRegister(BNO_ADDR, ACC_OFFSET_Z_LSB_ADDR, (ptr->acc_z)&B11111111);

	writeRegister(BNO_ADDR, MAG_OFFSET_X_MSB_ADDR, (ptr->mag_x)>>8);
	writeRegister(BNO_ADDR, MAG_OFFSET_X_LSB_ADDR, (ptr->mag_x)&B11111111);
	writeRegister(BNO_ADDR, MAG_OFFSET_Y_MSB_ADDR, (ptr->mag_y)>>8);
	writeRegister(BNO_ADDR, MAG_OFFSET_Y_LSB_ADDR, (ptr->mag_y)&B11111111);
	writeRegister(BNO_ADDR, MAG_OFFSET_Z_MSB_ADDR, (ptr->mag_z)>>8);
	writeRegister(BNO_ADDR, MAG_OFFSET_Z_LSB_ADDR, (ptr->mag_z)&B11111111);

	writeRegister(BNO_ADDR, GYR_OFFSET_X_MSB_ADDR, (ptr->gyr_x)>>8);
	writeRegister(BNO_ADDR, GYR_OFFSET_X_LSB_ADDR, (ptr->gyr_x)&B11111111);
	writeRegister(BNO_ADDR, GYR_OFFSET_Y_MSB_ADDR, (ptr->gyr_y)>>8);
	writeRegister(BNO_ADDR, GYR_OFFSET_Y_LSB_ADDR, (ptr->gyr_y)&B11111111);
	writeRegister(BNO_ADDR, GYR_OFFSET_Z_MSB_ADDR, (ptr->gyr_z)>>8);
	writeRegister(BNO_ADDR, GYR_OFFSET_Z_LSB_ADDR, (ptr->gyr_z)&B11111111);

	writeRegister(BNO_ADDR, ACC_RADIUS_MSB_ADDR, (ptr->acc_rad)>>8);
	writeRegister(BNO_ADDR, ACC_RADIUS_LSB_ADDR, (ptr->acc_rad)&B11111111);
	writeRegister(BNO_ADDR, MAG_RADIUS_MSB_ADDR, (ptr->mag_rad)>>8);
	writeRegister(BNO_ADDR, MAG_RADIUS_LSB_ADDR, (ptr->mag_rad)&B11111111);

	writeRegister(BNO_ADDR, OPR_MODE_ADDR, OPR_MODE_NDOF);
	delay(19);
}

void BNO::getCalibStat(struct calibStat *ptr)	//gets current calibration status
{
	if(readRegister(BNO_ADDR, PAGE_ID_ADDR) != 0) writeRegister(BNO_ADDR, PAGE_ID_ADDR, 0);

	ptr->sys = (readRegister(BNO_ADDR, CALIB_STAT_ADDR)&B11000000)>>6;
	ptr->gyr = (readRegister(BNO_ADDR, CALIB_STAT_ADDR)&B00110000)>>4;
	ptr->acc = (readRegister(BNO_ADDR, CALIB_STAT_ADDR)&B00001100)>>2;
	ptr->mag = readRegister(BNO_ADDR, CALIB_STAT_ADDR)&B00000011;
}
