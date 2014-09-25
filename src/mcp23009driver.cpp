#include "mcp23009driver.h"
#include <QThread>

mcp23009Driver::mcp23009Driver(unsigned char address)
{
    mcp23009Address = address;
    init();

    printf("mcp23009 initialized\n");
}

mcp23009Driver::~mcp23009Driver()
{

}

void mcp23009Driver::init()
{
    /* Set all pins as inputs */
    writeToRegister(IODIR, 0xFF);

    /* Invert DIP switch inputs */
    writeToRegister(IPOL, 0x3F);

    /* Interrupt on change of dip-switches and push-button */
    writeToRegister(GPINTEN, 0xBF);

    /* Default comparison value */
    writeToRegister(DEFVAL, 0x00);

    /* Interrupt on change control register - compare to previous value */
    writeToRegister(INTCON, 0x00);

    /* Configuration register - opendrain interrupt, reading GPIO register clears interrupt */
    writeToRegister(IOCON, 0x04);

    /* Pullups, all except pushbutton */
    writeToRegister(GPPU, 0x7F);

    /* Outputs, clear, if any */
    writeToRegister(OLAT, 0x00);

}


char mcp23009Driver::readInputState()
{
    QByteArray data;
    data = writeThenRead(mcp23009Address, GPIO, 1);

    return data.at(0);
}


void mcp23009Driver::writeToRegister(char regAddr, char data)
{
    char buf[2];

    /* Set all pins as inputs */
    buf[0] = regAddr;
    buf[1] = data;

    if (!writeBytes(mcp23009Address, buf, 2))
        qDebug() << "Failed to write";

}

/*
 * Check that GPINTEN has value programmed at init()
 */
int mcp23009Driver::isAlive()
{
    QByteArray data;
    data = writeThenRead(mcp23009Address, GPINTEN, 1);

    return (data.at(0) == 0xBF);
}




