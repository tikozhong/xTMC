/*
	This file provides all functions needed for easy
	access to the TMC429 motion control IC.

	Please not that functions for communication over SPI must be added by the user,
	because this is specific to the MCU that is to be used.

	The ReadWriteSPI function with the following parameters and functionality:
	First parameter: indentifies the SPI device
	Second parameter: byte to be sent to the SPI device
	Third parameter: FALSE means that more bytes will follow, so do not relase the
	  chip select line. TRUE means that this was the last byte, so release the chip
	  select line after this byte has been sent and the answer has been fully received.

	The function shall return the byte that has been received via SPI.
*/

#include "TMC429.h"

	// TMC429 library functions
static void Init429(TMC429_RSRC* p);
static void ReadWrite429(TMC429_RSRC* p, uint8_t *Read, uint8_t *Write);
static void Write429Zero(TMC429_RSRC* p, uint8_t Address);
static void Write429Bytes(TMC429_RSRC* p, uint8_t Address, uint8_t *Bytes);
static void Write429Datagram(TMC429_RSRC* p, uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte);
static void Write429Short(TMC429_RSRC* p, uint8_t Address, int32_t Value);	// write s16
static void Write429Int(TMC429_RSRC* p, uint8_t Address, int32_t Value);	// write s32
static uint8_t Read429Status(TMC429_RSRC* p);
static uint8_t Read429Bytes(TMC429_RSRC* p, uint8_t Address, uint8_t *Bytes);
static uint8_t Read429SingleByte(TMC429_RSRC* p, uint8_t Address, uint8_t Index);
static int32_t Read429Short(TMC429_RSRC* p, uint8_t Address);
static int32_t Read429Int(TMC429_RSRC* p, uint8_t Address);
static void Set429RampMode(TMC429_RSRC* p, uint8_t Axis, uint8_t RampMode);
static void Set429SwitchMode(TMC429_RSRC* p, uint8_t Axis, uint8_t SwitchMode);
static uint8_t SetAMax(TMC429_RSRC* p, uint8_t Motor, uint32_t AMax);
static void HardStop(TMC429_RSRC* p, uint32_t Motor);

void setupDev_tmc429(
	TMC429_DEV *dev,
	const char* NAME,
	SPI_HandleTypeDef* SPI_HANDLE,	// spi handle
	const PIN_T *CS				// spi select
){


}


//
//
///******************************
//   Funktion: RotateRight()
//
//   Zweck: TMCL-Befehl ROR
// *******************************/
//static void RotateRight(TMC429_RSRC* p, u8 axisIndx)
//{
//  if(axisIndx < TMC429_MOTOR_COUNT)
//  {
//	Set429RampMode(p, axisIndx, RM_VELOCITY);
//	Write428Short(axisIndx<<5|IDX_VMAX, 2047);
//	Write428Short(axisIndx<<5|IDX_VTARGET, ActualCommand.Value.Int32);
////    Set428RampMode(ActualCommand.Motor, RM_VELOCITY);
////    Write428Short(ActualCommand.Motor<<5|IDX_VMAX, 2047);
////    Write428Short(ActualCommand.Motor<<5|IDX_VTARGET, ActualCommand.Value.Int32);
//  }
//}
//
//
///******************************
//   Funktion: RotateLeft()
//
//   Zweck: TMCL-Befehl ROL
// *******************************/
//static void RotateLeft(void)
//{
//  if(ActualCommand.Motor<N_O_MOTORS)
//  {
//    Set428RampMode(ActualCommand.Motor, RM_VELOCITY);
//    Write428Short(ActualCommand.Motor<<5|IDX_VMAX, 2047);
//    Write428Short(ActualCommand.Motor<<5|IDX_VTARGET, -ActualCommand.Value.Int32);
//  }
//}
//
//
///******************************
//   Funktion: MotorStop()
//
//   Zweck: TMCL-Befehl MST
// *******************************/
//static void MotorStop(void)
//{
//  if(ActualCommand.Motor<N_O_MOTORS)
//  {
//    Set428RampMode(ActualCommand.Motor, RM_VELOCITY);
//    Write428Short(ActualCommand.Motor<<5|IDX_VTARGET, 0);
//  }
//}
//
//
///******************************
//   Funktion: MoveToPosition()
//
//   Zweck: TMCL-Befehl MVP
// *******************************/
//static void MoveToPosition(void)
//{
//  UINT NewPosition;
//
//  if(ActualCommand.Motor>N_O_MOTORS)
//  {
//    ActualReply.Status=REPLY_INVALID_VALUE;
//    return;
//  }
//
//  if((ActualCommand.Type==MVP_ABS || ActualCommand.Type==MVP_REL) && ActualCommand.Motor>=N_O_MOTORS)
//  {
//    ActualReply.Status=REPLY_INVALID_VALUE;
//    return;
//  }
//
//  switch(ActualCommand.Type)
//  {
//    case MVP_ABS:
//      CheckVAModified(ActualCommand.Motor);
//      Write428Int(ActualCommand.Motor<<5|IDX_XTARGET, ActualCommand.Value.Int32);
//      Set428RampMode(ActualCommand.Motor, RM_RAMP);
//      break;
//
//    case MVP_REL:
//      CheckVAModified(ActualCommand.Motor);
//      NewPosition=Read428Int(ActualCommand.Motor<<5|IDX_XACTUAL)+ActualCommand.Value.Int32;
//      Write428Int(ActualCommand.Motor<<5|IDX_XTARGET, NewPosition);
//      Set428RampMode(ActualCommand.Motor, RM_RAMP);
//      ActualReply.Value.Int32=NewPosition;
//      break;
//
//    default:
//      ActualReply.Status=REPLY_WRONG_TYPE;
//      break;
//  }
//}
//
//
///******************************
//   Funktion: SetAxisParameter
//
//   Zweck: TMCL-Befehl SAP
// *******************************/
//static void SetAxisParameter(void)
//{
//  UCHAR Read[4];
//
//  if(ActualCommand.Motor>=N_O_MOTORS)
//  {
//    ActualReply.Status=REPLY_INVALID_VALUE;
//    return;
//  }
//
//  switch(ActualCommand.Type)
//  {
//    case 0:
//      Write428Int(ActualCommand.Motor<<5|IDX_XTARGET, ActualCommand.Value.Int32);
//      break;
//
//    case 1:
//      Write428Int(ActualCommand.Motor<<5|IDX_XACTUAL, ActualCommand.Value.Int32);
//      break;
//
//    case 2:
//      Write428Short(ActualCommand.Motor<<5|IDX_VTARGET, ActualCommand.Value.Int32);
//      Write428Short(ActualCommand.Motor<<5|IDX_VMAX, abs(ActualCommand.Value.Int32));
//      VAModified[ActualCommand.Motor]=TRUE;
//      break;
//
//    case 3:
//      Write428Short(ActualCommand.Motor<<5|IDX_VACTUAL, ActualCommand.Value.Int32);
//      break;
//
//    case 4:
//      Write428Short(ActualCommand.Motor<<5|IDX_VMAX, ActualCommand.Value.Int32);
//      VMax[ActualCommand.Motor]=ActualCommand.Value.Int32;
//      VAModified[ActualCommand.Motor]=FALSE;
//      break;
//
//    case 5:
//      SetAMax(ActualCommand.Motor, ActualCommand.Value.Int32);
//      AMax[ActualCommand.Motor]=ActualCommand.Value.Int32;
//      VAModified[ActualCommand.Motor]=FALSE;
//      break;
//
//    case 6:
//      Set262StallGuardCurrentScale(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 12:
//      Read428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      if(ActualCommand.Value.Int32!=0)
//        Read[1]|=0x02;
//      else
//        Read[1]&= ~0x02;
//      Write428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      break;
//
//    case 13:
//      Read428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      if(ActualCommand.Value.Int32!=0)
//        Read[1]|=0x01;
//      else
//        Read[1]&= ~0x01;
//      Write428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      break;
//
//    case 130:
//      Write428Short(ActualCommand.Motor<<5|IDX_VMIN, ActualCommand.Value.Int32);
//      break;
//
//    case 135:
//      Write428Short(ActualCommand.Motor<<5|IDX_AACTUAL, ActualCommand.Value.Int32);
//      break;
//
//    case 138:
//      Read428Bytes(ActualCommand.Motor<<5|IDX_REFCONF_RM, Read);
//      Read[2]=ActualCommand.Value.Byte[0];
//      Write428Bytes(ActualCommand.Motor<<5|IDX_REFCONF_RM, Read);
//      break;
//
//    case 140:
//      Set262StepDirMStepRes(ActualCommand.Motor, 8-ActualCommand.Value.Byte[0]);
//      break;
//
//    case 141:
//      Write428Short(ActualCommand.Motor<<5|IDX_DX_REFTOLERANCE, ActualCommand.Value.Int32);
//      break;
//
//    case 149:
//      Read428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      if(ActualCommand.Value.Int32!=0)
//        Read[1]|=0x04;
//      else
//        Read[1]&= ~0x04;
//      Write428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      break;
//
//    case 153:
//      Read428Bytes(ActualCommand.Motor<<5|IDX_PULSEDIV_RAMPDIV, Read);
//      Read[1]=(Read[1] & 0xf0) | (ActualCommand.Value.Byte[0] & 0x0f);
//      Write428Bytes(ActualCommand.Motor<<5|IDX_PULSEDIV_RAMPDIV, Read);
//      break;
//
//    case 154:
//      Read428Bytes(ActualCommand.Motor<<5|IDX_PULSEDIV_RAMPDIV, Read);
//      Read[1]=(Read[1] & 0x0f) | (ActualCommand.Value.Byte[0]<<4);
//      Write428Bytes(ActualCommand.Motor<<5|IDX_PULSEDIV_RAMPDIV, Read);
//      break;
//
//    case 160:
//      Set262StepDirInterpolation(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 161:
//      Set262StepDirDoubleEdge(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 162:
//      Set262ChopperBlankTime(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 163:
//      Set262ChopperMode(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 164:
//      Set262ChopperHysteresisDecay(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 165:
//      Set262ChopperHysteresisEnd(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 166:
//      Set262ChopperHysteresisStart(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 167:
//      Set262ChopperTOff(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 168:
//      Set262SmartEnergyIMin(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 169:
//      Set262SmartEnergyDownStep(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 170:
//      Set262SmartEnergyStallLevelMax(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 171:
//      Set262SmartEnergyUpStep(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 172:
//      Set262SmartEnergyStallLevelMin(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 173:
//      Set262StallGuardFilter(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 174:
//      Set262StallGuardThreshold(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 175:
//      Set262DriverSlopeHighSide(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 176:
//      Set262DriverSlopeLowSide(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 177:
//      Set262DriverDisableProtection(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 178:
//      Set262DriverProtectionTimer(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 179:
//      Set262DriverVSenseScale(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 183:
//      Set262DriverStepDirectionOff(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 184:
//      Set262ChopperRandomTOff(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 185:
//      Set262DriverTestMode(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 209:
//      WriteEncoder(ActualCommand.Motor, ActualCommand.Value.Int32);
//      break;
//
//    case 210:
//      SetEncoderPrescaler(ActualCommand.Motor, ActualCommand.Value.Int32>>5, ActualCommand.Value.Byte[0] & 0x1f);
//      break;
//      break;
//
//    default:
//      ActualReply.Status=REPLY_WRONG_TYPE;
//      break;
//  }
//}
//
//
///******************************
//   Funktion: GetAxisParameter()
//
//   Zweck: TMCL-Befehl GAP
// *******************************/
//static void GetAxisParameter(void)
//{
//  UCHAR Read[4];
//
//  if(ActualCommand.Motor>=N_O_MOTORS)
//  {
//    ActualReply.Status=REPLY_INVALID_VALUE;
//    return;
//  }
//
//  switch(ActualCommand.Type)
//  {
//    case 0:
//      ActualReply.Value.Int32=Read428Int(ActualCommand.Motor<<5|IDX_XTARGET);
//      break;
//
//    case 1:
//      ActualReply.Value.Int32=Read428Int(ActualCommand.Motor<<5|IDX_XACTUAL);
//      break;
//
//    case 2:
//      ActualReply.Value.Int32=Read428Short(ActualCommand.Motor<<5|IDX_VTARGET);
//      break;
//
//    case 3:
//      ActualReply.Value.Int32=Read428Short(ActualCommand.Motor<<5|IDX_VACTUAL);
//      break;
//
//    case 4:
//      ActualReply.Value.Int32=VMax[ActualCommand.Motor];
//      break;
//
//    case 5:
//      ActualReply.Value.Int32=AMax[ActualCommand.Motor];
//      break;
//
//    case 6:
//      ActualReply.Value.Int32=Get262StallGuardCurrentScale(ActualCommand.Motor);
//      break;
//
//    case 8:
//      ActualReply.Value.Int32=(Read428Status() & (1<<(ActualCommand.Motor*2))) ? 1:0;
//      break;
//
//    case 12:
//      Read428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      ActualReply.Value.Int32=(Read[1] & 0x02) ? 1:0;
//      break;
//
//    case 13:
//      Read428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      ActualReply.Value.Int32=(Read[1] & 0x01) ? 1:0;
//      break;
//
//    case 9:
//    case 11:
//      Read428Bytes(IDX_REF_SWITCHES, Read);  //linker Schalter
//      ActualReply.Value.Int32=(Read[2] & (0x02<<ActualCommand.Motor*2)) ? 1:0;
//      break;
//
//    case 10:
//      Read428Bytes(IDX_REF_SWITCHES, Read);  //rechter Schalter
//      ActualReply.Value.Int32=(Read[2] & (0x01<<ActualCommand.Motor*2)) ? 1:0;
//      break;
//
//    case 130:
//      ActualReply.Value.Int32=Read428Short(ActualCommand.Motor<<5|IDX_VMIN);
//      break;
//
//    case 135:
//      ActualReply.Value.Int32=Read428Short(ActualCommand.Motor<<5|IDX_AACTUAL);
//      break;
//
//    case 138:
//      Read428Bytes(ActualCommand.Motor<<5|IDX_REFCONF_RM, Read);
//      ActualReply.Value.Int32=Read[2];
//      break;
//
//    case 140:
//      ActualReply.Value.Int32=8-Get262StepDirMStepRes(ActualCommand.Motor);
//      break;
//
//    case 141:
//      ActualReply.Value.Int32=Read428Short(ActualCommand.Motor<<5|IDX_DX_REFTOLERANCE);
//      break;
//
//    case 149:
//      Read428Bytes(IDX_REFCONF_RM|ActualCommand.Motor<<5, Read);
//      ActualReply.Value.Int32=(Read[1] & 0x04) ? 1:0;
//      break;
//
//    case 153:
//      Read428Bytes(ActualCommand.Motor<<5|IDX_PULSEDIV_RAMPDIV, Read);
//      ActualReply.Value.Int32=Read[1] & 0x0f;
//      break;
//
//    case 154:
//      Read428Bytes(ActualCommand.Motor<<5|IDX_PULSEDIV_RAMPDIV, Read);
//      ActualReply.Value.Int32=Read[1] >> 4;
//      break;
//
//    case 206:
//      ActualReply.Value.Int32=StallLevel[ActualCommand.Motor];
//      break;
//
//    case 207:
//      switch(ActualCommand.Motor)
//      {
//        case 0:
//          ActualReply.Value.Int32=(AT91C_BASE_PIOB->PIO_PDSR & BIT9) ? 1:0;
//          break;
//
//        case 1:
//          ActualReply.Value.Int32=(AT91C_BASE_PIOB->PIO_PDSR & BIT8) ? 1:0;
//          break;
//
//        case 2:
//          ActualReply.Value.Int32=(AT91C_BASE_PIOB->PIO_PDSR & BIT14) ? 1:0;
//          break;
//      }
//      break;
//
//    case 208:
//      ActualReply.Value.Int32=DriverFlags[ActualCommand.Motor];
//      break;
//
//    case 160:
//      ActualReply.Value.Int32=Get262StepDirInterpolation(ActualCommand.Motor);
//      break;
//
//    case 161:
//      ActualReply.Value.Int32=Get262StepDirDoubleEdge(ActualCommand.Motor);
//      break;
//
//    case 162:
//      ActualReply.Value.Int32=Get262ChopperBlankTime(ActualCommand.Motor);
//      break;
//
//    case 163:
//      ActualReply.Value.Int32=Get262ChopperMode(ActualCommand.Motor);
//      break;
//
//    case 164:
//      ActualReply.Value.Int32=Get262ChopperHysteresisDecay(ActualCommand.Motor);
//      break;
//
//    case 165:
//      ActualReply.Value.Int32=Get262ChopperHysteresisEnd(ActualCommand.Motor);
//      break;
//
//    case 166:
//      ActualReply.Value.Int32=Get262ChopperHysteresisStart(ActualCommand.Motor);
//      break;
//
//    case 167:
//      ActualReply.Value.Int32=Get262ChopperTOff(ActualCommand.Motor);
//      break;
//
//    case 168:
//      ActualReply.Value.Int32=Get262SmartEnergyIMin(ActualCommand.Motor);
//      break;
//
//    case 169:
//      ActualReply.Value.Int32=Get262SmartEnergyDownStep(ActualCommand.Motor);
//      break;
//
//    case 170:
//      ActualReply.Value.Int32=Get262SmartEnergyStallLevelMax(ActualCommand.Motor);
//      break;
//
//    case 171:
//      ActualReply.Value.Int32=Get262SmartEnergyUpStep(ActualCommand.Motor);
//      break;
//
//    case 172:
//      ActualReply.Value.Int32=Get262SmartEnergyStallLevelMin(ActualCommand.Motor);
//      break;
//
//    case 173:
//      ActualReply.Value.Int32=Get262StallGuardFilter(ActualCommand.Motor);
//      break;
//
//    case 174:
//      ActualReply.Value.Int32=Get262StallGuardThreshold(ActualCommand.Motor);
//      break;
//
//    case 175:
//      ActualReply.Value.Int32=Get262DriverSlopeHighSide(ActualCommand.Motor);
//      break;
//
//    case 176:
//      ActualReply.Value.Int32=Get262DriverSlopeLowSide(ActualCommand.Motor);
//      break;
//
//    case 177:
//      ActualReply.Value.Int32=Get262DriverDisableProtection(ActualCommand.Motor);
//      break;
//
//    case 178:
//      ActualReply.Value.Int32=Get262DriverProtectionTimer(ActualCommand.Motor);
//      break;
//
//    case 179:
//      ActualReply.Value.Int32=Get262DriverVSenseScale(ActualCommand.Motor);
//      break;
//
//    case 180:
//      ActualReply.Value.Int32=SmartEnergy[ActualCommand.Motor];
//      break;
//
//    case 182:
//      if(IS_SD_MODE_SELECTED() || ActualCommand.Motor!=0)
//      {
//        if(Get262DriverReadSelect(ActualCommand.Motor)!=TMC262_RB_MSTEP)
//          Set262DriverReadSelect(ActualCommand.Motor, TMC262_RB_MSTEP);
//        ActualReply.Value.Int32=0;
//        Read262State(ActualCommand.Motor, &ActualReply.Value.Byte[1], &ActualReply.Value.Byte[0], NULL, NULL, NULL);
//      }
//      else
//      {
//        ActualReply.Value.Byte[1]=0x00;
//        ActualReply.Value.Byte[0]=0x00;
//      }
//      break;
//
//    case 183:
//      ActualReply.Value.Int32=Get262DriverStepDirectionOff(ActualCommand.Motor);
//      break;
//
//    case 184:
//      ActualReply.Value.Int32=Get262ChopperRandomTOff(ActualCommand.Motor);
//      break;
//
//    case 209:
//      ActualReply.Value.Int32=ReadEncoder(ActualCommand.Motor);
//      break;
//
//    default:
//      ActualReply.Status=REPLY_WRONG_TYPE;
//      break;
//  }
//}










/***************************************************************//**
	 \fn ReadWrite429(uint8_t *Read, uint8_t *Write)
	 \brief 32 bit SPI communication with TMC429
	 \param Read   four byte array holding the data read from the TMC428
	 \param Write  four byte array holding the data to write to the TMC428

	 This is the low-level function that does all SPI communication with
	 the TMC429. It sends a 32 bit SPI telegramme to the TMC429 and
	 receives the 32 bit answer telegramme from the TMC429.
********************************************************************/
static void ReadWrite429(TMC429_RSRC* p, uint8_t *rxBuf, uint8_t *txBuf)
{
	while (HAL_SPI_GetState(p->SPI_HANDLE) != HAL_SPI_STATE_READY){}
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(p->CS->GPIOx, p->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(p->SPI_HANDLE, txBuf, rxBuf, 5, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(p->CS->GPIOx, p->CS->GPIO_Pin, GPIO_PIN_SET);
}

/***************************************************************//**
	 \fn Write429Bytes(uint8_t Address, uint8_t *Bytes)
	 \brief TMC429 write access
	 \param Address  TMC429 register address (see also TMC429.h)
	 \param Bytes  Array holding three bytes to be written to the
	               TMC429 register.

	 This function writes an array of  three bytes to a TMC429 register.
********************************************************************/
static void Write429Bytes(TMC429_RSRC* p, uint8_t Address, uint8_t *Bytes)
{
	uint8_t Write[4], Read[4];

	Write[0] = Address;
	Write[1] = Bytes[0];
	Write[2] = Bytes[1];
	Write[3] = Bytes[2];
	ReadWrite429(p, Read, Write);
}

/***************************************************************//**
	 \fn Write429Datagram(uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte)
	 \brief TMC429 write access
	 \param Address   TMC429 register address
	 \param HighByte  MSB of the TMC429 register
	 \param MidByte   mid byte of the TMC429 register
	 \param LowByte   LSB of the TMC429 register

	 This function write three bytes to a TMC429 register.
********************************************************************/
static void Write429Datagram(TMC429_RSRC* p, uint8_t Address, uint8_t HighByte, uint8_t MidByte, uint8_t LowByte)
{
	uint8_t Write[4], Read[4];

	Write[0] = Address;
	Write[1] = HighByte;
	Write[2] = MidByte;
	Write[3] = LowByte;
	ReadWrite429(p,Read, Write);
}

/***************************************************************//**
	 \fn Write429Zero(uint8_t Address)
	 \brief Write zero to a TMC429 register
	 \param Address  TMC429 register address

	 This function sets a TMC429 register to zero. This can be useful
	 e.g. to stop a motor quickly.
********************************************************************/
static void Write429Zero(TMC429_RSRC* p, uint8_t Address)
{
	uint8_t Write[4], Read[4];

	Write[0] = Address;
	Write[1] = 0;
	Write[2] = 0;
	Write[3] = 0;
	ReadWrite429(p, Read, Write);
}

/***************************************************************//**
	 \fn Write429Short(uint8_t Address, int32_t Value)
	 \brief Write 16 bit value to a TMC429 register
	 \param Address  TMC429 register address
	 \param Value    Value to be written

	 This function writes a 16 bit value to a TMC429 register.
********************************************************************/
static void Write429Short(TMC429_RSRC* p, uint8_t Address, int32_t Value)
{
	uint8_t Write[4], Read[4];

	Write[0] = Address;
	Write[1] = 0;
	Write[2] = Value >> 8;
	Write[3] = Value & 0xFF;
	ReadWrite429(p, Read, Write);
}

/***************************************************************//**
	 \fn Write429Int(uint8_t Address, int32_t Value)
	 \brief Write 24 bit value to a TMC429 register
	 \param Address  TMC429 register address
	 \param Value    Value to be written

	 This function writes a 24 bit value to a TMC429 register.
********************************************************************/
static void Write429Int(TMC429_RSRC* p, uint8_t Address, int32_t Value)
{
	uint8_t Write[4], Read[4];

	Write[0] = Address;
	Write[1] = Value >> 16;
	Write[2] = Value >> 8;
	Write[3] = Value & 0xFF;
	ReadWrite429(p, Read, Write);
}

/***************************************************************//**
	 \fn Read429Status
	 \brief Read TMC429 status byte

	 \return TMC429 status byte

	 This functions reads just the status byte of the TMC429 using
	 a single byte SPI access which makes this a little bit faster.
********************************************************************/
static uint8_t Read429Status(TMC429_RSRC* p)
{
	//return ReadWriteSPI(p, SPI_DEV_TMC429, 0x01);
	return 0;
}

/***************************************************************//**
	 \fn Read429Bytes(uint8_t Address, uint8_t *Bytes)
	 \brief Read TMC429 register
	 \param Address  TMC429 register address (see TMC429.h)
	 \param Bytes  Pointer at array of three bytes

	 \return TMC429 status byte

	 This functions reads a TMC429 register and puts the result into
	 an array of bytes. It also returns the TMC429 status bytes.
********************************************************************/
static uint8_t Read429Bytes(TMC429_RSRC* p, uint8_t Address, uint8_t *Bytes)
{
	uint8_t Read[4], Write[4];

	Write[0] = Address | TMC429_READ;
	ReadWrite429(p, Read, Write);

	Bytes[0] = Read[1];
	Bytes[1] = Read[2];
	Bytes[2] = Read[3];

	return Read[0];
}

/***************************************************************//**
	 \fn Read429SingleByte(uint8_t Address, uint8_t Index)
	 \brief Read TMC429 register
	 \param Address  TMC429 register address (see TMC429.h)
	 \param Index  TMC429 register byte to be returned (0..3)

	 \return TM429 register byte

	 This functions reads a TMC429 register and returns the desired
	 byte of that register.
********************************************************************/
static uint8_t Read429SingleByte(TMC429_RSRC* p, uint8_t Address, uint8_t Index)
{
	uint8_t Read[4], Write[4];

	Write[0] = Address | TMC429_READ;
	ReadWrite429(p, Read, Write);

	return Read[Index+1];
}

/***************************************************************//**
	 \fn Read429Short(uint8_t Address)
	 \brief Read TMC429 register (12 bit)
	 \param Address  TMC429 register address (see TMC429.h)

	 \return TMC429 register value (sign extended)

	 This functions reads a TMC429 12 bit register and sign-extends the
	 register value to 32 bit.
********************************************************************/
static int32_t Read429Short(TMC429_RSRC* p, uint8_t Address)
{
	uint8_t Read[4], Write[4];
	int32_t Result;

	Write[0] = Address | TMC429_READ;
	ReadWrite429(p, Read, Write);

	Result = (Read[2]<<8) | Read[3];
	if(Result & 0x00000800)
		Result |= 0xFFFFF000;

	return Result;
}

/***************************************************************//**
	 \fn Read429Int(uint8_t Address)
	 \brief Read TMC429 register (24 bit)
	 \param Address  TMC429 register address (see TMC429.h)

	 \return TMC429 register value (sign extended)

	 This functions reads a TMC429 24 bit register and sign-extends the
	 register value to 32 bit.
********************************************************************/
static int32_t Read429Int(TMC429_RSRC* p, uint8_t Address)
{
	uint8_t Read[4], Write[4];
	int32_t Result;

	Write[0] = Address | TMC429_READ;
	ReadWrite429(p, Read, Write);

	Result = (Read[1]<<16) | (Read[2]<<8) | (Read[3]);
	if(Result & 0x00800000)
		Result |= 0xFF000000;

	return Result;
}

/***************************************************************//**
	 \fn Set429RampMode(uint8_t Axis, uint8_t RampMode)
	 \brief Set the ramping mode of an axis
	 \param  Axis  Motor number (0, 1 or 2)
	 \param  RampMode  ramping mode (RM_RAMP/RM_SOFT/RM_VELOCITY/RM_HOLD)

	 This functions changes the ramping mode of a motor in the TMC429.
	 It is some TMC429 register bit twiddling.
********************************************************************/
static void Set429RampMode(TMC429_RSRC* p, uint8_t Axis, uint8_t RampMode)
{
	uint8_t Write[4], Read[4];

	Write[0] = TMC429_IDX_REFCONF_RM(Axis)|TMC429_READ;
	ReadWrite429(p, Read, Write);

	Write[0] = TMC429_IDX_REFCONF_RM(Axis);
	Write[1] = Read[1];
	Write[2] = Read[2];
	Write[3] = RampMode;
	ReadWrite429(p, Read, Write);
}

/***************************************************************//**
	 \fn Set429SwitchMode(uint8_t Axis, uint8_t SwitchMode)
	 \brief Set the end switch mode
	 \param  Axis  Motor number (0, 1 or 2)
	 \param  SwitchMode  end switch mode

	 This functions changes the end switch mode of a motor in the TMC429.
	 It is some TMC429 register bit twiddling.
********************************************************************/
static void Set429SwitchMode(TMC429_RSRC* p, uint8_t Axis, uint8_t SwitchMode)
{
	uint8_t Write[4], Read[4];

	Write[0] = TMC429_IDX_REFCONF_RM(Axis) | TMC429_READ;
	ReadWrite429(p, Read, Write);

	Write[0] = TMC429_IDX_REFCONF_RM(Axis);
	Write[1] = Read[1];
	Write[2] = SwitchMode;
	Write[3] = Read[3];
	ReadWrite429(p, Read, Write);
}

/***************************************************************//**
	 \fn SetAMax(uint8_t Motor, uint32_t AMax)
	 \brief Set the maximum acceleration
	 \param Motor  motor number (0, 1, 2)
	 \param AMax: maximum acceleration (1..2047)

	 This function sets the maximum acceleration and also calculates
	 the PMUL and PDIV value according to all other parameters
	 (please see the TMC429 data sheet for more info about PMUL and PDIV
	 values).
********************************************************************/
static uint8_t SetAMax(TMC429_RSRC* rsrc, u8 Motor, uint32_t AMax)
{
	int32_t pm, pd;
	float p, p_reduced;
	int32_t ramp_div;
	int32_t pulse_div;
	uint8_t PulseRampDiv;
	uint8_t Data[3];

	AMax &= 0x000007FF;
	Read429Bytes(rsrc, TMC429_IDX_PULSEDIV_RAMPDIV(Motor), Data);
	PulseRampDiv = Data[1];
	pulse_div = PulseRampDiv >> 4;
	ramp_div = PulseRampDiv & 0x0F;

	// -1 indicates : no valid pair found
	pm = -1;
	pd = -1;

	if(ramp_div >= pulse_div)
		p = AMax / ( 128.0 * (1<<ramp_div-pulse_div));  // Exponent positive or 0
	else
		p = AMax / ( 128.0 / (1<<pulse_div-ramp_div));  // Exponent negative

	p_reduced = p*0.988;

	int32_t pdiv;
	for(pdiv = 0; pdiv <= 13; pdiv++)
	{
		int32_t pmul = (int32_t) (p_reduced * 8.0 * (1<<pdiv)) - 128;

		if((0 <= pmul) && (pmul <= 127))
		{
			pm = pmul + 128;
			pd = pdiv;
		}
	}

	Data[0] = 0;
	Data[1] = (uint8_t) pm;
	Data[2] = (uint8_t) pd;
	Write429Bytes(rsrc, TMC429_IDX_PMUL_PDIV(Motor), Data);
	Write429Short(rsrc, TMC429_IDX_AMAX(Motor), AMax);

	return 0;
}

/***************************************************************//**
	 \fn HardStop(uint32_t Motor)
	 \brief Stop a motor immediately
	 \param Motor: motor number (0, 1, 2)

	 This function stops a motor immediately (hard stop) by switching
	 to velocity mode and then zeroing the V_TARGT and V_ACTUAL registers
	 of that axis.
********************************************************************/
static void HardStop(TMC429_RSRC* p, uint32_t Motor)
{
	Set429RampMode(p, Motor, TMC429_RM_VELOCITY);
	Write429Zero(p, TMC429_IDX_VTARGET(Motor));
	Write429Zero(p, TMC429_IDX_VACTUAL(Motor));
}

/***************************************************************//**
	 \fn Init429
	 \brief TMC429 initialization

	 This function does all necessary initializations of the TMC429
	 to operate in step/direction mode.
********************************************************************/
static void Init429(TMC429_RSRC* p)
{
	uint8_t motor;
	for(motor = 0; motor < 3; motor++)
	{
		uint32_t addr;
		for(addr = 0; addr <= TMC429_IDX_XLATCHED(motor); addr++)
			Write429Zero(p, addr | TMC429_MOTOR(motor));
	}

	Write429Int(p, TMC429_IDX_IF_CONFIG_429, TMC429_IFCONF_EN_SD | TMC429_IFCONF_EN_REFR | TMC429_IFCONF_SDO_INT);
	Write429Datagram(p, TMC429_IDX_SMGP, 0x00, 0x00, 0x02);

	for(motor = 0; motor < 3; motor++)
	{
		Write429Datagram(p, TMC429_IDX_PULSEDIV_RAMPDIV(motor), 0x00, 0x37, 0x06);
		Write429Datagram(p, TMC429_IDX_REFCONF_RM(motor), 0x00, TMC429_NO_REF, 0x00);
		Write429Short(p, TMC429_IDX_VMIN(motor), 1);

		Write429Int(p, TMC429_IDX_VMAX(motor), 1000);
		SetAMax(p, motor, 1000);
	}
}

