// Written by Robin Verschueren, used and modified by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef RCDRIVER_H
#define RCDRIVER_H

#include <comedilib.h>

// Channels of the NI DAQ
#define THROTTLE 5
#define STEER 6

// This class' purpose is to convert control inputs to a voltage which is then
// applied to the hacked 2.4GHz transmitter via the NI data acquisition card.
class RCdriver
{
public:
    // Constructor
    RCdriver();

    // Set speed / control inputs.
    // (Functions written and voltage mapped by Johannes)
    void setSpeed(double);
    void setSteer(double);

private:

    // Data for initialization
    char const *default_filename;
    comedi_t *device;
    int outputSubdevice;
    int range;
    int aref;

    // put Data / Voltage to DAQ channel
    // (Functions written by Robin)
    void putData(int channel, int data);
    void putVoltage(int channel, double voltage);
};

#endif //RCDRIVER_H
