#include "SignalSimulation.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include <iostream>

using namespace Garfield;

void InjectElectrons(Sensor* sensor, int nElectrons) {
  AvalancheMicroscopic* aval = new AvalancheMicroscopic();
  aval->SetSensor(sensor);
  aval->EnableSignalCalculation();
  aval->EnableAvalancheSizeLimit(50000);  // 2次電子数の制限（安全対策）

const double xStart = 0.9;
const double xEnd = -0.9;  // 18 mm
const double yStart = 1.0;
const double yEnd = -1.0;  // 2 cm

std::cout << "Checkpoint: Electron injection loop begins." << std::endl;

for (int i = 0; i < nElectrons; ++i) {
  double f = static_cast<double>(i) / (nElectrons - 1);

  double x = xStart + f * (xEnd - xStart);
  double y = yStart + f * (yEnd - yStart);

  std::cout << "[Electron " << i + 1 << "] x = " << x
            << ", y = " << y << ", z = 0.0" << std::endl;

  aval->AvalancheElectron(x, y, 0.0, 0.0, 0.0, 0.0, 0.0);

  std::cout << "[Electron " << i + 1 << "] injected successfully." << std::endl;
}

std::cout << "Checkpoint: Electron injection loop completed." << std::endl;
}
