// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "../include/Common.h"
#include "../include/PDControl.h"
#include "../include/RCdriver.h"
#include "../include/UDPclient.h"
#include "../include/UDPserver.h"

using namespace std;

//_____________________________________________________________________________
int main() {

    {

        UDPserver us(9930);
        UDPclient toDisplay(9931);
        RCdriver rc;
        PDControl pdc;

        pdc.init();

        bool received = false;
        char buf[BUFLEN];

        int timestep = -1;
        double xPos = -1;
        double yPos = -1;
        double psi = -1;
        double xDerivative = -1;
        double yDerivative = -1;
        double psiDerivative = -1;
        double dummy = 0.0;

        int refIndex = 0;

        cout << "\nWaiting for Vision Data...\n";

        while (true) {

            long loopstart = microtime();

            us.listen(buf);
            sscanf(buf,"%i, %f, %f, %f, %f, %f, %f, %f",&timestep, &dummy, &xPos, &xDerivative, &yPos, &yDerivative, &psi, &psiDerivative);

            if (timestep < 0) {
                rc.setSpeed(0);
                rc.setSteer(0);
                continue;
            }

            // loop if track reference goes through starting point
            if (abs(pdc.xrefNow) < 0.001 && abs(pdc.yrefNow) < 0.001 && refIndex != 0) {
                refIndex = -1;
            }
            ++refIndex;

            double speed_control = 0;
            double steer_control = 0;

            pdc.getControls(refIndex, xPos, yPos, psi, speed_control, steer_control);

            if (refIndex == -1) {
                rc.setSpeed(0);
                rc.setSteer(0);
                continue;
            } else if (refIndex == -2) {
                rc.setSpeed(0);
                rc.setSteer(0);
                cout << "\nGot termination order. Exiting.\n";
                exit(0);
            }

            cout << "\n________________________________________________________________";
            cout << pdc.getInfo();

            char messageToDisplay[BUFLEN];
            int identifier = 1;
            sprintf(messageToDisplay, "%i, %i, %6.3f, %6.3f, %6.3f", identifier, refIndex, pdc.xrefNow, pdc.yrefNow, psi);
            toDisplay.send(messageToDisplay);

            rc.setSpeed(speed_control);
            rc.setSteer(steer_control);


            while (microtime()-loopstart < PERIOD);

        }
    }
    return 0;
}
