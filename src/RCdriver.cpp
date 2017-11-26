// Written by Robin Verschueren, used and modified by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <comedilib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

#include "../include/RCdriver.h"

using namespace std;

//_____________________________________________________________________________
RCdriver::RCdriver() {
    
    outputSubdevice = 1;
    range = 0;
    aref = 0;
    default_filename = "/dev/comedi0";
    device = comedi_open(default_filename);
    if(!device){
        comedi_perror(default_filename);
        exit(-1);
    }
    else {
        printf("Opened output device succesfully\n");
    }
}

//_____________________________________________________________________________
void RCdriver::putData(int channel, int data) {
    if(channel == 5) printf("(int)throttle value: %d\n", data);
    //if(channel == 6) printf("steering value: %d /n", data);
    int ret = comedi_data_write(device, outputSubdevice, channel, range, aref, data);
    if(ret < 0){
        comedi_perror(default_filename);
        exit(-1);
    }
}

void RCdriver::putVoltage(int channel, double voltage) {
    
    int maxdata = comedi_get_maxdata(device, outputSubdevice, channel);
    int data = comedi_from_phys(voltage, comedi_get_range(device, outputSubdevice, channel, range), maxdata);
    int ret = comedi_data_write(device, outputSubdevice, channel, range, aref, data);   
    if(ret < 0){
        comedi_perror(default_filename);
        exit(-1);
    }
}

//_____________________________________________________________________________
void RCdriver::setSpeed(double speed) {
    if (speed > 200.0) {
        speed = 200.0;
    } else if (speed < -100.0) {
        speed = -100.0;
    }
    double control;
    if (speed >= 0) {
        control = 1.66 + speed / 200;
    } else {
        control = 1.3 + speed / 200;
    }
//  cout << "\nspeed control voltage set to: " << control;
    putVoltage(THROTTLE, control);
}

//_____________________________________________________________________________
void RCdriver::setSteer(double steer) {
    if (steer > 100.0) {
        steer = 100.0;
    } else if (steer < -100.0) {
        steer = -100.0;
    }

    double control = 1.50 + steer / 100;
//  cout << "\nsteer control voltage set to: " << control;
    putVoltage(STEER, control);
}
