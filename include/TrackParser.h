// Written by Robin Verschueren, used and modified by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef TRACKPARSER_H
#define TRACKPARSER_H

#include <fstream>

// This class parses Matlab-script generated reference track files and provides
// the data e.g. for a control algorithm.
class TrackParser
{
public:
    // Constructor, pass the file (path)
    TrackParser(std::string filename);

    // Get the number of lines in the file.
    int getNbLines();

    // Takes allocated arrays with reference time, x, y and psi and fills them
    // with the track data.
    void parseTrack(double tref[], double xref[], double yref[], double psiref[]);

    // Close the trackfile.
    void close();

private:
    // Input file stream for parsing.
    std::ifstream trackfile;
};

#endif // TRACKPARSER_H
