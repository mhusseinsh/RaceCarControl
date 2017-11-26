// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef PDCONTROL_H_
#define PDCONTROL_H_

#include <string>
#include <vector>

// This class implements a simple PD control and is meant for testing and
// demonstrating the vision code.
class PDControl {

public:
    // Contructor
    PDControl();

    // Initialize everything.
    void init();

    // Calculates control output (speed, steer) from reference index (line of the trackfile) and pose (x, y, psi).
    int getControls(int refIndex, double xPos, double yPos, double psi, double &speed_control, double &steer_control);

    // Get info string.
    std::string getInfo();

    // Enable debugging output.
    void setDebug(bool debug);

    // Recent reference positions (for info output).
    double xrefNow;
    double yrefNow;

    double tracklength;
    std::vector<double> tref;
    std::vector<double> xref;
    std::vector<double> yref;
    std::vector<double> psiref;

    // P and D constants
    double p_const_parallel;
    double d_const_parallel;
    double p_const_lateral;
    double d_const_lateral;

    // variables for PD control
    double p_error_parallel;
    double p_error_parallel_old;
    double p_error_lateral;
    double p_error_lateral_old;
    double d_error_parallel;
    double d_error_lateral;

    int numLines;

    bool debug;

    std::string info_string;
};

#endif // PDCONTROL_H_
