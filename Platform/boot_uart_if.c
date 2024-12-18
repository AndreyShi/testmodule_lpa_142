//--------------------------------------------------
// Интерфейс к UART связи с барьером,
// используется для обновления прошивки и
// для управления барьером во время тестирования
//--------------------------------------------------
#include "stm32f4xx_hal.h"

#include "app_config.h"
#include "boot_uart_if.h"
#include "boot_uart_private.h"
#include "common.h"
#include "firmware.h"
#include "gpio_if.h"
#include "relay_if.h"
//#include "menu.h"
#include "usart.h"

#include "modes.h"

//--------------------------------------------------
enum {/*{{{*/
    BOOT_0,
    BOOT_0A,
    BOOT_0B,
    BOOT_0C,
    BOOT_0D,
    BOOT_0E,
    BOOT_0F,

    BOOT_1,
    BOOT_2,
    BOOT_3,
    BOOT_4,
    BOOT_5,
    BOOT_5A,
    BOOT_5B,
    BOOT_6,
    BOOT_7,
    BOOT_8,
    BOOT_9,
    BOOT_10,
    BOOT_11,
    BOOT_12,
    BOOT_12A,
    BOOT_13,
    BOOT_14,
    BOOT_15,
    BOOT_16,
    BOOT_17,
    BOOT_18,
    BOOT_19,
    BOOT_20,
    BOOT_21,

    BOOT_50,
    BOOT_51,
    BOOT_52,
    BOOT_53,
} boot_state;/*}}}*/
//--------------------------------------------------
extern uint8_t active_firmware[TM_142_LPA_FW_SIZE];
//--------------------------------------------------
static const char lpa_modes[]="0123456789ABCDEF";

//--------------------------------------------------
static bool boot_uart_ready;
static bool boot_flag;
static uint8_t  boot_try;
static uint32_t boot_clock;

//--------------------------------------------------
static uint8_t exit_update(uint8_t err);
static void exit_bootloader(void);

//--------------------------------------------------
void boot_init(void)/*{{{*/
{
boot_state = BOOT_0;
boot_uart_ready = false;
boot_flag = false;
boot_clock = 0;
}/*}}}*/
//--------------------------------------------------
bool boot_update(void)/*{{{*/
{
const int timeout = 100;

uint8_t cmd;
uint8_t buff_out[50];
uint8_t buff_in [50];
uint32_t addr;

/* this is presumed */
boot_uart_ready = false;

/* hardware reset */
/* TODO: preserve the state of this relay */
/* check if it already has firmware */
while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0) { }
DISABLE_BOOT;
HAL_Delay(100);
while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0) { }
HAL_Delay(50);
//menu_task();
HAL_Delay(50);
//menu_task();

/* flush uart */
__HAL_UART_CLEAR_PEFLAG(&boot_uart);

addr = HAL_GetTick();
boot_uart.Instance->DR = DEFAULT_MODE;
while(1)
    {
    cmd = boot_uart.Instance->DR;
    if(cmd == 'O' || cmd == 'E')
	{
	exit_bootloader();
	return true;
	} // no need in firmware upgrade

    if(HAL_GetTick() - addr >= 5*timeout)
	{ break; }
    };

/* need firmware upload, go into bootloader */
while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0) { }
ENABLE_BOOT;
HAL_Delay(100);
while(relay_set(TM_142_RELAY_POWER, CH_1,  STATE_ON) == 0) { }
HAL_Delay(50);
//menu_task();
HAL_Delay(50);
//menu_task();

/* flush uart */
__HAL_UART_CLEAR_PEFLAG(&boot_uart);
/* contact bootloader {{{ */
buff_out[0] = 0x7F;
HAL_UART_Transmit(&boot_uart, buff_out, 1, 50);
if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) == HAL_OK)
    {
    /* bootloader error */
    if(buff_in[0] != REPLY_ACK &&
       buff_in[0] != REPLY_NACK)
	{ return false; }
    }
else
    {
    /* repeat this dummy byte in case bootloader is already tuned */
    HAL_UART_Transmit(&boot_uart, buff_out, 1, 50);

    if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
	{ return false; }

    /* expect NACK on wrong command */
    if(buff_in[0] != REPLY_NACK)
	{ return false; }
    }
/*}}}*/
/* step 0. get supported commands {{{ */
cmd = BOOT_GET;
buff_out[0] = cmd;
buff_out[1] = ~cmd;

HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
    { return false; }

if(buff_in[0] != REPLY_ACK)
    { return false; }

if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
    { return false; }

HAL_UART_Receive_DMA(&boot_uart, buff_in, buff_in[0] + 1);
HAL_Delay(100);

if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
    { return false; }

if(buff_in[0] != REPLY_ACK)
    { return false; }
/*}}}*/
/* update firmware */
/* step 1. erase {{{ */
cmd = BOOT_ERASE_EX;
buff_out[0] = cmd;
buff_out[1] = ~cmd;

HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
    { return false; }

/* simple case, just erase and done with it {{{ */
if(buff_in[0] == REPLY_ACK)
    { 
    buff_out[0] = 0xFF;
    buff_out[1] = 0xFF;
    buff_out[2] = 0x00;

    HAL_UART_Transmit_DMA(&boot_uart, buff_out, 3);
    if(HAL_UART_Receive(&boot_uart, buff_in,  1, 2000) != HAL_OK)
	{ return false; }

    if(buff_in[0] != REPLY_ACK)
	{ return false; }
    } /*}}}*/
/* read protection active, just disable it and reinit bootloader {{{ */
else if(buff_in[0] == REPLY_NACK)
    {
    cmd = BOOT_READ_UNPROT;
    buff_out[0] = cmd;
    buff_out[1] = ~cmd;

    HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
    if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
	{ return false; }

    if(buff_in[0] != REPLY_ACK)
	{ return false; }

    if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
	{ return false; }

    if(buff_in[0] != REPLY_ACK)
	{ return false; }

    /* erase takes a long time */
    HAL_Delay(2000);

    buff_out[0] = 0x7F;
    HAL_UART_Transmit(&boot_uart, buff_out, 1, 50);
    if(HAL_UART_Receive (&boot_uart, buff_in,  1, timeout) != HAL_OK)
	{ return false; }

    if(buff_in[0] != REPLY_ACK)
	{ return false; }
    }/*}}}*/
/*}}}*/
/* step 2. write {{{ */
for(uint32_t fw = 0; fw<TM_142_LPA_FW_SIZE; fw += 256)
    {
    led_tgl(TM_142_LED_GREEN);

    cmd = BOOT_WRITE_MEM;
    buff_out[0] = cmd;
    buff_out[1] = ~cmd;

    HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
    HAL_UART_Receive(&boot_uart, buff_in,  1, timeout);

    if(buff_in[0] != REPLY_ACK)
	{ return false; }

    addr = 0x08000000 + fw;
    buff_out[0] = (addr >> 24) & 0xFF;
    buff_out[1] = (addr >> 16) & 0xFF;
    buff_out[2] = (addr >>  8) & 0xFF;
    buff_out[3] = (addr      ) & 0xFF;
    buff_out[4] = buff_out[0] ^ buff_out[1] ^ buff_out[2] ^ buff_out[3];
    
    HAL_UART_Transmit_DMA(&boot_uart, buff_out, 5);
    if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
	{ return false; }

    if(buff_in[0] != REPLY_ACK)
	{ return false; }

    buff_out[0] = 0xFF;
    HAL_UART_Transmit(&boot_uart, buff_out, 1, 50);

    HAL_UART_Transmit_DMA(&boot_uart, (uint8_t *)(active_firmware + fw), 256);

    cmd = 0xFF; // now checksum
    for(uint16_t j = 0; j<256; j++)
	{ cmd ^= active_firmware[fw + j]; }

    HAL_Delay(80); // approx

    HAL_UART_Transmit(&boot_uart, &cmd, 1, 50);

    buff_in[0] = 0x00;
    if(HAL_UART_Receive(&boot_uart, buff_in,  1, 1000) != HAL_OK)
	{ break; }

    if(buff_in[0] != REPLY_ACK)
	{ break; }
    }
/*}}}*/
/* step 3. read protect {{{ */
cmd = BOOT_READ_PROT;
buff_out[0] = cmd;
buff_out[1] = ~cmd;

HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
    { return false; }

if(buff_in[0] != REPLY_ACK)
    { return false; }

if(HAL_UART_Receive(&boot_uart, buff_in,  1, timeout) != HAL_OK)
    { return false; }

if(buff_in[0] != REPLY_ACK)
    { return false; }
/*}}}*/
/* let automatic reset bring us into firmware */
exit_bootloader();
return true;
}/*}}}*/
//--------------------------------------------------
uint8_t boot_update_nb(void)/*{{{*/
{
const uint32_t  boot_timeout = 1000;
const uint32_t erase_timeout = 2000;
const uint32_t  recv_timeout = 1000;

static uint8_t  cmd;
static uint8_t  crc;
static uint8_t  buff_in[50];
static uint8_t  buff_out[50];
static uint32_t fw;

uint32_t addr;

switch(boot_state)
    {
    case BOOT_0:
	if(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0)
	    { break; }

	DISABLE_BOOT;
	boot_state = BOOT_0A;
	boot_uart_ready = false;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_0A:
	if(HAL_GetTick() - boot_clock < boot_timeout)
	    { break; }

	boot_state = BOOT_0B;
	break;

    case BOOT_0B:
	if(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0)
	    { break; }

	boot_state = BOOT_0C;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_0C:
	if(HAL_GetTick() - boot_clock < boot_timeout)
	    { break; }

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);

	boot_uart.Instance->DR = DEFAULT_MODE;
	boot_state = BOOT_0D;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_0D:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{
		boot_try = 0;
		boot_state = BOOT_1; // go reload firmware
		break;
		}

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd == 'O' || cmd == 'E')
	    { boot_state = BOOT_0E; }
	break;

    case BOOT_0E:
	if(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0)
	    { break; }

	DISABLE_BOOT;
	boot_state = BOOT_0F;
	break;

    case BOOT_0F:
	if(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0)
	    { break; }

	boot_uart_ready = true;
	return exit_update(255);

    case BOOT_1:
	if(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0)
	    { break; }

	ENABLE_BOOT;
	boot_state = BOOT_2;
	boot_uart_ready = false;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_2:
	if(HAL_GetTick() - boot_clock < boot_timeout)
	    { break; }

	boot_state = BOOT_3;
	break;

    case BOOT_3:
	if(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0)
	    { break; }

	boot_state = BOOT_4;
	boot_clock = HAL_GetTick();
	break;

    /* do this two times */
    case BOOT_4:
	if(HAL_GetTick() - boot_clock < boot_timeout)
	    { break; }

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);

	boot_uart.Instance->DR = 0x7F;
	boot_state = BOOT_5;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_5:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{
		boot_state = BOOT_1; // try harder!
		boot_try++;
		if(boot_try > 2)
		    { return exit_update(250); }

		break;
		}

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd == REPLY_ACK ||
	   cmd == REPLY_NACK)
	    {
	    boot_state = BOOT_5B;
	    break;
	    }

	/* repeat once more */
	__HAL_UART_CLEAR_PEFLAG(&boot_uart);

	boot_uart.Instance->DR = 0x7F;
	boot_state = BOOT_5A;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_5A:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK &&
	   cmd != REPLY_NACK)
	    { return exit_update(1); }

	boot_state = BOOT_5B;
	break;

    /* get supported commands, just to see if it works ok */
    case BOOT_5B:
	cmd = BOOT_GET;
	buff_out[0] = cmd;
	buff_out[1] = ~cmd;

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);

	HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
	boot_state = BOOT_6;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_6:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	boot_state = BOOT_7;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_7:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	HAL_UART_Receive_DMA(&boot_uart, buff_in, cmd + 1);
	boot_state = BOOT_8;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_8:
	if( !boot_flag )
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	boot_state = BOOT_9;
	boot_clock = HAL_GetTick();
	break;

    /* step 0. issue erase command */
    case BOOT_9:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	cmd = BOOT_ERASE_EX;
	buff_out[0] = cmd;
	buff_out[1] = ~cmd;

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);
	HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
	boot_state = BOOT_10;
	boot_clock = HAL_GetTick();
	break;

    /* step 1. erase */
    case BOOT_10:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	/* simple case, just erase and done with it */
	cmd = boot_uart.Instance->DR;
	if(cmd == REPLY_ACK)
	    {
	    buff_out[0] = 0xFF;
	    buff_out[1] = 0xFF;
	    buff_out[2] = 0x00;

	    __HAL_UART_CLEAR_PEFLAG(&boot_uart);
	    HAL_UART_Transmit_DMA(&boot_uart, buff_out, 3);

	    boot_state = BOOT_11;
	    }
	else /* read protection active, just disable it and reinit bootloader */
	    {
	    cmd = BOOT_READ_UNPROT;
	    buff_out[0] = cmd;
	    buff_out[1] = ~cmd;

	    __HAL_UART_CLEAR_PEFLAG(&boot_uart);
	    HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
	    boot_state = BOOT_50;
	    }

	boot_clock = HAL_GetTick();
	break;

    case BOOT_11:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	boot_state = BOOT_12;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_50:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	boot_state = BOOT_52;
	boot_clock = HAL_GetTick();
	break;

    /* wait for erase to complete */
    case BOOT_52:
	if(HAL_GetTick() - boot_clock < erase_timeout)
	    { break; }

	/* reinitialize bootloader */
	__HAL_UART_CLEAR_PEFLAG(&boot_uart);

	boot_uart.Instance->DR = 0x7F;
	boot_state = BOOT_53;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_53:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	boot_state = BOOT_12;
	break;

    /* step 2. write */
    case BOOT_12:
	fw = 0;
	addr = 0;
	boot_state = BOOT_12A;
	break;

    case BOOT_12A:
	cmd = BOOT_WRITE_MEM;
	buff_out[0] = cmd;
	buff_out[1] = ~cmd;

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);
	HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
	boot_state = BOOT_13;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_13:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	addr = 0x08000000 + fw;
	buff_out[0] = (addr >> 24) & 0xFF;
	buff_out[1] = (addr >> 16) & 0xFF;
	buff_out[2] = (addr >>  8) & 0xFF;
	buff_out[3] = (addr      ) & 0xFF;
	buff_out[4] = buff_out[0] ^ buff_out[1] ^ buff_out[2] ^ buff_out[3];

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);
	HAL_UART_Transmit_DMA(&boot_uart, buff_out, 5);
	boot_state = BOOT_14;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_14:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);

	boot_uart.Instance->DR = 0xFF;
	boot_state = BOOT_15;
	break;

    case BOOT_15:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_TC) != UART_FLAG_TC)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);
	HAL_UART_Transmit_DMA(&boot_uart, (uint8_t *)(active_firmware + fw), 256);
	boot_state = BOOT_16;
	break;

    case BOOT_16:
	crc = 0xFF;
	for(uint16_t j=0; j<256; j++)
	    { crc ^= active_firmware[fw + j]; }

	boot_clock = HAL_GetTick();
	boot_state = BOOT_17;
	break;

    case BOOT_17:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_TC) != UART_FLAG_TC)
	    {
	    if(HAL_GetTick() - boot_clock >= 100)
		{ return exit_update(250); }

	    break;
	    }

	/* transmit checksum */
	__HAL_UART_CLEAR_PEFLAG(&boot_uart);

	boot_uart.Instance->DR = crc;
	boot_state = BOOT_18;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_18:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	fw += 256;
	if(fw < TM_142_LPA_FW_SIZE)
	    { boot_state = BOOT_12A; }
	else
	    { boot_state = BOOT_19; }
	break;

    /* step 3. read protect */
    case BOOT_19:
	cmd = BOOT_READ_PROT;
	buff_out[0] = cmd;
	buff_out[1] = ~cmd;

	__HAL_UART_CLEAR_PEFLAG(&boot_uart);
	HAL_UART_Transmit_DMA(&boot_uart, buff_out, 2);
	boot_state = BOOT_20;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_20:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }

	    break;
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	boot_state = BOOT_21;
	boot_clock = HAL_GetTick();
	break;

    case BOOT_21:
	cmd = boot_uart.Instance->SR;
	if((cmd & UART_FLAG_RXNE) != UART_FLAG_RXNE)
	    {
	    if(HAL_GetTick() - boot_clock >= recv_timeout)
		{ return exit_update(250); }
	    }

	cmd = boot_uart.Instance->DR;
	if(cmd != REPLY_ACK)
	    { return exit_update(1); }

	DISABLE_BOOT;
	boot_uart_ready = true;
	return exit_update(255);
    };
return 0;
}/*}}}*/
//--------------------------------------------------
bool set_lpa_mode(uint8_t mode)/*{{{*/
{
if( !boot_uart_ready )
    { return true; }

if(HAL_UART_GetState(&boot_uart) != HAL_UART_STATE_READY)
    { return false; }

/* flush uart */
__HAL_UART_CLEAR_PEFLAG(&boot_uart);

mode = mode & MODE_MASK;
mode = lpa_modes[mode];
HAL_UART_Transmit(&boot_uart, &mode, 1, 50);
if(HAL_UART_Receive(&boot_uart, &mode, 1, 500) != HAL_OK)
    { return false; }

if(mode == 'O')
    { return true; }
else
    { return false; }
}/*}}}*/
//--------------------------------------------------
static uint8_t exit_update(uint8_t err)/*{{{*/
{
boot_state = BOOT_0;
boot_flag = false;
return err;
}/*}}}*/
//--------------------------------------------------
static void exit_bootloader(void)/*{{{*/
{
DISABLE_BOOT;
HAL_Delay(100);

/* TODO: restore the state of lpa power relay */
boot_uart_ready = true;
}/*}}}*/
//--------------------------------------------------
void boot_uart_rx_isr()
{ boot_flag = true; }
//--------------------------------------------------
