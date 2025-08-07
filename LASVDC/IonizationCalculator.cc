#include "IonizationCalculator.hh"
#include <cmath>

int IonizationCalculator::ComputeNumElectrons(double deltaE, double ionE) {
  return static_cast<int>(deltaE / ionE);
}

double IonizationCalculator::EstimateEnergyLoss(double dEdx, double rho, double path_cm) {
  return dEdx * rho * path_cm;
}

double IonizationCalculator::ComputeEffectivePath(double spacing_mm, double angle_deg) {
  return (spacing_mm / 10.0) / std::sin(angle_deg * M_PI / 180.0);
}
