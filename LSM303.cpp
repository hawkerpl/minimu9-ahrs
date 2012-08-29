#include "LSM303.h"

#define MAG_ADDRESS            (0x3C >> 1)
#define ACC_ADDRESS_SA0_A_LOW  (0x30 >> 1)
#define ACC_ADDRESS_SA0_A_HIGH (0x32 >> 1)

LSM303::LSM303(const char * i2cDeviceName) :
  i2c_mag(i2cDeviceName), i2c_acc(i2cDeviceName)
{
    i2c_mag.addressSet(MAG_ADDRESS);
    i2c_acc.addressSet(ACC_ADDRESS_SA0_A_HIGH);
}

uint8_t LSM303::readMagReg(int8_t reg)
{
    return i2c_mag.readByte(reg);
}

void LSM303::writeMagReg(uint8_t reg, uint8_t value)
{
    i2c_mag.writeByte(reg, value);
}

void LSM303::writeAccReg(uint8_t reg, uint8_t value)
{
    i2c_acc.writeByte(reg, value);
}

// Turns on the LSM303's accelerometer and magnetometers and places them in normal
// mode.
void LSM303::enableDefault(void)
{
    // Enable Accelerometer
    // Normal power mode, all axes enabled, 50 Hz
    writeAccReg(LSM303_CTRL_REG1_A, 0b01000111);

    // Enable Magnetometer
    // Continuous conversion mode
    writeMagReg(LSM303_MR_REG_M, 0x00);
}

void LSM303::readAcc(void)
{
    uint8_t block[6];
    i2c_acc.readBlock(0x80 | LSM303_OUT_X_L_A, sizeof(block), block);

    a[0] = (int16_t)(block[0] | block[1] << 8) >> 4;
    a[1] = (int16_t)(block[2] | block[3] << 8) >> 4;
    a[2] = (int16_t)(block[4] | block[5] << 8) >> 4;
}

void LSM303::readMag(void)
{
    uint8_t block[6];
    i2c_mag.readBlock(0x80 | LSM303_OUT_X_H_M, sizeof(block), block);

    // DLM, DLHC: register address order is X,Z,Y with high bytes first
    m[0] = (int16_t)(block[1] | block[0] << 8);
    m[1] = (int16_t)(block[5] | block[4] << 8);
    m[2] = (int16_t)(block[3] | block[2] << 8);
}

// Reads all 6 channels of the LSM303 and stores them in the object variables
void LSM303::read(void)
{
    readAcc();
    readMag();
}
