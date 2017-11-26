// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include "../include/PDControl.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/Common.h"
#include "../include/TrackParser.h"

using namespace std;

//_____________________________________________________________________________
PDControl::PDControl() {

    xrefNow = 0;
    yrefNow = 0;

    // P and D constants
    p_const_parallel = 500;
    d_const_parallel = 1000;
    p_const_lateral  = 4000;
    d_const_lateral  = 4000;

    p_error_parallel_old = 0;
    p_error_lateral_old = 0;
    p_error_parallel = 0;
    p_error_lateral = 0;
    d_error_parallel = 0;
    d_error_lateral = 0;

    // TODO: correct initialization
    debug = false;
    tracklength = 0;
    numLines = 0;
}

//_____________________________________________________________________________
void PDControl::init() {

    TrackParser tp(TRACK);

    // parse track
    numLines = tp.getNbLines();
    tref.reserve(numLines);
    xref.reserve(numLines);
    yref.reserve(numLines);
    psiref.reserve(numLines);
    tp.parseTrack(&tref[0], &xref[0], &yref[0], &psiref[0]);

    int loop_points = 0;
    // calculate tracklength
    for (int i = 0; i < numLines; ++i) {
        tracklength += sqrt(pow(xref[i+1]-xref[i], 2) + pow(yref[i+1]-yref[i], 2));

        ++ loop_points;

        // only consider 1 loop
        if (abs(xref[i]) < 0.001 && abs(yref[i]) < 0.001 && i != 0) {
            break;
        }
    }

    double frequency = 1000000 / (double)PERIOD;
    double ref_speed = tracklength * frequency / (double)loop_points;

    cout << "\nLoaded track:           \t" << TRACK;
    cout << "\ntrack length (m):       \t" << tracklength;
    cout << "\ncontrol frequency (Hz): \t" << frequency;
    cout << "\nloop points:            \t" << loop_points;
    cout << "\nreference speed (m/s):  \t" << ref_speed;
    cout << "\n";
}

//_____________________________________________________________________________
int PDControl::getControls(int refIndex, double xPos, double yPos, double psi, double &speed_control, double &steer_control) {

    xrefNow = xref[refIndex];
    yrefNow = yref[refIndex];

    // first translation then rotation gives p-errors
    // ATTENTION: because of weird coordinate system (not my idea) y_error has different sign!
    p_error_parallel =  (xref[refIndex] - xPos) * cos(psi) - (-yref[refIndex] + yPos) * sin(psi);
    p_error_lateral  =  (xref[refIndex] - xPos) * sin(psi) + (-yref[refIndex] + yPos) * cos(psi);

    // calculate d-error from history
    d_error_parallel = p_error_parallel - p_error_parallel_old;
    d_error_lateral  = p_error_lateral  - p_error_lateral_old;

    // save history
    p_error_parallel_old = p_error_parallel;
    p_error_lateral_old  = p_error_lateral;

    // calculate controls
    speed_control = p_error_parallel * p_const_parallel + d_error_parallel * d_const_parallel;
    steer_control = p_error_lateral  * p_const_lateral  + d_error_lateral  * d_const_lateral;

    stringstream info;
    info << "\n[PDControl] tref:            \t" << tref[refIndex];
    info << "\n[PDControl] refIndex:        \t" << refIndex;
    info << "\n[PDControl] xref:            \t" << xref[refIndex];
    info << "\n[PDControl] xPos:            \t" << xPos;
    info << "\n[PDControl] yref:            \t" << yref[refIndex];
    info << "\n[PDControl] yPos:            \t" << yPos;
    info << "\n[PDControl] psiref:          \t" << psiref[refIndex];
    info << "\n[PDControl] psi:             \t" << psi;
    info << "\n[PDControl] p_error_parallel:\t" << p_error_parallel;
    info << "\n[PDControl] p_error_lateral: \t" << p_error_lateral;
    info << "\n[PDControl] d_error_parallel:\t" << d_error_parallel;
    info << "\n[PDControl] d_error_lateral: \t" << d_error_lateral;
    info << "\n[PDControl] speed_control:   \t" << speed_control;
    info << "\n[PDControl] steer_control:   \t" << steer_control;
    info << "\n[PDControl] numLines:        \t" << numLines;
    info_string = info.str();

    return 0;
}

//_____________________________________________________________________________
string PDControl::getInfo() {
    return info_string;
}

//_____________________________________________________________________________
void PDControl::setDebug(bool dbg) {
    debug = dbg;
}

