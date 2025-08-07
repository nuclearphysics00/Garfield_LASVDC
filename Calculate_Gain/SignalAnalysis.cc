#include "SignalAnalysis.hh"
#include <cmath>

std::pair<double, double> ComputeTOT(Garfield::Sensor* sensor, const std::string& label, double threshold) {
  const int nBins = sensor->GetNumberOfTimeBins();
  double tRise = -1.0, tFall = -1.0;

  for (int i = 0; i < nBins; ++i) {
    const double t = sensor->GetTimeBinCenter(i);
    const double iSig = sensor->GetSignal(label, i);
    if (iSig > threshold && tRise < 0.0) {
      tRise = t;
    }
    if (tRise > 0.0 && iSig < threshold) {
      tFall = t;
      break;
    }
  }

  return {tRise, tFall};
}

double ComputeSignalCharge(Garfield::Sensor* sensor, const std::string& label) {
  const int nBins = sensor->GetNumberOfTimeBins();
  const double dt = sensor->GetTimeBinWidth();
  double q = 0.0;

  for (int i = 0; i < nBins; ++i) {
    q += sensor->GetSignal(label, i) * dt;
  }
  return q;
}
