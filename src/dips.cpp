/*
Copyright (c) 2014 kimmoli kimmo.lindholm@gmail.com @likimmo

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "dips.h"
#include "mcp23009driver.h"
//#include <QSettings>
#include <QCoreApplication>
#include <QThread>
#include <QStringList>
#include <QSocketNotifier>


#define GPIO_INT "67"
#define GPIO_INT_EDGE "falling"

Dips::Dips(QObject *parent) :
    QObject(parent)
{
    emit versionChanged();

    vddStateSet(true);

    mcp = new mcp23009Driver(MCP23009_ADDRESS);

    gpioExport();
    gpioDirection(false);
    gpioInterruptEdge();

    gpioFd = open("/sys/class/gpio/gpio" GPIO_INT "/value", O_RDONLY | O_NONBLOCK);
    gpioNotifier = new QSocketNotifier(gpioFd, QSocketNotifier::Exception);

    connect(gpioNotifier, SIGNAL(activated(int)), this, SLOT(gpioChangedState(int)));

}

QString Dips::readVersion()
{
    return APPVERSION;
}


Dips::~Dips()
{

    delete mcp;

    gpioRelease();

    vddStateSet(false);
}


QStringList Dips::readDips()
{
    QStringList list;

    char data = mcp->readInputState();

    int i;
    for (i=5 ; i>=0; i--)
    {
        list.append((data & 1<<i) ? "1" : "0");
    }

    return list;
}

/*
 * Function to control TOH Vdd state
 */
void Dips::vddStateSet(bool state)
{
    int fd = open("/sys/devices/platform/reg-userspace-consumer.0/state", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, state ? "1" : "0", 1) != 1)
            qCritical() << "Failed to control VDD.";

        close(fd);
        return;
    }

    QThread::msleep(100);
}

/*
 * Function to control state of GPIO pin
 */

void Dips::gpioExport()
{
    int fd = open("/sys/class/gpio/export", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, GPIO_INT, strlen(GPIO_INT)) != strlen(GPIO_INT))
            qCritical() << "Failed to export GPIO";

        close(fd);
        return;
    }
}

void Dips::gpioRelease()
{
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, GPIO_INT, strlen(GPIO_INT)) != strlen(GPIO_INT))
            qCritical() << "Failed to unexport GPIO";

        close(fd);
        return;
    }

}

void Dips::gpioDirection(bool output)
{

    int fd = open("/sys/class/gpio/gpio" GPIO_INT "/direction", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, output ? "out" : "in", (output ? 3 : 2)) != (output ? 3 : 2))
            qCritical() << "Failed to change direction of GPIO to " << (output ? "output" : "input");

        close(fd);
        return;
    }
}

void Dips::gpioInterruptEdge()
{
    int fd = open("/sys/class/gpio/gpio" GPIO_INT "/edge", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, GPIO_INT_EDGE, strlen(GPIO_INT_EDGE)) != strlen(GPIO_INT_EDGE))
            qCritical() << "Failed to set GPIO edge to " GPIO_INT_EDGE;

        close(fd);
        return;
    }
}


void Dips::gpioChangedState(int fd)
{
    char buf[20];

    int n = read(fd, buf, sizeof(buf));
    Q_UNUSED(n);

    qDebug() << "Dips toggled";

    emit dipsToggled();
}

