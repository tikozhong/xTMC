/******************** (C) COPYRIGHT 2015 INCUBECN *****************************
* File Name          : inputCmd.c
* Author             : Tiko Zhong
* Date First Issued  : 12/01/2015
* Description        : This file provides a set of functions needed to manage the
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "tmcCmd.h"
#include "tmc5160.h"
#include "string.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const char TMC_HELP[] = {
	"input command:"
	"\n %brd.%dev.help()"
	"\n %brd.%dev.readPin()/(indx)/(indx0,indx1)"
	"\n %brd.%dev.enableFalling()/(indx)"
	"\n %brd.%dev.disableFalling()/(indx)"
	"\n %brd.%dev.enableRaising()/(indx)"
	"\n %brd.%dev.disableRaising()/(indx)"
	"\n"
};

/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name  : inputCmd
* Description    : gpio function commands
* Input          : 
									: 
* Output         : None
* Return         : None 
*******************************************************************************/
u8 tmcCmd(void *p, char* CMD, u8 brdAddr, void (*xprint)(const char* FORMAT_ORG, ...)){
	s32 i,j;
	const char* line;
	TMC5160_DEV* dev = p;
	TMC5160_RSRC *pRsrc = &dev->rsrc;

	if(strncmp(CMD, pRsrc->name, strlen(pRsrc->name)) != 0)	return 0;
	line = &CMD[strlen(pRsrc->name)];

	//.help()
	if(strncmp(line, ".help", strlen(".help")) == 0){
		xprint("+ok@%d.%s.help()\r\n%s", brdAddr, pRsrc->name, TMC_HELP);
		return 1;
	}

	//.reg(addr,val)
	else if(sscanf(line, ".reg %d %d", &i, &j)==2){
		dev->writeRegister(pRsrc, i, j);
		xprint("+ok@%d.%s.reg(0x%08x,0x%08x)\r\n", brdAddr, pRsrc->name, i, j);
		return 1;
	}
	//.reg(addr)
	else if(sscanf(line, ".reg %d", &i)==1){
		dev->readRegister(pRsrc, (u8)(0xff&i), &j);
		xprint("+ok@%d.%s.reg(0x%08x,0x%08x)\r\n", brdAddr, pRsrc->name, i, j);
		return 1;
	}

	// .stop
	else if(strncmp(line, ".stop", strlen(".stop")) == 0){
		dev->stop(pRsrc);
		xprint("+ok@%d.%s.stop()\r\n", brdAddr, pRsrc->name);
		return 1;
	}

	// .run vel
	else if(sscanf(line, ".run %d", &i)==1){
		dev->rotate(pRsrc, i);
		xprint("+ok@%d.%s.run(%d)\r\n", brdAddr, pRsrc->name, i);
		return 1;
	}

	// .moveto pos
	else if(sscanf(line, ".moveto %d", &i)==1){
		dev->moveTo(pRsrc, i);
		xprint("+ok@%d.%s.movto(%d)\r\n", brdAddr, pRsrc->name, i);
		return 1;
	}

	else{
		xprint("+unknown@%s", CMD);
		return 1;
	}
	return 0;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
