#ifndef I2C_H
#define I2C_H

#define I2C_ACK  0
#define I2C_NACK 1

void I2cInit(void);
void I2cStart(void);
void I2cStop(void);
uint8_t I2cWrite(uint8_t data);
uint8_t I2cRead(void);

#endif

