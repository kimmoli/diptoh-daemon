/*
Copyright (c) 2014 kimmoli kimmo.lindholm@gmail.com @likimmo

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <QScopedPointer>
#include <QCoreApplication>
#include "dips.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

void daemonize();
void signalHandler(int sig);

Dips* dips;

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);
    daemonize();

    setlinebuf(stdout);
    setlinebuf(stderr);

    printf("diptoh starting, yay. My version is %s\n", APPVERSION);

    dips = new Dips();

    return app.exec();
}


void daemonize()
{
    /* Change the file mode mask */
    umask(0);

    /* Change the current working directory */
    if ((chdir("/tmp")) < 0)
        exit(EXIT_FAILURE);

    /* register signals to monitor / ignore */
    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,signalHandler); /* catch hangup signal */
    signal(SIGTERM,signalHandler); /* catch kill signal */
}


void signalHandler(int sig) /* signal handler function */
{
    switch(sig)
    {
        case SIGHUP:
            /* rehash the server */
            printf("Received signal SIGHUP\n");
            break;
        case SIGTERM:
            /* finalize the server */
            printf("Received signal SIGTERM\n");
            delete(dips);
            printf("diptoh exiting, bye\n");
            exit(0);
            break;
    }
}


