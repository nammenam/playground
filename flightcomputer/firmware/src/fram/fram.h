/*
 * fram.h
 *
 * Created: 25.06.2023 15:26:57
 *  Author: Ralv
 */ 


#ifndef FRAM_H_
#define FRAM_H_


#include <cstdint>
#include <stdio.h>

#include "fram_common.h"


typedef union {
	uint8_t buffer[27];
	struct {
		uint8_t addr_from;
		uint8_t addr_to;
		uint8_t cmd;
		uint32_t context;
		uint8_t data[20];
	};
} FRAM_HEADER_t;


/*
	FRAM sensor interface functions
*/
void fram_setup(uint8_t this_fram_addr);
uint32_t fram_get_sensor_val(uint8_t addr, uint32_t context);


/*
	Common FRAM functions
*/
// Used to transfer arbitrary data to the FRAM-Bus module
void fram_send_buffer(uint8_t *data, uint8_t size);

// Write to a register
void fram_register_write(uint8_t addr, uint8_t data);

// Read a register
uint8_t fram_register_read(uint8_t addr);

// Save non-volatile registers
void fram_register_save();

// Tell FRAM-Bus module that this device is set-up and ready
void fram_ready();

// Reboot the FRAM-Bus module (caution)
void fram_reboot();

// Get the temperature on the FRAM-Bus module
int8_t fram_temperature_get();

// Read the STATUS register
uint8_t fram_read_status();

// Write to the status register
void fram_write_status(uint8_t data);



#endif /* FRAM_H_ */
