#ifndef SIGNAL_ANALYSIS_H
#define SIGNAL_ANALYSIS_H

#include "Garfield/Sensor.hh"
#include <vector>
#include <string>

std::pair<double, double> ComputeTOT(Garfield::Sensor* sensor, const std::string& label, double threshold);
double ComputeSignalCharge(Garfield::Sensor* sensor, const std::string& label);

#endif
