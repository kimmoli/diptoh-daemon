/*
Copyright (c) 2014 kimmoli kimmo.lindholm@gmail.com @likimmo

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef DIPS_H
#define DIPS_H
#include <QObject>
#include "mcp23009driver.h"
#include <QStringList>
#include <QSocketNotifier>
#include <QThread>
#include "worker.h"

extern "C"
{
    #include "iphbd/libiphb.h"
}

class Dips : public QObject
{
    Q_OBJECT

public:
    explicit Dips(QObject *parent = 0);
    ~Dips();


private slots:
    void gpioChangedState();

    void heartbeatReceived(int sock);
    void iphbStop();
    void iphbStart();

private:
    void vddStateSet(bool state);
    void gpioExport();
    void gpioRelease();
    void gpioDirection(bool output);
    void gpioInterruptEdge();

    int gpioFd;
    QSocketNotifier* gpioNotifier;

    mcp23009Driver* mcp;

    QThread *thread;
    Worker *worker;

    iphb_t iphbdHandler;
    int iphb_fd;
    QSocketNotifier *iphbNotifier;
    bool iphbRunning;

    char prevDips;
};


#endif // DIPS_H

