#ifndef MCP23009DRIVER_H
#define MCP23009DRIVER_H
#include "driverBase.h"

/*
 * I2C address of MCP23009 on diptoh
 */
#define MCP23009_ADDRESS 0x20

/*
 * Registers
 */
#define IODIR   0x00
#define IPOL    0x01
#define GPINTEN 0x02
#define DEFVAL  0x03
#define INTCON  0x04
#define IOCON   0x05
#define GPPU    0x06
#define INTF    0x07
#define INTCAP  0x08
#define GPIO    0x09
#define OLAT    0x0A


class mcp23009Driver : public DriverBase
{
    Q_OBJECT
public:
    explicit mcp23009Driver(unsigned char address);
    ~mcp23009Driver();

    void init();
    char readInputState();
    void writeToRegister(char regAddr, char data);
    int isAlive();

private:
    unsigned char mcp23009Address;


};

#endif // MCP23009DRIVER_H
