/*
 * fram.cpp
 *
 * Created: 25.06.2023 15:26:50
 *  Author: Ralv
 */ 

#include "component/sercom.h"
#include "fram_common.h"
#include "fram.h"
#include "peripheral/sercom/usart/plib_sercom3_usart.h"

volatile uint8_t fram_read_flag = 0, fram_read_data, fram_index = 0;
FRAM_HEADER_t fram_rx;


volatile uint8_t fram_get_flag = 0, fram_get_size, fram_get_index = 0;
uint8_t fram_get_data[10];



void fram_write_callback(uint32_t* context)
{
    fram_get_flag = 1;
}



void fram_read_callback(uint32_t* context)
{
    if(SERCOM3_USART_ErrorGet() != USART_ERROR_NONE)
    {
        /* ErrorGet clears errors, set error flag to notify console */
        errorStatus = true;
    }
    else
    {
        fram_read_flag = 1;
    }
}





void fram_setup(uint8_t this_fram_addr)
{
    /* Register callback functions and send start message */
    SERCOM3_USART_WriteCallbackRegister(fram_write_callback, 0);
    SERCOM3_USART_ReadCallbackRegister(fram_read_callback, 0);
    SERCOM3_USART_Write(&messageStart[0], sizeof(messageStart));
	
	
	uint8_t programmed_device = fram_register_read(FRAM_REGISTER_DEVICE_TYPE);
	uint8_t programmed_address = fram_register_read(FRAM_REGISTER_ADDR);

	// Check if FRAM-Bus module is programmed
	if (programmed_device != FRAM_DEVICE_OPERATOR)
	{
		fram_register_write(FRAM_REGISTER_DEVICE_TYPE, FRAM_DEVICE_OPERATOR);
		
		if (programmed_address != this_fram_addr)
		{
			fram_register_write(FRAM_REGISTER_ADDR, this_fram_addr);
		}
		
		fram_register_save();
		fram_reboot();
		_delay_ms(100);
	}
	else if (programmed_address != this_fram_addr)
	{
		fram_register_write(FRAM_REGISTER_ADDR, this_fram_addr);
		fram_register_save();
		fram_reboot();
		_delay_ms(100);
	}
	
	_delay_ms(10);
	fram_ready();
}

uint32_t fram_get_sensor_val(uint8_t addr, uint32_t context)
{
	static uint8_t tx_buf[10];
	uint16_t timeout_us = 0;
	uint32_t sen_val = 0;
	
	tx_buf[0] = FRAM_TARGET_CMD_GET;
	tx_buf[1] = addr;
	tx_buf[2] = (uint8_t)((context & 0xFF000000) >> 24);
	tx_buf[3] = (uint8_t)((context & 0x00FF0000) >> 16);
	tx_buf[4] = (uint8_t)((context & 0x0000FF00) >> 8);
	tx_buf[5] = (uint8_t)((context & 0x000000FF));
	
	fram_get_flag = 4;
	fram_get_index = 0;
	
	fram_send_buffer(tx_buf, 6);
	
	while (fram_get_flag)
	{
		_delay_us(1);
		if (timeout_us < 9999)
		{
			timeout_us++;
		}
		else
		{
			// Timed out
			fram_get_flag = 0;
			return 0xFFFFFFFF;
		}
	}
	
	sen_val |= (uint32_t)fram_get_data[3] << 24;
	sen_val |= (uint32_t)fram_get_data[2] << 16;
	sen_val |= (uint32_t)fram_get_data[1] << 8;
	sen_val |= (uint32_t)fram_get_data[0];
	
	return sen_val;
}




void fram_send_buffer(uint8_t *data, uint8_t size)
{
    // write operation
}


void fram_register_write(uint8_t addr, uint8_t data)
{
    // write operation
	FRAM_TARGET_CMD_REG_WRITE;
}



uint8_t fram_register_read(uint8_t addr)
{
    // read operation
	uint16_t timeout_us = 0;
	fram_read_flag = 1;

    FRAM_TARGET_CMD_REG_READ;
	
	
	while (fram_read_flag)
	{
		if (timeout_us > 9999)
		{
			return 0xFF;
		}
		timeout_us++;
	}
	return fram_read_data;
}



void fram_register_save()
{
    // write operation
	FRAM_TARGET_CMD_REG_SAVE;
}

void fram_ready()
{
    // write operation
    FRAM_TARGET_CMD_READY;
}

void fram_reboot()
{
    // write operation
    FRAM_TARGET_CMD_REBOOT;
}



int8_t fram_temperature_get()
{
	int8_t temperature_c;
	
	temperature_c = (int8_t)fram_register_read(FRAM_REGISTER_TEMPERATURE);
	
	return temperature_c;
}

uint8_t fram_read_status()
{
	uint8_t status;
	
	status = fram_register_read(FRAM_REGISTER_STATUS);
	
	return status;
}

void fram_write_status(uint8_t data)
{
	fram_register_write(FRAM_REGISTER_STATUS, data);
}
