#include "string.h"
#include "TMC5160.h"
#include "tmc5160_dev.h"


#define ERRORS_VM        (1<<0)
#define ERRORS_VM_UNDER  (1<<1)
#define ERRORS_VM_OVER   (1<<2)

#define VM_MIN         50   // VM[V/10] min
#define VM_MAX         660  // VM[V/10] max

#define DEFAULT_MOTOR  0

#define TMC5160_TIMEOUT 50 // UART Timeout in ms

//static bool vMaxModified = false;
//static uint32_t vMax		   = 1;

//static bool uart_mode = false;

static uint32_t tmc5160_right(TMC5160_RSRC *rsrc, int32_t velocity);
static uint32_t tmc5160_left(TMC5160_RSRC *rsrc, int32_t velocity);
static uint32_t tmc5160_rotate(TMC5160_RSRC *rsrc, int32_t velocity);
static uint32_t tmc5160_stop(TMC5160_RSRC *rsrc);
static uint32_t tmc5160_moveTo(TMC5160_RSRC *rsrc, int32_t position);
static uint32_t tmc5160_moveBy(TMC5160_RSRC *rsrc, int32_t *ticks);
static uint32_t tmc5160_GAP(TMC5160_RSRC *rsrc, uint8_t type, int32_t *value);
static uint32_t tmc5160_SAP(TMC5160_RSRC *rsrc, uint8_t type, int32_t value);
static void tmc5160_readRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t *value);
static void tmc5160_writeRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t value);
static uint32_t tmc5160_getMeasuredSpeed(TMC5160_RSRC *rsrc, int32_t *value);

void tmc5160_writeDatagram(TMC5160_RSRC *rsrc, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
void tmc5160_writeInt(TMC5160_RSRC *rsrc, uint8_t address, int value);
int tmc5160_readInt(TMC5160_RSRC *rsrc, uint8_t address);
static void tmc5160_writeDatagram_spi(TMC5160_RSRC *rsrc, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4);
static int32_t tmc5160_readInt_spi(TMC5160_RSRC *rsrc, uint8_t address);

static void tmc5160_periodicJob(TMC5160_RSRC *rsrc, uint32_t tick);
static void tmc5160_checkErrors(TMC5160_RSRC *rsrc, uint32_t tick);
static void tmc5160_deInit(TMC5160_RSRC *rsrc);

static uint8_t tmc5160_reset(TMC5160_RSRC *rsrc);

//static TMC5160TypeDef TMC5160;
//static ConfigurationTypeDef *TMC5160_config;

void setupDev_tmc5160(TMC5160_DEV *dev, bool con_mode){

	// reg funtions
	dev->right = tmc5160_right;
	dev->left = tmc5160_left;
	dev->rotate = tmc5160_rotate;
	dev->stop = tmc5160_stop;
	dev->moveTo = tmc5160_moveTo;
	dev->moveBy = tmc5160_moveBy;
	dev->GAP = tmc5160_GAP;
	dev->SAP = tmc5160_SAP;
	dev->readRegister = tmc5160_readRegister;
	dev->writeRegister = tmc5160_writeRegister;
	dev->getMeasuredSpeed = tmc5160_getMeasuredSpeed;
	dev->periodicJob = tmc5160_periodicJob;
	dev->checkErrors = tmc5160_checkErrors;
	dev->deInit = tmc5160_deInit;
//	dev->userFunction = tmc5160_userFunction;
	dev->reset = tmc5160_reset;

}

void tmc5160_writeDatagram(TMC5160_RSRC *rsrc, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
	u8 txBuf[5], rxBuf[5]={0};
	address = TMC_ADDRESS(address);

	txBuf[0] = address|0x80;
	txBuf[1] = x1;
	txBuf[2] = x2;
	txBuf[3] = x3;
	txBuf[4] = x4;

	while (HAL_SPI_GetState(rsrc->SPI_HANDLE) != HAL_SPI_STATE_READY);
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(rsrc->SPI_HANDLE, txBuf, rxBuf, 5, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_SET);

	int value = x1;
	value <<= 8;
	value |= x2;
	value <<= 8;
	value |= x3;
	value <<= 8;
	value |= x4;

	if(rsrc->TMC5160_config)
		rsrc->TMC5160_config->shadowRegister[address] = value;
}

void tmc5160_writeInt(TMC5160_RSRC *rsrc, uint8_t address, int value)
{
	tmc5160_writeDatagram(rsrc, address, 0xFF & (value>>24), 0xFF & (value>>16), 0xFF & (value>>8), 0xFF & (value>>0));
}

int tmc5160_readInt(TMC5160_RSRC *rsrc, uint8_t address)
{
	u8 txBuf[5], rxBuf[5]={0};
	int value = 0;
	address = TMC_ADDRESS(address);

	// Register not readable -> shadow register copy
	if(!TMC_IS_READABLE(rsrc->TMC5160.registerAccess[address]))
		return rsrc->TMC5160_config->shadowRegister[address];

	txBuf[0] = address;
	txBuf[1] = 0;
	txBuf[2] = 0;
	txBuf[3] = 0;
	txBuf[4] = 0;

	while (HAL_SPI_GetState(rsrc->SPI_HANDLE) != HAL_SPI_STATE_READY);
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(rsrc->SPI_HANDLE, txBuf, rxBuf, 5, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_SET);

	miscDelay(100);
	memset(rxBuf, 0, 5);

	while (HAL_SPI_GetState(rsrc->SPI_HANDLE) != HAL_SPI_STATE_READY);
	/* nSS signal activation - low */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_RESET);
	/* SPI byte send */
	HAL_SPI_TransmitReceive(rsrc->SPI_HANDLE, txBuf, rxBuf, 5, 2);
	/* nSS signal deactivation - high */
	HAL_GPIO_WritePin(rsrc->CS->GPIOx, rsrc->CS->GPIO_Pin, GPIO_PIN_SET);

	value = rxBuf[1];	value <<= 8;
	value |= rxBuf[2];	value <<= 8;
	value |= rxBuf[3];	value <<= 8;
	value |= rxBuf[4];

	return value;
}

static uint32_t tmc5160_rotate(TMC5160_RSRC *rsrc, int32_t velocity)
{
	rsrc->vMaxModified = true;

	// set absolute velocity, independant from direction
	tmc5160_writeInt(rsrc, TMC5160_VMAX, abs(velocity));

	// signdedness defines velocity mode direction bit in rampmode register
	tmc5160_writeDatagram(rsrc, TMC5160_RAMPMODE, 0, 0, 0, (velocity >= 0)? 1 : 2);

	return TMC_ERROR_NONE;
}

static uint32_t tmc5160_right(TMC5160_RSRC *rsrc, int32_t velocity)
{
	return tmc5160_rotate(rsrc, velocity);
}

static uint32_t tmc5160_left(TMC5160_RSRC *rsrc, int32_t velocity)
{
	return tmc5160_rotate(rsrc, -velocity);
}

static uint32_t tmc5160_stop(TMC5160_RSRC *rsrc)
{
	return tmc5160_rotate(rsrc, 0);
}

static uint32_t tmc5160_moveTo(TMC5160_RSRC *rsrc, int32_t position)
{
	if(rsrc->vMaxModified)
	{
		tmc5160_writeInt(rsrc, TMC5160_VMAX, rsrc->TMC5160_config->shadowRegister[TMC5160_VMAX]);
		rsrc->vMaxModified = false;
	}

	// set position
	tmc5160_writeInt(rsrc, TMC5160_XTARGET, position);

	// change to positioning mode
	tmc5160_writeDatagram(rsrc, TMC5160_RAMPMODE, 0, 0, 0, 0);

	return TMC_ERROR_NONE;
}

static uint32_t tmc5160_moveBy(TMC5160_RSRC *rsrc, int32_t *ticks)
{
	// determine actual position and add numbers of ticks to move
	*ticks = tmc5160_readInt(rsrc, TMC5160_XACTUAL) + *ticks;

	return moveTo(rsrc, *ticks);
}

static uint32_t tmc5160_SAP(uint8_t type, uint8_t motor, int32_t value)
{
	return handleParameter(WRITE, motor, type, &value);
}

static uint32_t GAP(uint8_t type, uint8_t motor, int32_t *value)
{
	return handleParameter(READ, motor, type, value);
}

static uint32_t tmc5160_getMeasuredSpeed(TMC5160_RSRC *rsrc, int32_t *value)
{
	*value = rsrc->TMC5160.velocity;
	return TMC_ERROR_NONE;
}

static void tmc5160_writeRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t value)
{
	tmc5160_writeInt(DEFAULT_MOTOR, address, value);
}

static void tmc5160_readRegister(TMC5160_RSRC *rsrc, uint8_t address, int32_t *value)
{
	*value = tmc5160_readInt(DEFAULT_MOTOR, address);
}

static void tmc5160_periodicJob(TMC5160_RSRC *rsrc, uint32_t tick)
{
	tmc5160_periodicJob(motor, tick, &TMC5160, TMC5160_config);
}

static void tmc5160_checkErrors(uint32_t tick)
{
	UNUSED(tick);
	Evalboards.ch1.errors = 0;
}

static void tmc5160_deInit(void)
{
	HAL.IOs->config->setLow(Pins.DRV_ENN_CFG6);
	HAL.IOs->config->setLow(Pins.SD_MODE);
	HAL.IOs->config->setLow(Pins.SPI_MODE);
	HAL.IOs->config->reset(Pins.ENCA_DCIN_CFG5);
	HAL.IOs->config->reset(Pins.ENCB_DCEN_CFG4);
	HAL.IOs->config->reset(Pins.ENCN_DCO);
	HAL.IOs->config->reset(Pins.REFL_UC);
	HAL.IOs->config->reset(Pins.REFR_UC);
	HAL.IOs->config->reset(Pins.SWN_DIAG0);
	HAL.IOs->config->reset(Pins.SWP_DIAG1);
	HAL.IOs->config->reset(Pins.DRV_ENN_CFG6);
	HAL.IOs->config->reset(Pins.SD_MODE);
	HAL.IOs->config->reset(Pins.SPI_MODE);
};

static uint8_t tmc5160_reset()
{
	if(!tmc5160_readInt(0, TMC5160_VACTUAL))
		tmc5160_reset(TMC5160_config);

	HAL.IOs->config->toInput(Pins.REFL_UC);
	HAL.IOs->config->toInput(Pins.REFR_UC);

	return 1;
}

static uint8_t tmc5160_restore()
{
	return tmc5160_restore(TMC5160_config);
}

static void tmc5160_enableDriver(DriverState state)
{
	if(state == DRIVER_USE_GLOBAL_ENABLE)
		state = Evalboards.driverEnable;

	if(state ==  DRIVER_DISABLE)
		HAL.IOs->config->setHigh(Pins.DRV_ENN_CFG6);
	else if((state == DRIVER_ENABLE) && (Evalboards.driverEnable == DRIVER_ENABLE))
		HAL.IOs->config->setLow(Pins.DRV_ENN_CFG6);
}

void TMC5160_init(void)
{
	tmc5160_initConfig(&TMC5160);

	Pins.DRV_ENN_CFG6    = &HAL.IOs->pins->DIO0;
	Pins.ENCN_DCO        = &HAL.IOs->pins->DIO1;
	Pins.ENCA_DCIN_CFG5  = &HAL.IOs->pins->DIO2;
	Pins.ENCB_DCEN_CFG4  = &HAL.IOs->pins->DIO3;
	Pins.REFL_UC         = &HAL.IOs->pins->DIO6;
	Pins.REFR_UC         = &HAL.IOs->pins->DIO7;
	Pins.SD_MODE         = &HAL.IOs->pins->DIO9;
	Pins.SPI_MODE        = &HAL.IOs->pins->DIO11;
	Pins.SWP_DIAG1       = &HAL.IOs->pins->DIO15;
	Pins.SWN_DIAG0       = &HAL.IOs->pins->DIO16;

	HAL.IOs->config->toOutput(Pins.DRV_ENN_CFG6);
	HAL.IOs->config->toOutput(Pins.SD_MODE);
	HAL.IOs->config->toOutput(Pins.SPI_MODE);

	HAL.IOs->config->setHigh(Pins.DRV_ENN_CFG6);
	HAL.IOs->config->setLow(Pins.SD_MODE);
	HAL.IOs->config->setHigh(Pins.SPI_MODE);

	HAL.IOs->config->toInput(Pins.ENCN_DCO);
	HAL.IOs->config->toInput(Pins.ENCB_DCEN_CFG4);
	HAL.IOs->config->toInput(Pins.ENCA_DCIN_CFG5);
	HAL.IOs->config->toInput(Pins.SWN_DIAG0);
	HAL.IOs->config->toInput(Pins.SWP_DIAG1);
	HAL.IOs->config->toInput(Pins.REFL_UC);
	HAL.IOs->config->toInput(Pins.REFR_UC);

	// Disable CLK output -> use internal 12 MHz clock
	// Switchable via user function
//	HAL.IOs->config->toOutput(&HAL.IOs->pins->CLK16);
//	HAL.IOs->config->setLow(&HAL.IOs->pins->CLK16);

	init_comm((uart_mode) ? TMC_COMM_UART : TMC_COMM_SPI);

	TMC5160_config = Evalboards.ch1.config;

	Evalboards.ch1.config->reset        = reset;
	Evalboards.ch1.config->restore      = restore;
	Evalboards.ch1.config->state        = CONFIG_RESET;
	Evalboards.ch1.config->configIndex  = 0;

	Evalboards.ch1.rotate               = rotate;
	Evalboards.ch1.right                = right;
	Evalboards.ch1.left                 = left;
	Evalboards.ch1.stop                 = stop;
	Evalboards.ch1.GAP                  = GAP;
	Evalboards.ch1.SAP                  = SAP;
	Evalboards.ch1.moveTo               = moveTo;
	Evalboards.ch1.moveBy               = moveBy;
	Evalboards.ch1.writeRegister        = writeRegister;
	Evalboards.ch1.readRegister         = readRegister;
	Evalboards.ch1.periodicJob          = periodicJob;
	Evalboards.ch1.userFunction         = userFunction;
	Evalboards.ch1.getMeasuredSpeed     = getMeasuredSpeed;
	Evalboards.ch1.enableDriver         = enableDriver;
	Evalboards.ch1.checkErrors          = checkErrors;
	Evalboards.ch1.numberOfMotors       = TMC5160_MOTORS;
	Evalboards.ch1.VMMin                = VM_MIN;
	Evalboards.ch1.VMMax                = VM_MAX;
	Evalboards.ch1.deInit               = deInit;

	enableDriver(DRIVER_USE_GLOBAL_ENABLE);
};
