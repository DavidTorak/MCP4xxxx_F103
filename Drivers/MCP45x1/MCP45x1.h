/*
 * MCP45x1.h
 *
 *  Created on: Mar 8, 2023
 *      Author: DavidTorak
 */


// https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/22096b.pdf

/*
 * 	8-bit Device          	RS = Rab/(256)
 * 	7-bit Device			RS = Rab/(127)
 *
 * 	Resistance 		 Typical		Default POR				 Wiper Code
 * 	  Code			RAB Value		Wiper Setting			8-bit 	7-bit
 * 	  502				5k0			 Mid-scale				80h		40h
 * 	  103				10k			 Mid-scale				80h		40h
 * 	  503				50k			 Mid-scale				80h		40h
 * 	  104			   100k			 Mid-scale				80h		40h
 *
 *
 *		RWB CALCULATION
 *		8-bit Device        R-WB = ((R-AB*N)/256)+RW    N = 0 to 256 (decimal)
 *		7-bit Device        R-WB = ((R-AB*N)/127)+RW    N = 0 to 127 (decimal)
 */
#ifndef MCP45X1_MCP45X1_H_
#define MCP45X1_MCP45X1_H_

#include "MojeFunkce.h"
#include "MojeMacro.h"

// I2C Address of device
#define MCP45X1_ADDRESS_A0_VCC	0x2F	// A0 is connected to VCC
#define MCP45X1_ADDRESS_A0_GND	0x2E	// A0 is connected to GND

// Commands
#define MCP45X1_CMD_WRITE	0x00
#define MCP45X1_CMD_INC		0x04
#define MCP45X1_CMD_DEC		0x08
#define MCP45X1_CMD_READ	0x0C

// Control bit definitions (sent to TCON register)
/*    GCEN: General Call Enable bit
 *
 *		This bit specifies if I2C General Call commands are accepted
 *			1 = Enable Device to “Accept” the General Call Address (0000h)
 *			0 = The General Call Address is disabled
 */
#define MCP45X1_TCON_GCEN	0b100000000

/*	R1HW: Resistor 1 Hardware Configuration Control bit
 *
 * 		This bit forces Resistor 1 into the “shutdown” configuration of the Hardware pin
 * 			1 = Resistor 1 is NOT forced to the hardware pin “shutdown” configuration
 * 			0 = Resistor 1 is forced to the hardware pin “shutdown” configuration
 */
#define MCP45X1_TCON_R1HW	0b010000000

/*	R1A: Resistor 1 Terminal A (P1A pin) Connect Control bit
 *
 * 		This bit connects/disconnects the Resistor 1 Terminal A to the Resistor 1 Network
 * 			1 = P1A pin is connected to the Resistor 1 Network
 * 			0 = P1A pin is disconnected from the Resistor 1 Network
 */
#define MCP45X1_TCON_R1A	0b001000000

/*	R1W: Resistor 1 Wiper (P1W pin) Connect Control bit
 *
 * 		This bit connects/disconnects the Resistor 1 Wiper to the Resistor 1 Network
 * 			1 = P1W pin is connected to the Resistor 1 Network
 * 			0 = P1W pin is disconnected from the Resistor 1 Network
 */
#define MCP45X1_TCON_R1W	0b000100000

/*	R1B: Resistor 1 Terminal B (P1B pin) Connect Control bit
 *
 * 		This bit connects/disconnects the Resistor 1 Terminal B to the Resistor 1 Network
 * 			1 = P1B pin is connected to the Resistor 1 Network
 * 			0 = P1B pin is disconnected from the Resistor 1 Network
 */
#define MCP45X1_TCON_R1B	0b000010000

/*	R0HW: Resistor 0 Hardware Configuration Control bit
 *
 * 		This bit forces Resistor 0 into the “shutdown” configuration of the Hardware pin
 * 			1 = Resistor 0 is NOT forced to the hardware pin “shutdown” configuration
 * 			0 = Resistor 0 is forced to the hardware pin “shutdown” configuration
 */
#define MCP45X1_TCON_R0HW	0b000001000

/*	R0A: Resistor 0 Terminal A (P0A pin) Connect Control bit
 *
 * 		This bit connects/disconnects the Resistor 0 Terminal A to the Resistor 0 Network
 * 			1 = P0A pin is connected to the Resistor 0 Network
 * 			0 = P0A pin is disconnected from the Resistor 0 Network
 */
#define MCP45X1_TCON_R0A	0b000000100

/*	R0W: Resistor 0 Wiper (P0W pin) Connect Control bit
 *
 * 		This bit connects/disconnects the Resistor 0 Wiper to the Resistor 0 Network
 * 			1 = P0W pin is connected to the Resistor 0 Network
 * 			0 = P0W pin is disconnected from the Resistor 0 Network
 */
#define MCP45X1_TCON_R0W	0b000000010

/*	R0B: Resistor 0 Terminal B (P0B pin) Connect Control bit
 *
 * 		This bit connects/disconnects the Resistor 0 Terminal B to the Resistor 0 Network
 * 			1 = P0B pin is connected to the Resistor 0 Network
 * 			0 = P0B pin is disconnected from the Resistor 0 Network
 */
#define MCP45X1_TCON_R0B	0b000000001


// Register addresses
#define MCP45X1_WIPER_0		0x00
#define MCP45X1_WIPER_1		0x01
#define MCP45X1_TCON		0x04


// Common WIPER values 7b
#define MCP45X1_WIPER_WA	0x080 	//Full-Scale (W = A), Increment commands ignored
#define MCP45X1_WIPER_MID	0x040	// W = N (Mid-Scale)
#define MCP45X1_WIPER_WB	0x000	//Zero Scale (W = B) Decrement command ignored

typedef enum _MCP45X1_Device
{
// 129 steps
	MCP4531 = 0,
	MCP4631,
// 257 steps
	MCP4551,
	MCP4651
} MCP45X1_Device;

typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint16_t addr;
	u16 steps;
	MCP45X1_Device device;

} MCP45X1_HandleTypeDef;





bool MCP45X1_init(I2C_HandleTypeDef *hi2c, MCP45X1_HandleTypeDef *hdfm, u16 addres, MCP45X1_Device device);
int16_t MCP45X1_getWiper(MCP45X1_HandleTypeDef *hdfm);
bool MCP45X1_setWiper(MCP45X1_HandleTypeDef *hdfm, uint16_t value) ;
bool MCP45X1_Up(MCP45X1_HandleTypeDef *hdfm);
bool MCP45X1_Dn(MCP45X1_HandleTypeDef *hdfm);
bool MCP45X1_setPercent(MCP45X1_HandleTypeDef *hdfm , float percent);
float MCP45X1_getPercent(MCP45X1_HandleTypeDef *hdfm );

#endif /* MCP45X1_MCP45X1_H_ */
