#include <iostream>
#include "sensors/joystickMB/JoystickMB.h"
#include "actuators/motors/OSMC_driver/OSMC_driver.hpp"
#include <sys/time.h>
#include <boost/thread.hpp>

using namespace std;

class TankDriver
{
    private:
    JoystickMB *_joy;
    OSMC_driver *_driver;
    LISTENER(TankDriver, onNewJoystick, JoystickState);
    volatile int _LPWM, _RPWM, _LDIR, _RDIR;

    public:
    TankDriver(JoystickMB *joystick, OSMC_driver *driver)
        : LonNewJoystick(this)
    {
        _joy = joystick;
        _joy->onNewData += &LonNewJoystick;
        _driver = driver;
        _driver->stopMotors();
        _LDIR = 0;
        _LPWM = 0;
        _RDIR = 0;
        _RPWM = 0;
    }

    void onNewJoystick(JoystickState state)
    {
        int LIn = state.axes[1];
        int RIn = state.axes[3];

        double maxVal = 32767.0;

        _LPWM = abs( ( LIn / maxVal ) * 150.0 );
        _RPWM= abs( ( RIn / maxVal ) * 150.0 );

        int DEADBAND = 30;
        if(_LPWM < DEADBAND)
        {
            _LPWM = 0;
        }
        if(_RPWM < DEADBAND)
        {
            _RPWM = 0;
        }

        _LDIR = !signbit(LIn);
        _RDIR = !signbit(RIn);

        cout << "Right : " << (_LDIR &&_LPWM? "-" : " ") << _LPWM << "    " << (_RDIR &&_RPWM? "-" : " ") << _RPWM << endl;
        _driver->setRightLeftPwm(_RPWM, _RDIR, _LPWM, _LDIR);

        if(state.buttons[0])
        {
            cout << "exit" << endl;
            _joy->disconnect();
            exit(0);
        }
    }
};

int main()
{
    JoystickMB joystick;
    if(joystick.isOpen())
    {
        OSMC_driver driver;
        TankDriver tankdrive(&joystick, &driver);
        cout << "Press button 1 to exit." << endl;
        while(joystick.isOpen()) { }
    } else {
        cout << "Could not open joystick, exiting." << endl;
    }
    return 0;
}
