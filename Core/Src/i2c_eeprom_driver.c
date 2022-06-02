#include "i2c_eeprom_driver.h"

HAL_StatusTypeDef I2C_EEPROM_Init(I2C_HandleTypeDef *hi2c, uint16_t DevAddress)
{
	  while (1)
	  {
		  if (HAL_I2C_IsDeviceReady(hi2c, (uint16_t)DevAddress, 1, -1) == HAL_OK)
			  break;
	  }

	  return HAL_OK;
}

HAL_StatusTypeDef I2C_EEPROM_UartSend(UART_HandleTypeDef *huart, char *message)
{
	if (HAL_UART_Transmit(huart, (uint8_t *)message, strlen(message), 10) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

HAL_StatusTypeDef I2C_EEPROM_UartReceive(UART_HandleTypeDef *huart, char *buffer, int bufferLenght)
{
	char ch;
	for (int i = 0; i < bufferLenght - 1; i++)
	{
	  if (HAL_UART_Receive(huart, (uint8_t *)&ch, 1, HAL_MAX_DELAY) != HAL_OK)
	  {
		  return HAL_ERROR;
	  }
	  if (ch == '\n')
	  {
		  break;
	  }
	  else
	  {
		  buffer[i] = ch;
	  }
	}
	return HAL_OK;
}

HAL_StatusTypeDef I2C_EEPROM_CommandParse(UART_HandleTypeDef *huart, argStruct *args, char *buffer, char *dataString)
{
	  char *space1 = strchr(buffer, ' ');
	  if (space1 == NULL)
	  {
		  return HAL_ERROR;
	  }
	  char chipSelect[5] = { 0 };
	  strncpy(chipSelect, buffer, space1 - buffer);
	  if (strncmp(chipSelect, "mem1", 4) == 0)
	  {
		  args->eepromAddr = (0x50) << 1;
	  }
	  else if (strncmp(chipSelect, "mem2", 4) == 0)
	  {
		  args->eepromAddr = (0x51) << 1;
	  }
	  else
	  {
		  return HAL_ERROR;
	  }


	  char *space2;
	  space2 = strchr(space1 + 1, ' ');
	  if (space2 == NULL)
	  {
		  return HAL_ERROR;
	  }
	  char mode = *(space1 + 1);
	  if (*(space1 + 2) != ' ')
	  {
		  return HAL_ERROR;
	  }
	  if (mode == 'w')
	  {
		  args->mode = mode;
	  }
	  else if (mode == 'r')
	  {
		  args->mode = mode;
	  }
	  else
	  {
		  return HAL_ERROR;
	  }


	  char *space3;
	  char addr[7] = { 0 };
	  space3 = strchr(space2 + 1, ' ');
	  if ((space3 == NULL) && (mode == 'r'))
	  {
		  strcpy(addr, (space2 + 1));
//		  HAL_UART_Transmit(huart, (uint8_t *)"Blabla Read", 12, 10);
	  }
	  else if ((space3 != NULL) && (mode == 'w'))
	  {
		  strncpy(addr, space2 + 1, space3 - space2 - 1);
//		  HAL_UART_Transmit(huart, (uint8_t *)"Blabla Write", 13, 10);
	  }
	  else
	  {
		  return HAL_ERROR;
	  }

	  if (strncmp(addr, "0x", 2) == 0)
	  {
		  args->addrFlag = 0;
		  int addrValue = (int)strtol(addr, NULL, 0);
		  if (addrValue > 0x7fff)
		  {
			  return HAL_ERROR;
		  }
		  args->addr = (uint16_t)addrValue;

		  int page;
		  for (page = 1; page < 512; ++page)
		  {
			  if ((args->addr > (64 * (page - 1))) && (args->addr < (64 * page - 1)))
			  {
				  break;
			  }
		  }
		  args->page = page;

	  }
	  else
	  {
		  args->addrFlag = 1;
		  int page = atoi(addr);
		  if (page < 0 || page > 512)
		  {
			  return HAL_ERROR;
		  }
		  else
		  {
			  args->page = (uint16_t)page;
		  }
	  }


	  if (mode == 'w')
	  {
		  strcpy(dataString, (space3 + 1));
		  args->data = dataString;

	  }
	  else
	  {
		  args->data = NULL;
	  }

	  return HAL_OK;
}

void I2C_EEPROM_AddressRead(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress)
{
	uint8_t data;
	HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, &data, 1, 10);
	HAL_Delay(1);

	char buffer[20];
	sprintf(buffer, "%04x:   %02x\n\r", MemAddress, data);

	HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), 10);
}

void I2C_EPPROM_PageRead(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t pageNumber)
{
	uint16_t memAddress = 64 * (pageNumber - 1);
	for (unsigned int base = (0 + (pageNumber - 1) * 64); base < (64 + (pageNumber - 1) * 64); base += 16)
	{
	  uint8_t data[16] = { 0 };
	  for (uint16_t offset = 0; offset <= 15; offset++)
	  {
		  HAL_I2C_Mem_Read(hi2c, DevAddress, (memAddress + base + offset), I2C_MEMADD_SIZE_16BIT,
						   &data[offset], 1, 10);
//		  HAL_Delay(10);
	  }

	  char buffer[80] = { 0 };
	  sprintf(buffer, "%04x:   %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x\n\r",
			  base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
			  data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

	  HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), 10);
	}

	HAL_UART_Transmit(huart, (uint8_t *)"\n\r", 1, 10);
}

void I2C_EEPROM_AddressWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *targetData, uint16_t dataSize)
{
	HAL_Delay(100);
	HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, targetData, dataSize, 10);
	HAL_Delay(250);
}

void I2C_EEPROM_PageWrite(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t pageNumber, uint8_t *targetData, uint16_t dataSize)
{
	uint16_t MemAddress = 64 * (pageNumber - 1);
	HAL_Delay(100);
	HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, targetData, dataSize, 10);
	HAL_Delay(250);
}

uint8_t I2C_EEPROM_StringToHex(char *string, uint8_t *hex)
{
	unsigned int hexArray[140] = { 0 };

    int len = strlen(string);
    if ((len & 1) != 0) len++;

    uint8_t i, j;
    for (i = 0, j = 0; i < (len >> 1); i++, j+=2)
    {
        sscanf(string + j, "%02x", hexArray + i);
    }

    for (i = 0; i < (len >> 1); i++)
    {
    	hex[i] = (uint8_t)hexArray[i];
    }

    return len >> 1;
}
