#ifndef TEST_FRAMEWORK_I2C_DEVICE
#define TEST_FRAMEWORK_I2C_DEVICE

#include <linux/i2c.h>

#define MAX_CHIPS 8
/*
#define STUB_FUNC (I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE | \
	   I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA | \
	   I2C_FUNC_SMBUS_I2C_BLOCK)
*/
#define STUB_FUNC (I2C_FUNC_I2C                         | \
                   I2C_FUNC_10BIT_ADDR                  | \
                   I2C_FUNC_PROTOCOL_MANGLING           | \
                   I2C_FUNC_SMBUS_PEC                   | \
                   I2C_FUNC_SMBUS_BLOCK_PROC_CALL       | \
                   I2C_FUNC_SMBUS_QUICK                 | \
                   I2C_FUNC_SMBUS_READ_BYTE             | \
                   I2C_FUNC_SMBUS_WRITE_BYTE            | \
                   I2C_FUNC_SMBUS_READ_BYTE_DATA        | \
                   I2C_FUNC_SMBUS_WRITE_BYTE_DATA       | \
                   I2C_FUNC_SMBUS_READ_WORD_DATA        | \
                   I2C_FUNC_SMBUS_WRITE_WORD_DATA       | \
                   I2C_FUNC_SMBUS_PROC_CALL             | \
                   I2C_FUNC_SMBUS_READ_BLOCK_DATA       | \
                   I2C_FUNC_SMBUS_WRITE_BLOCK_DATA      | \
                   I2C_FUNC_SMBUS_READ_I2C_BLOCK        | \
                   I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)

int initialize_i2c_device (void);
void free_i2c_device (void);

#endif
