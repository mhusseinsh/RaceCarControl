// Written by Robin Verschueren, used by:
// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "../include/TrackParser.h"

//_____________________________________________________________________________
TrackParser::TrackParser(std::string filename) {
    
    trackfile.open(filename.c_str());

}

//_____________________________________________________________________________
int TrackParser::getNbLines() {
    
    // put file back to beginning
    trackfile.clear();
    trackfile.seekg(0, trackfile.beg);
    
    int number_of_lines = 0;
    std::string line;
    if (trackfile.is_open()) {
        while (std::getline(trackfile, line))
            ++number_of_lines;
        std::cout << "Number of lines in text file: " << number_of_lines << std::endl;
    }
    else {
        std::cout << "Unable to open trackfile"; 
        exit(-1);   
    }
    return number_of_lines;
}

//_____________________________________________________________________________
void TrackParser::parseTrack(double tref[], double xref[], double yref[], double psiref[]) {
    
    std::string line;
    int i = 0;
    
    if (trackfile.is_open()) {
        
        // put file back to beginning
        trackfile.clear();
        trackfile.seekg(0, trackfile.beg);
        while ( getline (trackfile,line) )
        {   
            sscanf(line.c_str(),"%e %e %e %e", &tref[i], &xref[i], &yref[i], &psiref[i]);
            i++;
        }
    }
    else {
        std::cout << "Unable to open trackfile"; 
        exit(-1);
    }
    
}

//_____________________________________________________________________________
void TrackParser::close() {
        
    trackfile.close();
}
