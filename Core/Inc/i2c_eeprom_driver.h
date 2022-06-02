#ifndef __I2C_EEPROM_DRIVER_H
#define __I2C_EEPROM_DRIVER_H

#include "main.h"

#include <stdio.h>
#include <string.h>

HAL_StatusTypeDef I2C_EEPROM_Init(I2C_HandleTypeDef *hi2c, uint16_t DevAddress);
HAL_StatusTypeDef I2C_EEPROM_UartSend(UART_HandleTypeDef *huart, char *message);
HAL_StatusTypeDef I2C_EEPROM_UartReceive(UART_HandleTypeDef *huart, char *buffer, int bufferLenght);
HAL_StatusTypeDef I2C_EEPROM_CommandParse(UART_HandleTypeDef *huart, argStruct *args, char *buffer, char *dataString);

void I2C_EEPROM_AddressRead(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress);
void I2C_EPPROM_PageRead(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t pageNumber);
void I2C_EEPROM_AddressWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *targetData, uint16_t dataSize);
void I2C_EEPROM_PageWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t pageNumber, uint8_t *targetData, uint16_t dataSize);

uint8_t I2C_EEPROM_StringToHex(char *string, uint8_t *hex);

#endif /* __I2C_EEPROM_DRIVER_H */
