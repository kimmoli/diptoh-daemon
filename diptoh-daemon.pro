TARGET = harbour-diptoh-daemon

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
QT -= gui

LIBS += -lrt

target.path = /usr/bin/

systemd.path = /etc/systemd/system/
systemd.files = config/$${TARGET}.service

udevrule.path = /etc/udev/rules.d/
udevrule.files = config/95-$${TARGET}.rules

ambience.path = /usr/share/ambience/$${TARGET}
ambience.files = ambience/$${TARGET}.ambience

images.path = $${ambience.path}/images
images.files = ambience/images/*

scripts.path = /home/nemo/diptoh
scripts.files = config/dip*.sh config/button*.sh

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

message($${DEFINES})

INSTALLS += target systemd udevrule ambience images scripts

INCLUDEPATH += \
    ./3rdparty/libdsme/include/ \
    ./3rdparty/mce-dev/include/ \
    ./3rdparty/libiphb/src/

SOURCES += \
    src/diptoh.cpp \
    src/dips.cpp \
    src/mcp23009driver.cpp \
    src/driverBase.cpp \
    src/worker.cpp \
    3rdparty/libiphb/src/libiphb.c
	
HEADERS += \
    src/dips.h \
    src/mcp23009driver.h \
    src/driverBase.h \
    src/worker.h \
    3rdparty/libiphb/src/libiphb.h \
    3rdparty/libiphb/src/iphb_internal.h \
    3rdparty/libdsme/include/dsme/messages.h \
    3rdparty/mce-dev/include/mce/dbus-names.h


OTHER_FILES += \
    rpm/diptoh-daemon.spec \
    config/$${TARGET}.conf \
    config/$${TARGET}.service \
    config/95-$${TARGET}.rules \
    ambience/$${TARGET}.ambience \
    ambience/images/diptoh.jpg \
    config/dip6on.sh \
    config/dip6off.sh \
    config/dip5on.sh \
    config/dip5off.sh \
    config/dip4on.sh \
    config/dip4off.sh \
    config/dip3on.sh \
    config/dip3off.sh \
    config/dip2on.sh \
    config/dip2off.sh \
    config/dip1on.sh \
    config/dip1off.sh \
    config/buttondown.sh \
    config/buttonup.sh

