#ifndef DETECTOR_SETUP_HH
#define DETECTOR_SETUP_HH

#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/Sensor.hh"
#include <vector>
#include <string>

using namespace Garfield;


  static double vAnode  = -300.0;   // [V]
  static double vCathode = -4000.0; // [V]

  
void SetupDetector(MediumMagboltz*& gas,
                   ComponentAnalyticField*& cmp,
                   Sensor*& sensor) {
  // --- ガス設定 ---
  gas = new MediumMagboltz();
  gas->SetComposition("ar", 71.4, "isobutane", 28.6);
  gas->Initialise();
  gas->SetMaxElectronEnergy(500.0);  // 高エネルギー対応
  gas->SetPressure(375.);         // ← ここで圧力を指定（Torr）
  gas->SetTemperature(293.15);    // 温度（任意、単位は K）

  // --- 構造パラメータ ---
  const double rAnode = 0.0010;    // [cm] アノード半径 (20μm)
  const double rField = 0.0025;    // [cm] フィールドワイヤー半径 (50μm)
  const double pitch   = 0.6;      // [cm] セル間隔
  const double spacing = 0.2;      // [cm] ポテンシャルワイヤーのオフセット
  const double gap = 1.0;          // [cm] アノード～カソード間距離

  // --- 構成オブジェクト初期化 ---
  cmp = new ComponentAnalyticField();
  cmp->SetMedium(gas);

  // --- アノードワイヤーとポテンシャルワイヤーの配置 ---
  std::vector<std::string> anodeTags = {"anodeL", "anodeC", "anodeR"};
  std::vector<double> anodeX = {-pitch, 0.0, pitch};

  for (size_t i = 0; i < anodeX.size(); ++i) {
    double x = anodeX[i];
    const std::string& tag = anodeTags[i];

    // アノードワイヤー（信号取得用）
    cmp->AddWire(x, 0.0, 2 * rAnode, vAnode, tag);

    // 左右にフィールドワイヤーを追加
    cmp->AddWire(x + spacing, 0.0, 2 * rField, 0.0, "potential");
    cmp->AddWire(x - spacing, 0.0, 2 * rField, 0.0, "potential");
  }

  // --- カソード面の追加 ---
  cmp->AddPlaneY(+gap, vCathode, "top");
  cmp->AddPlaneY(-gap, vCathode, "bottom");

  // --- センサーの構成（電極ごとに分離）---
  sensor = new Sensor();
  sensor->AddComponent(cmp);
  sensor->AddElectrode(cmp, "anodeL");
  sensor->AddElectrode(cmp, "anodeC");
  sensor->AddElectrode(cmp, "anodeR");
}

// --- Getter 実装 ---
double GetAnodeVoltage() {
  return vAnode;
}

double GetCathodeVoltage() {
  return vCathode;
}
#endif  // DETECTOR_SETUP_HH
