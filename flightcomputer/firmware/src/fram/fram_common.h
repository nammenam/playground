/*
 * fram_common.h
 *
 * Created: 25.06.2023 15:27:05
 *  Author: Ralv
 */ 


#ifndef FRAM_COMMON_H_
#define FRAM_COMMON_H_


/*
	Commands
*/
// #define FRAM_TARGET_CMD_SET		0x01 // Not applicable for sensor device
#define FRAM_TARGET_CMD_GET			0x02
// #define FRAM_TARGET_CMD_STATUS	0x03 // Not applicable for sensor device
#define FRAM_TARGET_CMD_WRITE		0x04 // Not applicable for sensor device, but used for WRITE demo
#define FRAM_TARGET_CMD_READ		0x05 // Not applicable for sensor device, but used for READ demo
#define FRAM_TARGET_CMD_ABORT		0x10
#define FRAM_TARGET_CMD_ARM			0x11
#define FRAM_TARGET_CMD_LAUNCH		0x12

#define FRAM_TARGET_CMD_REG_WRITE	0xF0
#define FRAM_TARGET_CMD_REG_READ	0xF1
#define FRAM_TARGET_CMD_REG_SAVE	0xF2
#define FRAM_TARGET_CMD_READY		0xF3
#define FRAM_TARGET_CMD_REBOOT		0xF4

/*
	Devices
*/
// #define FRAM_DEVICE_ACTUATOR		0x00
// #define FRAM_DEVICE_SENSOR		0x01
#define FRAM_DEVICE_OPERATOR		0x03

/*
	Non-volatile registers
*/
#define FRAM_REGISTER_ADDR			0x00						// Read/Write	:	FRAM Address
#define FRAM_REGISTER_MULTI_ADDR_0	0x01						// Read/Write	:	Alias address 0 for multi-address systems
#define FRAM_REGISTER_MULTI_ADDR_1	0x02						// Read/Write	:	Alias address 1 for multi-address systems
#define FRAM_REGISTER_MULTI_ADDR_2	0x03						// Read/Write	:	Alias address 2 for multi-address systems
#define FRAM_REGISTER_MULTI_ADDR_3	0x04						// Read/Write	:	Alias address 3 for multi-address systems
#define FRAM_REGISTER_MULTI_ADDR_4	0x05						// Read/Write	:	Alias address 4 for multi-address systems
#define FRAM_REGISTER_DEVICE_TYPE	0x06						// Read/Write	:	FRAM module device class

/*
	Volatile registers
*/
#define FRAM_REGISTER_STATUS		0x10						// See 'Status register bits'
#define FRAM_REGISTER_TEMPERATURE	0x11						// Read only	:	Temperature of the FRAM-Bus module in Celsius

/*
	Status register bits
*/
#define FRAM_STATUS_TARGET_OK_bm					(1 << 7)	// Read/Write	:	A '1' signifies that this device is OK (All good)
#define FRAM_STATUS_TARGET_FAULT_OUTPUT_bm			(1 << 6)	// Read/Write	:	A '1' signifies that this devices output is faulty
#define FRAM_STATUS_TARGET_FAULT_INPUT_bm			(1 << 5)	// Read/Write	:	A '1' signifies that this devices input is faulty
#define FRAM_STATUS_TARGET_FAULT_CRITICAL_bm		(1 << 4)	// Read/Write	:	A '1' signifies that there is a critical fault in this device
#define FRAM_STATUS_MODULE_FAULT_OUTPUT_POWER_bm	(1 << 3)	// Read only	:	A '1' signifies that the output power is turned off
#define FRAM_STATUS_MODULE_FAULT_VOLTAGE_TARGET_bm	(1 << 2)	// Read only	:	A '1' signifies that the target voltage supply is below 1.8 V
#define FRAM_STATUS_MODULE_FAULT_CONNECTION_bm		(1 << 1)	// Read only	:	A '1' signifies that the target is ready for communication on the FRAM-Bus
#define FRAM_STATUS_MODULE_FAULT_TEMPERATURE_bm		(1 << 0)	// Read only	:	A '1' signifies that the FRAM-Bus module is too hot (x > 100 degrees Celsius)




#endif /* FRAM_COMMON_H_ */
