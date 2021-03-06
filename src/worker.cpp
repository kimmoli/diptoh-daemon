#include "worker.h"
#include <QTimer>
#include <QEventLoop>
#include <poll.h>

#include <QThread>


Worker::Worker(QObject *parent) :
    QObject(parent)
{
    _working =false;
    _abort = false;
}

void Worker::requestWork(int gpio_fd)
{
    mutex.lock();
    _working = true;
    _gpio_fd = gpio_fd;
    _abort = false;

    mutex.unlock();

    emit workRequested();
}

void Worker::abort()
{
    mutex.lock();
    if (_working)
    {
        _abort = true;
    }
    mutex.unlock();
}

void Worker::doWork()
{
    struct pollfd fdset[1];
    int nfds = 1;

    int timeout;
    int dummy = 0;
    char *buf[20];

    timeout = 5000;

    for (;;)
    {

        // Checks if the process should be aborted
        mutex.lock();
        bool abort = _abort;
        mutex.unlock();

        if (abort)
        {
            break;
        }

        memset((void*)fdset, 0, sizeof(fdset));

        fdset[0].fd = _gpio_fd;
        fdset[0].events = POLLPRI;

        poll(fdset, nfds, timeout);

        if (fdset[0].revents & POLLPRI)
        {
            dummy += read(fdset[0].fd, buf, 20);
            emit gpioInterruptCaptured();
        }
    }

    mutex.lock();
    _working = false;
    mutex.unlock();

    emit finished();
}
