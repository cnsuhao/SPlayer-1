#pragma once

#include <QObject>

// TODO: read QtSystemInfo.ScreenSaver

class ScreenSaver : QObject
{
    Q_OBJECT
public:
    static ScreenSaver& instance();
    ScreenSaver();
    ~ScreenSaver();
    // enable: just restore the previous settings. settings changed during the object life will ignored
    bool enable(bool yes);
public slots:
    void enable();
    void disable();
protected:
    virtual void timerEvent(QTimerEvent *);
private:
    //return false if already called
    bool retrieveState();
    bool restoreState();
    bool state_saved, modified;
#ifdef Q_OS_LINUX
    bool isX11;
    int timeout;
    int interval;
    int preferBlanking;
    int allowExposures;
#endif //Q_OS_LINUX
    int ssTimerId; //for mac
    quint32 osxIOPMAssertionId; // for mac OSX >= 10.8
};
