
#ifndef I2C_H
#define I2C_H

#include "frdm_bsp.h"

#define I2C_ERR_TIMEOUT 0x01
#define I2C_ERR_NOACK 0x02

/**
 * @brief I2C initialization.
 */
void I2C_Init(void);
/**
 * @brief I2C write operation.
 *
 * @param Address.
 * @param Data.
 */
uint8_t I2C_Write(uint8_t address, uint8_t data);
	
#endif /* I2C_H */
