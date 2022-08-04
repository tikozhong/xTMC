/*
 * TMC5160.h
 *
 *  Created on: 17.03.2017
 *      Author: ED based on BS
 */

#ifndef TMC_DEV_TMC5160_H_
#define TMC_DEV_TMC5160_H_

#include "misc.h"

typedef struct{
	char name[DEV_NAME_LEN];
	const PIN_T *REFL_UC;
	const PIN_T *REFR_UC;
	const PIN_T *DRV_ENN_CFG6;
	const PIN_T *ENCA_DCIN_CFG5;
	const PIN_T *ENCB_DCEN_CFG4;
	const PIN_T *ENCN_DCO;
	const PIN_T *SD_MODE;
	const PIN_T *SPI_MODE;
	const PIN_T *SWN_DIAG0;
	const PIN_T *SWP_DIAG1;
	const PIN_T *CS;
	SPI_HandleTypeDef* SPI_HANDLE;

	u16 eepromAddrBase;
	s8 (*EepromWrite)  (u16 addr, const u8 *pDat, u16 nBytes);
	s8 (*EepromRead)   (u16 addr, u8 *pDat, u16 nBytes);

	/* callback */
	void (*callBackRefLTurnOn) 	(char *devName);	//left sw turn on
	void (*callBackRefLRelease) (char *devName);	//left sw release
	void (*callBackRefRTurnOn) 	(char *devName);
	void (*callBackRefRRelease) (char *devName);
	void (*callBackAbsPosChanged) (char *devName, u32 pos);
	void (*callBackStatusChanged) (char *devName, u16 status);
	void (*callBackAdcOutChanged) (char *devName, u8 adc);

	bool vMaxModified;
	bool uart_mode;
	TMC5160TypeDef TMC5160;
	ConfigurationTypeDef *TMC5160_config;
} TMC5160_RSRC;

typedef struct{
	TMC5160_RSRC rsrc;
	uint32_t (*right)(TMC5160_RSRC *rsrc,int32_t velocity);
	uint32_t (*left)(TMC5160_RSRC *rsrc,int32_t velocity);
	uint32_t (*rotate)(TMC5160_RSRC *rsrc,int32_t velocity);
	uint32_t (*stop)(uint8_t motor);
	uint32_t (*moveTo)(TMC5160_RSRC *rsrc,int32_t position);
	uint32_t (*moveBy)(TMC5160_RSRC *rsrc,int32_t *ticks);
	uint32_t (*GAP)(uint8_t type, uint8_t motor, int32_t *value);
	uint32_t (*SAP)(uint8_t type, uint8_t motor, int32_t value);
	void (*readRegister)(TMC5160_RSRC *rsrc,uint8_t address, int32_t *value);
	void (*writeRegister)(TMC5160_RSRC *rsrc,uint8_t address, int32_t value);
	uint32_t (*getMeasuredSpeed)(TMC5160_RSRC *rsrc,int32_t *value);

	void (*tmc5160_writeDatagram)(TMC5160_RSRC *rsrc,uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
	void (*tmc5160_writeInt)(TMC5160_RSRC *rsrc,uint8_t address, int value);
	int (*tmc5160_readInt)(TMC5160_RSRC *rsrc,uint8_t address);
	void (*writeDatagram_spi)(TMC5160_RSRC *rsrc,uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
	int32_t (*readInt_spi)(TMC5160_RSRC *rsrc,uint8_t address);
	void (*writeDatagram_uart)(TMC5160_RSRC *rsrc,uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
	int32_t (*readInt_uart)(TMC5160_RSRC *rsrc,uint8_t address);

	void (*periodicJob)(uint32_t tick);
	void (*checkErrors)(uint32_t tick);
	void (*deInit)(void);
	uint32_t (*userFunction)(uint8_t type, uint8_t motor, int32_t *value);
	uint8_t (*reset)();
} TMC5160_DEV;

void setupDev_tmc5160(TMC5160_DEV *dev, bool con_mode);

#endif /* TMC_IC_TMC5160_H_ */
