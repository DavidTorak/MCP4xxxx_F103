/*
 * MCP45x1.c
 *
 *  Created on: Mar 8, 2023
 *      Author: DavidTorak
 */

#include "MCP45x1.h"

#define I2C_READYNESS_DELAY		500

static HAL_StatusTypeDef _MCP45X1_Write(MCP45X1_HandleTypeDef *hdfm, u8 Register, u16 Value);

/* ************************************************************
 * 					Inicializace E-Pot. MCP4xxxxx
 * ************************************************************
 * @param 		I2C_HandleTypeDef *hi2c
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * @param 		addres adresa i2c
 * @param 		MCP45X1_Device 	konkrétní typ pot.
 * ************************************************************
 * @return		úspěšná komunikace s čipem
 *
 * ********************************************************** */
bool MCP45X1_init(I2C_HandleTypeDef *hi2c, MCP45X1_HandleTypeDef *hdfm, u16 addres, MCP45X1_Device device) {
	HAL_StatusTypeDef ret;
	hdfm->hi2c = hi2c;
	hdfm->addr = addres << 1;
	switch (device) {
		case MCP4531:
			hdfm->steps = 128;
			hdfm->device = MCP4531;
			break;
		case MCP4631:
			hdfm->steps = 128;
			hdfm->device = MCP4631;
			break;
		case MCP4551:
			hdfm->steps = 256;
			hdfm->device = MCP4551;
			break;
		case MCP4651:
			hdfm->steps = 256;
			hdfm->device = MCP4651;
			break;
		default:
			break;
	}
	// zjistí zada je komunikace funkční
	ret = HAL_I2C_IsDeviceReady(hdfm->hi2c, hdfm->addr, 3, 500);

	if (ret != HAL_OK){
		// bohužel není
		return false;
	}
	else if (ret == HAL_OK){
		// je funkční, nastavý se potenciometr na střed (50%)
		MCP45X1_setPercent(hdfm, 50);
		return true;
	}
	return false;
}

/* ************************************************************
 * 			Přečte se nastavení odporu (poměru) v počtu kroků
 * ************************************************************
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * ************************************************************
 * @return		počet kroků jaky je momentálně nastaven
 *
 * ********************************************************** */
int16_t MCP45X1_getWiper(MCP45X1_HandleTypeDef *hdfm) {
	HAL_StatusTypeDef rett;// záznam jak dopadla komunikace
	u8 data[3];// buffer pro načtení dat z E-Pot.
	int16_t ret;// pro vrácení počtu kroku
	rett = HAL_I2C_Mem_Read(hdfm->hi2c, hdfm->addr, MCP45X1_CMD_READ, 1, data, 2, I2C_READYNESS_DELAY);// přečte data
	ret = (data[0] << 8) & 0xFF00;//data se převedou ze dvou 8B na jeden 16B
	ret |= data[1];
	if (rett != HAL_OK){
		// komunikace neproběhla v pořádku
		return -1;
	}
	// vše ok, vrátí nastavenou hodnotu
	return ret;
}

/* ************************************************************
 * 			Nastavení odporu (poměru) v počtu kroků
 * ************************************************************
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * @param 		hodnota (počet kroků, který má být nastaven)
 * ************************************************************
 * @return		úspěšná komunikace s čipem
 *
 * ********************************************************** */
bool MCP45X1_setWiper(MCP45X1_HandleTypeDef *hdfm, uint16_t value) {
	HAL_StatusTypeDef rett;
	rett = _MCP45X1_Write(hdfm, MCP45X1_WIPER_0, value);
	if (rett != HAL_OK){
		return false;
	}
	return true;
}

/* ************************************************************
 * 		Nastavení odporu, inkrementace kroku ke st. hodnotě
 * ************************************************************
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * ************************************************************
 * @return		úspěšná komunikace s čipem
 *
 * ********************************************************** */
bool MCP45X1_Up(MCP45X1_HandleTypeDef *hdfm) {
	HAL_StatusTypeDef rett;
	rett = HAL_I2C_Mem_Write(hdfm->hi2c, hdfm->addr, MCP45X1_CMD_INC, 1, 0, 0, I2C_READYNESS_DELAY);
	if (rett != HAL_OK){
		return false;
	}
	return true;
}

/* ************************************************************
 * 		Nastavení odporu, dekrementace kroku ke st. hodnotě
 * ************************************************************
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * ************************************************************
 * @return		úspěšná komunikace s čipem
 *
 * ********************************************************** */
bool MCP45X1_Dn(MCP45X1_HandleTypeDef *hdfm) {
	HAL_StatusTypeDef rett;
	rett = HAL_I2C_Mem_Write(hdfm->hi2c, hdfm->addr, MCP45X1_CMD_DEC, 1, 0, 0, I2C_READYNESS_DELAY);
	if (rett != HAL_OK){
		return false;
	}
	return true;
}

/* ************************************************************
 * 		Nastavení odporu (poměru) v procentech
 * ************************************************************
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * @param 		procenta nastavení (může být float)
 * ************************************************************
 * @return		úspěšná komunikace s čipem
 *
 * ********************************************************** */
bool MCP45X1_setPercent(MCP45X1_HandleTypeDef *hdfm, float percent) {
	HAL_StatusTypeDef rett;
	float tmp = ((float) hdfm->steps / 100.0) * percent;
	u8 Wiper = (u8) tmp;
	rett = _MCP45X1_Write(hdfm, MCP45X1_WIPER_0, Wiper);
	if (rett != HAL_OK){
		return false;
	}
	return true;
}

/* ************************************************************
 * 		Vrátí jaké je nastavení odporu (poměru) v procentech
 * ************************************************************
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * ************************************************************
 * @return		procenta ve float
 *
 * ********************************************************** */
float MCP45X1_getPercent(MCP45X1_HandleTypeDef *hdfm) {
	u16 tmp = MCP45X1_getWiper(hdfm);
	return ((float) tmp / (float) hdfm->steps * 100.0);
}

/* ************************************************************
 * 		Zápis do registru E-Pot
 * ************************************************************
 * @param 		MCP45X1_HandleTypeDef (&MCP45hdev)
 * @param 		do jakého registru se bude zapisovat
 * @param 		jaká hodnota se bude zapisovat
 *
 * ************************************************************
 * @return		úspěšná komunikace s čipem
 *
 *
 *
 * Děkuji za tuto část kódu https://github.com/pkrakow
 * Thank you for this part of the code https://github.com/pkrakow
 * https://github.com/pkrakow/MCP466_DigitalPot
 * ********************************************************** */
static HAL_StatusTypeDef _MCP45X1_Write(MCP45X1_HandleTypeDef *hdfm, u8 Register, u16 Value) {
	HAL_StatusTypeDef rett;
	u8 firstCommandByte = 0x00;  //Připraví první Command Byte pro zápisy
	u8 secondCommandByte = 0x00;  // Prázdný datový bajt
	u16 tempWord = Value;
	u8 tempByte;

	// Připravte příkaz  k zápisu do digitálního potenciometru
	Register *= 16;  // Posune hodnotu registru doleva o čtyři bity
	firstCommandByte |= Register;  // Načtěte adresu registru do prvního Commandu
	tempWord &= 0x0100;  // Vymažte MSB 7 bitů a spodní bajt vstupní hodnoty pro vyzvednutí dvou dat bitů
	tempWord /= 256;  // Posune horní bajt vstupní hodnoty doprava o jeden bajt
	tempByte = (u8) (tempWord);  // Uloží horní bajt vstupní hodnoty do proměnné velikosti 1 bajtu
	firstCommandByte |= tempByte;  // Načtěte dva nejvyšší vstupní údaje do prvního Command bajtu
	tempWord = Value;  // Načtěte vstupní hodnotu do tempwordu
	tempWord &= 0x00FF;  // Vyčistí horní bajt
	secondCommandByte = (u8) (tempWord);  // Uloží spodní bajt vstupní hodnoty do druhého příkazu bajtu

	// Napište do digitálního potenciometru

	rett = HAL_I2C_Mem_Write(hdfm->hi2c, hdfm->addr, firstCommandByte, 1, &secondCommandByte, 1, I2C_READYNESS_DELAY);

	return rett;
}
