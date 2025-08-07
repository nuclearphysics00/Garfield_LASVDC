#include <TApplication.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TMarker.h>
#include <TROOT.h>

#include "Garfield/ComponentAnalyticField.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ViewField.hh"

using namespace Garfield;

int main(int argc, char* argv[]) {
  TApplication app("app", &argc, argv);

  // --- ガス設定 ---
  MediumMagboltz gas;
  gas.SetComposition("ar", 100);  // アルゴン
  gas.SetPressure(80.0);  // 気圧を 1/10 の 80 Torrに設定
  gas.Initialise();

  // --- 電場コンポーネント（解析電場） ---
  ComponentAnalyticField cmp;
  cmp.SetMedium(&gas);

  // --- 幾何パラメータ ---
  const double radiusSenseWire = 0.0010;        // アノード半径 [cm]（20μm）
  const double radiusPotentialWire = 0.0025;    // ポテンシャル線半径 [cm]（50μm）
  const double senseWireSpacing = 0.6;          // アノード間隔（センター間）[cm]
  const double potentialWirePitch = 0.2;        // ポテンシャルワイヤ間隔 [cm]
  const double voltageSense = 0.0;              // アノード電位 [V]
  const double voltagePotential = -300.0;       // ポテンシャルワイヤ電位 [V]
  const double voltageCathode = -5500.0;        // カソード面電位 [V]

  // --- ワイヤ配置 ---
  std::vector<double> anodeXPositions;
  std::vector<double> potentialXPositions;

  for (int wireIndex = -3; wireIndex <= 3; ++wireIndex) {
    double xAnode = wireIndex * senseWireSpacing;
    cmp.AddWire(xAnode, 0.0, 2 * radiusSenseWire, voltageSense, "sense");
    anodeXPositions.push_back(xAnode);

    if (wireIndex != 3) {
      double xPotential1 = xAnode + potentialWirePitch;
      double xPotential2 = xAnode + 2 * potentialWirePitch;
      cmp.AddWire(xPotential1, 0.0, 2 * radiusPotentialWire, voltagePotential, "potential");
      cmp.AddWire(xPotential2, 0.0, 2 * radiusPotentialWire, voltagePotential, "potential");
      potentialXPositions.push_back(xPotential1);
      potentialXPositions.push_back(xPotential2);
    }
  }

  // --- 上下方向の境界（Y軸方向） ---
  cmp.AddPlaneY(+1.0, voltageCathode, "cathode_top");    // 上カソード
  cmp.AddPlaneY(-1.0, voltageCathode, "cathode_bottom"); // 下カソード

  // --- 描画設定 ---
  TCanvas* canvas = new TCanvas("canvas", "MWDC FieldLines", 800, 600);
  ViewField view;
  view.SetCanvas(canvas);
  view.SetComponent(&cmp);
  view.SetArea(-2.0, -1.2, 2.0, 1.2);         // 描画範囲
  view.SetVoltageRange(-5500, 0);            // 等電位線の電圧スケール
  view.SetNumberOfContours(20);              // 等電位線の本数
  view.PlotContour();

  // --- アノード（赤）・ポテンシャルワイヤ（青）を可視化 ---
  for (const auto& xAnode : anodeXPositions) {
    TMarker* marker = new TMarker(xAnode, 0.0, 20);  // ● 赤
    marker->SetMarkerColor(kRed);
    marker->SetMarkerSize(1.0);
    marker->Draw("same");
  }

  for (const auto& xPotential : potentialXPositions) {
    TMarker* marker = new TMarker(xPotential, 0.0, 21);  // ■ 青
    marker->SetMarkerColor(kBlue);
    marker->SetMarkerSize(1.0);
    marker->Draw("same");
  }

  // --- カソード線（上下）を可視化（破線） ---
  TLine* lineTop = new TLine(-2.0, 1.0, 2.0, 1.0);
  TLine* lineBottom = new TLine(-2.0, -1.0, 2.0, -1.0);
  lineTop->SetLineStyle(2);
  lineBottom->SetLineStyle(2);
  lineTop->Draw("same");
  lineBottom->Draw("same");

  // --- 電気力線の種点を格子状に生成 ---
  std::vector<double> xSeeds, ySeeds, zSeeds;
  const double xMin = -1.5, xMax = 1.5;
  const double yMin = -1.0, yMax = 1.0;
  const int nXSeeds = 15;
  const int nYSeeds = 15;

  for (int ix = 0; ix < nXSeeds; ++ix) {
    double xSeed = xMin + ix * (xMax - xMin) / (nXSeeds - 1);
    for (int iy = 0; iy < nYSeeds; ++iy) {
      double ySeed = yMin + iy * (yMax - yMin) / (nYSeeds - 1);
      xSeeds.push_back(xSeed);
      ySeeds.push_back(ySeed);
      zSeeds.push_back(0.0);
    }
  }

  // --- 電気力線を描画 ---
  view.PlotFieldLines(xSeeds, ySeeds, zSeeds, false, false);

  // --- PDF出力 ---
  canvas->SaveAs("MWDC_LAS_Field.pdf");

  app.Run(kTRUE);
  return 0;
}
