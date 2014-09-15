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

    /* Inveert DIP switch inputs */
    writeToRegister(IPOL, 0x3F);

    /* Interrupt on change of dip-switches */
    writeToRegister(GPINTEN, 0x3F);

    /* Default comparison value */
    writeToRegister(DEFVAL, 0x00);

    /* Interrupt on change control register - compare to previous value */
    /* Except pushbutton, compare to 0 */
    writeToRegister(INTCON, 0x80);

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





