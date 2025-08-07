#ifndef IONIZATION_CALCULATOR_H
#define IONIZATION_CALCULATOR_H

class IonizationCalculator {
public:
  // 入射角・電離エネルギーなどを元に、生成される種（電子）数を返す
  static int ComputeNumElectrons(double energyLossMeV, double ionizationEnergyMeV);
  static double EstimateEnergyLoss(double stoppingPower, double density, double pathLength_cm);
  static double ComputeEffectivePath(double wireSpacing_mm, double angle_deg);
};

#endif
