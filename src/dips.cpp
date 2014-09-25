/*
Copyright (c) 2014 kimmoli kimmo.lindholm@gmail.com @likimmo

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "dips.h"
#include "worker.h"
#include "mcp23009driver.h"
#include <QCoreApplication>
#include <QThread>
#include <QStringList>
#include <QSocketNotifier>
#include <QProcess>

extern "C"
{
    #include "iphbd/libiphb.h"
}

#define GPIO_INT "67"
#define GPIO_INT_EDGE "falling"

Dips::Dips(QObject *parent) :
    QObject(parent)
{
    vddStateSet(true);

    mcp = new mcp23009Driver(MCP23009_ADDRESS);

    gpioExport();
    gpioDirection(false);
    gpioInterruptEdge();

    thread = new QThread();
    worker = new Worker();

    worker->moveToThread(thread);
    connect(worker, SIGNAL(gpioInterruptCaptured()), this, SLOT(gpioChangedState()));
    connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);

    prevDips = mcp->readInputState();

    gpioFd = open("/sys/class/gpio/gpio" GPIO_INT "/value", O_RDONLY | O_NONBLOCK);

    if (gpioFd > -1)
    {
        worker->abort();
        thread->wait(); // If the thread is not running, this will immediately return.

        worker->requestWork(gpioFd);

        printf("Worker started\n");
    }

    iphbRunning = false;
    iphbdHandler = iphb_open(0);

    if (!iphbdHandler)
        printf("Error opening iphb\n");

    iphb_fd = iphb_get_fd(iphbdHandler);

    iphbNotifier = new QSocketNotifier(iphb_fd, QSocketNotifier::Read);

    if (!QObject::connect(iphbNotifier, SIGNAL(activated(int)), this, SLOT(heartbeatReceived(int))))
    {
        delete iphbNotifier, iphbNotifier = 0;
        printf("failed to connect iphbNotifier\n");
    }
    else
    {
        iphbNotifier->setEnabled(false);
    }

    if (iphbNotifier)
        printf("iphb initialized succesfully\n");

    heartbeatReceived(0);
}


Dips::~Dips()
{
    delete mcp;

    gpioRelease();

    vddStateSet(false);
}


/*
 * Function to control TOH Vdd state
 */
void Dips::vddStateSet(bool state)
{
    printf("vdd set to %s\n", state ? "on" : "off");

    int fd = open("/sys/devices/platform/reg-userspace-consumer.0/state", O_WRONLY);

    if (!(fd < 0))
    {
        if (write (fd, state ? "1" : "0", 1) != 1)
            printf("Failed to control VDD\n");

        close(fd);
        return;
    }

    QThread::msleep(350);
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
            printf("Failed to export GPIO\n");

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
            printf("Failed to unexport GPIO\n");

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
            printf("Failed to change direction of GPIO to %s\n", (output ? "output" : "input") );

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
            printf("Failed to set GPIO edge to %s\n", GPIO_INT_EDGE);

        close(fd);
        return;
    }
}


void Dips::gpioChangedState()
{
    QProcess process[7];

    printf("Dips toggled: ");

    char data = mcp->readInputState();

    int i;
    for (i=5 ; i>=0; i--)
    {
        printf((data & 1<<i) ? "1" : "0");
    }
    printf(" button %s\n", (data & 1<<7) ? "down" : "up");

    if (data == prevDips)
    {
        printf("Nothing actually changed\n");
    }
    else
    {
        for (i=5 ; i>=0; i--)
        {
            if ((data & 1<<i) != (prevDips & 1<<i))
            {
                printf("dip %d changed to %s\n", i+1, (data & 1<<i) ? "on" : "off");
                process[i].startDetached("/bin/sh", QStringList()<< QString("/home/nemo/diptoh/dip%1%2.sh").arg(i+1).arg((data & 1<<i) ? "on" : "off"));
            }
        }

        /* Check if button was pressed or released */
        if ((data & 1<<7) != (prevDips & 1<<7))
        {

            if ((data & 1<<7) == (1<<7))
            {
                printf("Button pressed\n");
                process[6].startDetached("/bin/sh", QStringList()<< QString("/home/nemo/diptoh/buttondown.sh"));
            }
            else
            {
                printf("Button released\n");
                process[6].startDetached("/bin/sh", QStringList()<< QString("/home/nemo/diptoh/buttonup.sh"));
            }
        }
    }

    prevDips = data;
}

/* iphb wakeup stuff */

void Dips::heartbeatReceived(int sock)
{
    Q_UNUSED(sock);

    iphbStop();

    if (!mcp->isAlive())
    {
        printf("Seems that GPIO chip has been disconnect. Reinitializing...\n");
        mcp->init();
    }

    iphbStart();
}

void Dips::iphbStart()
{
    if (iphbRunning)
        return;

    if (!(iphbdHandler && iphbNotifier))
    {
        printf("iphbStart iphbHandler not ok\n");
        return;
    }

    time_t unixTime = iphb_wait(iphbdHandler, 25, 35 , 0);

    if (unixTime == (time_t)-1)
    {
        printf("iphbStart timer failed\n");
        return;
    }

    iphbNotifier->setEnabled(true);
    iphbRunning = true;

}

void Dips::iphbStop()
{
    if (!iphbRunning)
        return;

    if (!(iphbdHandler && iphbNotifier))
    {
        printf("iphbStop iphbHandler not ok\n");
        return;
    }

    iphbNotifier->setEnabled(false);

    (void)iphb_discard_wakeups(iphbdHandler);

    iphbRunning = false;

}
