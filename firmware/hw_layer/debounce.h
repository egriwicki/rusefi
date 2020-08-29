#include "global.h"
#include "globalaccess.h"

class ButtonDebounce {
public:
    ButtonDebounce(int t, brain_pin_e p, iomode_t m);
    bool readPinEvent();
private:
    bool checkThreshold(efitimems_t);
    int threshold;
    efitimems_t timeLast;
    brain_pin_e pin;
    iomode_t mode;
    bool readValue;
};
