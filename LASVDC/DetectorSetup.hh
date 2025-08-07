#ifndef DETECTOR_SETUP_H
#define DETECTOR_SETUP_H

#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/Sensor.hh"

void SetupDetector(Garfield::MediumMagboltz*& gas,
                   Garfield::ComponentAnalyticField*& cmp,
                   Garfield::Sensor*& sensor);
// 追加：電圧を取得する getter 関数
double GetAnodeVoltage();
double GetCathodeVoltage();
#endif
