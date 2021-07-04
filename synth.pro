QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += static
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    oldcode/PolySynth.cpp \
    oldcode/PolySynthNote.cpp \
    oldcode/WaveTableOscillator.cpp

HEADERS += \
    mainwindow.h \
    oldcode/Octaves.h \
    oldcode/PolySynth.h \
    oldcode/PolySynthNote.h \
    oldcode/ScopedPaHandler.h \
    oldcode/WaveTableOscillator.h \
    oldcode/WindowsKeyDefs.h \
    oldcode/common.h \
    oldcode/sine.h \
    oldcode/timer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/portaudio/lib/x64/release/ -lportaudio_x64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/portaudio/lib/x64/release/ -lportaudio_x64
else:unix: LIBS += -L$$PWD/portaudio/lib/x64/ -lportaudio_x64

INCLUDEPATH += $$PWD/portaudio/include
DEPENDPATH += $$PWD

DISTFILES += \
    portaudio_x64.dll
