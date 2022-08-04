/*
 * TMC429 library definitions
 *
 * This file contains all macro and function definitions of the TMC429 library.
*/

#ifndef TMC_IC_TMC429_H_
#define TMC_IC_TMC429_H_

#include "API_Header.h"
#include "TMC429_Register.h"
#include "misc.h"

#define TMC429_REGISTER_COUNT 128

typedef struct
{
	int velocity, oldX;
	uint32_t oldTick;
	int32_t regShadow[TMC429_REGISTER_COUNT];
	uint8_t registerAccess[TMC429_REGISTER_COUNT];
} TMC429TypeDef;

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

	u32 tick;
	bool vMaxModified;
	TMC429TypeDef CHIP;
} TMC429_RSRC;

typedef struct{
	TMC429_RSRC rsrc;
	uint32_t (*right)(TMC429_RSRC *rsrc,int32_t velocity);
	uint32_t (*left)(TMC429_RSRC *rsrc,int32_t velocity);
	uint32_t (*rotate)(TMC429_RSRC *rsrc,int32_t velocity);
	uint32_t (*stop)(TMC429_RSRC *rsrc);
	uint32_t (*moveTo)(TMC429_RSRC *rsrc,int32_t position);
	uint32_t (*moveBy)(TMC429_RSRC *rsrc,int32_t *ticks);
	void (*readRegister)(TMC429_RSRC *rsrc,uint8_t address, int32_t *value);
	void (*writeRegister)(TMC429_RSRC *rsrc,uint8_t address, int32_t value);
	uint32_t (*getMeasuredSpeed)(TMC429_RSRC *rsrc,int32_t *value);

	void (*periodicJob)(TMC429_RSRC *rsrc, uint32_t tick);
	void (*init)(TMC429_RSRC *rsrc);
	void (*deInit)(TMC429_RSRC *rsrc);
	uint8_t (*reset)(TMC429_RSRC *rsrc);
} TMC429_DEV;

// setup a TMC429 device
void setupDev_tmc429(
		TMC429_DEV *dev,
	const char* NAME,
	SPI_HandleTypeDef* SPI_HANDLE,	// spi handle
	const PIN_T *CS,				// spi select
	const PIN_T *DRV_ENN_CFG6,		// driver enable
	const PIN_T *SD_MODE,			//
	const PIN_T *SWN_DIAG0,			//
	const PIN_T *SWP_DIAG1,			//
	const PIN_T *ENCA_DCIN_CFG5,	//
	const PIN_T *ENCB_DCEN_CFG4,	//
	const PIN_T *ENCN_DCO			//
);

#endif /* TMC_IC_TMC429_H_ */
