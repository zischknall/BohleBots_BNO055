#ifndef BohleBots_BNO055_h
#define BohleBots_BNO055_h

#include "Arduino.h"
#include "stdint.h"

#define BNO_ADDR 0x28

//OPRMODES
#define OPR_MODE_CONFIG B0000
#define OPR_MODE_NDOF B1100

/***** PAGE 0 *****/

//SYSTEM
#define CHIP_ID_ADDR 0x00
#define ACC_ID_ADDR 0x01
#define MAG_ID_ADDR 0x02
#define GYR_ID_ADDR 0x03
#define PAGE_ID_ADDR 0x07
#define CALIB_STAT_ADDR 0x35
#define ST_RESULT_ADDR 0x36
#define INT_STA_ADDR 0x37
#define SYS_ERROR_ADDR 0x38
#define SYS_STATUS_ADDR 0x39
#define UNIT_SEL_ADDR 0x3B
#define OPR_MODE_ADDR 0x3D
#define PWR_MODE_ADDR 0x3E
#define SYS_TRIGGER 0x3F

//OFFSETS
#define ACC_OFFSET_X_LSB_ADDR 0x55
#define ACC_OFFSET_X_MSB_ADDR 0x56
#define ACC_OFFSET_Y_LSB_ADDR 0x57
#define ACC_OFFSET_Y_MSB_ADDR 0x58
#define ACC_OFFSET_Z_LSB_ADDR 0x59
#define ACC_OFFSET_Z_MSB_ADDR 0x5A
#define MAG_OFFSET_X_LSB_ADDR 0x5B
#define MAG_OFFSET_X_MSB_ADDR 0x5C
#define MAG_OFFSET_Y_LSB_ADDR 0x5D
#define MAG_OFFSET_Y_MSB_ADDR 0x5E
#define MAG_OFFSET_Z_LSB_ADDR 0x5F
#define MAG_OFFSET_Z_MSB_ADDR 0x60
#define GYR_OFFSET_X_LSB_ADDR 0x61
#define GYR_OFFSET_X_MSB_ADDR 0x62
#define GYR_OFFSET_Y_LSB_ADDR 0x63
#define GYR_OFFSET_Y_MSB_ADDR 0x64
#define GYR_OFFSET_Z_LSB_ADDR 0x65
#define GYR_OFFSET_Z_MSB_ADDR 0x66
#define ACC_RADIUS_LSB_ADDR 0x67
#define ACC_RADIUS_MSB_ADDR 0x68
#define MAG_RADIUS_LSB_ADDR 0x69
#define MAG_RADIUS_MSB_ADDR 0x6A

//DATA
#define EUL_HEADING_LSB_ADDR 0x1A
#define EUL_HEADING_MSB_ADDR 0x1B
#define EUL_ROLL_LSB_ADDR 0x1C
#define EUL_ROLL_MSB_ADDR 0x1D
#define EUL_PITCH_LSB_ADDR 0x1E
#define EUL_PITCH_MSB_ADDR 0x1F

/***** PAGE 1 *****/

#define INT_EN_ADDR 0x10
#define ACC_INT_SETTINGS_ADDR 0x12
#define ACC_HG_DURATION_ADDR 0x13
#define ACC_HG_THRES_ADDR 0x14
#define INT_MSK_ADDR 0x1F

class BNO
{
	public:
		BNO();
		int16_t getHeading();
		bool getImpact();
		bool isCalibrated();
		uint8_t getCalibration();
		void saveOffsets(unsigned int address);
		void loadOffsets(unsigned int address);
		void startBNO(uint8_t impact, bool forward);
		void serialPrintCalibStat();
		void setReference();
		int16_t getRLHeading();
		int16_t getHeadingAuto(unsigned int addr);
		int16_t getRLHeadingAuto(unsigned int addr);
		int16_t getPitch();
		int16_t getRoll();
	private:
		struct sysInfo
		{
			uint8_t chip_id;
			uint8_t acc_id;
			uint8_t mag_id;
			uint8_t gyr_id;
			uint8_t page_id;

			uint8_t opr_mode;
			uint8_t pwr_mode;

			uint8_t unit_ori;
			uint8_t unit_temp;
			uint8_t unit_eul;
			uint8_t unit_gyr;
			uint8_t unit_acc;

			uint8_t sys_status;
			uint8_t sys_error;
		} _infoData;
		struct calibStat
		{
			uint8_t sys;
  			uint8_t gyr;
			uint8_t acc;
			uint8_t mag;
		} _calibData;
		struct testResults
		{
			uint8_t mcu;
			uint8_t gyr;
			uint8_t mag;
			uint8_t acc;
		} _testData;
		struct calibOffsets
		{
			int16_t acc_x;
			int16_t acc_y;
			int16_t acc_z;
			int16_t mag_x;
			int16_t mag_y;
			int16_t mag_z;
			int16_t gyr_x;
			int16_t gyr_y;
			int16_t gyr_z;
			int16_t acc_rad;
			int16_t mag_rad;
		} _offsetData;
		struct calibOffsets _offsetCache = {};
		bool _cached = false;
    		int16_t _reference;
		void writePhase(uint8_t regaddr);
		uint8_t readRegister(uint8_t regaddr);
		uint16_t readRegister16(uint8_t regaddr);
		void writeRegister(uint8_t regaddr, uint8_t value);
		void setOffsets(struct calibOffsets *ptr);
		void getOffsets(struct calibOffsets *ptr);
		void getCalibStat(struct calibStat *ptr);
};
#endif
