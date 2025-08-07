#include <TApplication.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <cstdlib>  // for system()

#include "IonizationCalculator.hh"
#include "DetectorSetup.hh"
#include "SignalSimulation.hh"
#include "Garfield/ViewSignal.hh"

int main(int argc, char* argv[]) {
  TApplication app("app", &argc, argv);

  // ----------------------------
  // 1 パラメータ設定
  // ----------------------------
  const double theta = 45.0;           // [deg]
  const double spacing_mm = 6.0;       // [mm]
  const double dEdx = 2.4;             // [MeV cm²/g]
  const double rho = 1.796e-3;         // [g/cm³]
  const double ionE = 25.0e-6;         // [MeV]

  const double path_cm = IonizationCalculator::ComputeEffectivePath(spacing_mm, theta);
  const double deltaE = IonizationCalculator::EstimateEnergyLoss(dEdx, rho, path_cm);
  const int nElectrons = IonizationCalculator::ComputeNumElectrons(deltaE, ionE);
  std::cout << "生成される種（電子）数 = " << nElectrons << std::endl;

  // ----------------------------
  // 2 検出器構成
  // ----------------------------
  Garfield::MediumMagboltz* gas = nullptr;
  Garfield::ComponentAnalyticField* cmp = nullptr;
  Garfield::Sensor* sensor = nullptr;
  SetupDetector(gas, cmp, sensor);
  std::cout << "Checkpoint1" << std::endl;

  // ----------------------------
  // 3 電子の注入
  // ----------------------------
  InjectElectrons(sensor, nElectrons);
  std::cout << "Checkpoint2" << std::endl;

  // ----------------------------
  // 4 各アノードの波形表示
  // ----------------------------
  auto viewL = new Garfield::ViewSignal();
  auto viewC = new Garfield::ViewSignal();
  auto viewR = new Garfield::ViewSignal();

  viewL->SetSensor(sensor);
  viewC->SetSensor(sensor);
  viewR->SetSensor(sensor);

  viewL->PlotSignal("anodeL");
  viewC->PlotSignal("anodeC");
  viewR->PlotSignal("anodeR");

  viewL->GetCanvas()->SetTitle("Signal from Left Anode");
  viewC->GetCanvas()->SetTitle("Signal from Center Anode");
  viewR->GetCanvas()->SetTitle("Signal from Right Anode");

  std::cout << "Checkpoint3" << std::endl;

  // ----------------------------
  // 5 信号積分とゲイン計算
  // ----------------------------
  const std::vector<std::string> electrodes = {"anodeL", "anodeC", "anodeR"};
  const double e_C = 1.602e-19;
  const double tMin = 0.0;
  const double tMax = 100.0;
  const int nBins = 500;
  const double binWidth_ns = (tMax - tMin) / nBins;

  // After
  const double vAnode = GetAnodeVoltage();
  const double vCathode = GetCathodeVoltage();

  std::time_t t = std::time(nullptr);
  char timeStr[20];
  std::strftime(timeStr, sizeof(timeStr), "%Y%m%d-%H%M%S", std::localtime(&t));

  // ----------------------------
  // 6 出力ディレクトリ作成とファイル保存
  // ----------------------------
  const std::string outDir = "results";
  std::ostringstream subdir;
  subdir << outDir << "/VA" << std::abs((int)vAnode)
         << "_VC" << std::abs((int)vCathode)
         << "_" << timeStr;
  const std::string outPath = subdir.str();

  // ディレクトリ作成
  std::string mkdirCmd = "mkdir -p " + outPath;
  system(mkdirCmd.c_str());

  // --- テキストファイル保存 ---
  std::ostringstream txtname;
  txtname << outPath << "/gain_summary.txt";
  std::ofstream ofs(txtname.str());

  ofs << "### Simulation Results\n";
  ofs << "DateTime: " << timeStr << "\n";
  ofs << "theta = " << theta << " deg\n";
  ofs << "dEdx = " << dEdx << " MeV*cm^2/g\n";
  ofs << "rho  = " << rho << " g/cm^3\n";
  ofs << "ionE = " << ionE << " MeV\n";
  ofs << "Path Length = " << path_cm << " cm\n";
  ofs << "deltaE = " << deltaE << " MeV\n";
  ofs << "nElectrons = " << nElectrons << "\n";
  ofs << "Anode Voltage: " << vAnode << " V\n";
  ofs << "Cathode Voltage: " << vCathode << " V\n\n";

  for (const auto& label : electrodes) {
    double totalSignal_fC = 0.0;
    for (int i = 1; i < nBins; ++i) {
      double s1 = sensor->GetSignal(label, i - 1);
      double s2 = sensor->GetSignal(label, i);
      double avg = 0.5 * (s1 + s2);
      totalSignal_fC += avg * binWidth_ns;
    }

    double totalCharge_C = totalSignal_fC * 1e-15;
    double Q0 = nElectrons * e_C;
    double gain = (Q0 > 0) ? totalCharge_C / Q0 : 0.0;

    std::cout << "\n[" << label << "]" << std::endl;
    std::cout << "  積分電荷量: " << totalSignal_fC << " fC" << std::endl;
    std::cout << "  ゲイン     : " << gain << std::endl;

    ofs << "[" << label << "]\n";
    ofs << "  積分電荷量: " << totalSignal_fC << " fC\n";
    ofs << "  ゲイン     : " << gain << "\n\n";
  }

  ofs.close();
  std::cout << "→ 結果ファイル書き出し: " << txtname.str() << std::endl;

  // --- log出力 ---
  for (const auto& [view, label] : {
      std::make_pair(viewL, "anodeL"),
      std::make_pair(viewC, "anodeC"),
      std::make_pair(viewR, "anodeR") }) {

    std::ostringstream filename;
    filename << outPath << "/signal_" << label << ".pdf";

    view->GetCanvas()->SaveAs(filename.str().c_str());
    std::cout << "Saved: " << filename.str() << std::endl;
  }


  return 0;
}
