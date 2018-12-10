#include <time.h>
#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

#include <TBrowser.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>

#include "TUserInterface.hpp"

Double_t fitFnc(Double_t *pos, Double_t *par)
{  // This should be class not function.

  auto x = pos[0];
  auto mean = par[1];
  auto sigma = par[2];

  auto facRange = 3;
  auto limitHigh = mean + facRange * sigma;
  auto limitLow = mean - facRange * sigma;

  auto val = par[0] * TMath::Gaus(x, mean, sigma);

  auto backGround = 0.;
  if (x < limitLow)
    backGround = par[3] + par[4] * x;
  else if (x > limitHigh)
    backGround = par[5] + par[6] * x;
  else {
    auto xInc = limitHigh - limitLow;
    auto yInc = (par[5] + par[6] * limitHigh) - (par[3] + par[4] * limitLow);
    auto slope = yInc / xInc;

    backGround = (par[3] + par[4] * limitLow) + slope * (x - limitLow);
  }

  if (backGround < 0.) backGround = 0.;
  val += backGround;

  return val;
}

TUserInterface::TUserInterface()
    : TGMainFrame(gClient->GetRoot(), 1100, 800),
      fHis(nullptr),
      fSliderFac(1000)
{
  fGaussian = new TF1("Gaussian", "gaus");
  fGaussian->SetLineColor(kGreen);
  fPeak = fMean = fSigma = 10.;

  fBackground =
      new TF1("Background", fitFnc, fMean - 5 * fSigma, fMean + 5 * fSigma, 7);
  fBackground->SetLineColor(kCyan);
  fLeftVal = 1000.;
  fRightVal = 500.;

  fFitFnc =
      new TF1("FitFnc", fitFnc, fMean - 5 * fSigma, fMean + 5 * fSigma, 7);
  fFitFnc->SetParName(1, "#it{#mu}");
  fFitFnc->SetParName(2, "#it{#sigma}");

  char buf[32];
  SetCleanup(kDeepCleanup);

  fMenuBar = new TGMenuBar(this, 800, 50, kHorizontalFrame);
  fMenuFile = new TGPopupMenu(gClient->GetRoot());
  fMenuFile->AddEntry("Browse", static_cast<Int_t>(MenuMessages::BROWSE));
  fMenuFile->AddEntry("Exit", static_cast<Int_t>(MenuMessages::EXIT));
  fMenuFile->Connect("Activated(Int_t)", "TUserInterface", this,
                     "HandleMenu(Int_t)");

  fMenuBar->AddPopup("&File", fMenuFile,
                     new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
  AddFrame(fMenuBar,
           new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 5));

  // Create an embedded canvas and add to the main frame, centered in x and y
  // and with 30 pixel margins all around
  fCanvas = new TRootEmbeddedCanvas("Canvas", this, 800, 600);
  fLHint = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10);
  AddFrame(fCanvas, fLHint);
  fCanvas->GetCanvas()->SetBorderMode(0);
  fCanvas->GetCanvas()->SetGrid();
  fCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                                "TUserInterface", this, "CanvasEvent()");

  fHFrame = new TGHorizontalFrame(this, 0, 0, 0);
  fVFrame = new TGVerticalFrame(this, 0, 0, 0);
  fVFrame->Resize(800, 25);

  fHFrame->SetName("Hframe2");
  fLeftSlider =
      new TGVSlider(fHFrame, 134, kSlider1 | kScaleBoth, -1, kVerticalFrame);
  fLeftSlider->SetName("LeftSlider");
  fLeftSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                       "DoLeftSlider()");
  fHFrame->AddFrame(fLeftSlider,
                    new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));

  fPeakSlider = new TGVSlider(fHFrame, 134, kSlider1 | kScaleDownRight, -1,
                              kVerticalFrame);
  fPeakSlider->SetName("PeakSlider");
  fPeakSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                       "DoPeakSlider()");
  fHFrame->AddFrame(fPeakSlider,
                    new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));

  fRightSlider =
      new TGVSlider(fHFrame, 134, kSlider1 | kScaleBoth, -1, kVerticalFrame);
  fRightSlider->SetName("RightSlider");
  fRightSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                        "DoRightSlider()");
  fHFrame->AddFrame(fRightSlider,
                    new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));

  fSigmaSlider =
      new TGHSlider(fVFrame, 800, kSlider1 | kScaleBoth, -1, kHorizontalFrame);
  fSigmaSlider->SetName("SigmaSlider");
  fSigmaSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                        "DoSigmaSlider()");
  fVFrame->AddFrame(fSigmaSlider,
                    new TGLayoutHints(kLHintsExpandX | kLHintsNormal));

  fMeanSlider =
      new TGHSlider(fVFrame, 800, kSlider1 | kScaleBoth, -1, kHorizontalFrame);
  fMeanSlider->SetName("MeanSlider");
  fMeanSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                       "DoMeanSlider()");
  fVFrame->AddFrame(fMeanSlider,
                    new TGLayoutHints(kLHintsExpandX | kLHintsNormal));

  fFitButton =
      new TGTextButton(fHFrame, "Fit", -1, TGTextButton::GetDefaultGC()(),
                       TGTextButton::GetDefaultFontStruct(), kRaisedFrame);
  fFitButton->SetMargins(10, 10, 5, 5);
  fFitButton->Connect("Clicked()", "TUserInterface", this, "DoFit()");
  fHFrame->AddFrame(fFitButton, new TGLayoutHints(kLHintsNormal));
  // fVFrame->AddFrame(fMeanSlider, new TGLayoutHints(kLHintsNormal));
  // fVFrame->AddFrame(fSigmaSlider, new TGLayoutHints(kLHintsNormal));

  fUploadButton =
      new TGTextButton(fHFrame, "Upload", -1, TGTextButton::GetDefaultGC()(),
                       TGTextButton::GetDefaultFontStruct(), kRaisedFrame);
  fUploadButton->SetMargins(10, 10, 5, 5);
  fUploadButton->Connect("Clicked()", "TUserInterface", this, "DoUpload()");
  fHFrame->AddFrame(fUploadButton, new TGLayoutHints(kLHintsNormal));

  AddFrame(fVFrame, new TGLayoutHints(kLHintsNormal));
  AddFrame(fHFrame,
           new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1));

  SetWindowName("Fitter for GBS");
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
}

//______________________________________________________________________________
TUserInterface::~TUserInterface()
{
  // Clean up

  delete fGaussian;
  delete fBackground;
  delete fFitFnc;

  Cleanup();
}

//______________________________________________________________________________
void TUserInterface::CloseWindow()
{
  // Called when window is closed via the window manager.
  delete this;
}

void TUserInterface::CanvasEvent()
{
  // This function just check the histogram ploted in the canvas
  // If the histogram is not same as fHis, replace
  // This call all events now.  I have no idea about other signal.
  auto list = fCanvas->GetCanvas()->GetListOfPrimitives();
  for (auto &&obj : *list) {
    if (obj->InheritsFrom("TH1")) {
      auto his = (TH1 *)fCanvas->GetCanvas()->GetPrimitive(obj->GetName());
      if (fHis == nullptr || fHis != his) {
        fHis = his;

        fMean = fHis->GetXaxis()->GetBinCenter(fHis->GetMaximumBin());

        fPeak = fHis->GetMaximum();
        fLeftSlider->SetRange(0, fPeak);
        fLeftSlider->SetPosition(fPeak - fLeftVal);
        fPeakSlider->SetRange(0, fPeak);
        fPeakSlider->SetPosition(0);
        fRightSlider->SetRange(0, fPeak);
        fRightSlider->SetPosition(fPeak - fRightVal);
        fSigmaSlider->SetRange(0, fHis->GetRMS() * fSliderFac);
        fSigmaSlider->SetPosition(fSigma * fSliderFac);
        fMeanSlider->SetRange(
            fHis->GetXaxis()->GetBinCenter(1) * fSliderFac,
            fHis->GetXaxis()->GetBinCenter(fHis->GetNbinsX()) * fSliderFac);
        fMeanSlider->SetPosition(fMean * fSliderFac);
      }
    }
  }
}

void TUserInterface::DoPeakSlider()
{
  auto pos = fPeakSlider->GetPosition();
  auto max = fHis->GetMaximum();
  fPeak = max - pos;
  UpdateGraph();
}

void TUserInterface::DoLeftSlider()
{
  auto pos = fLeftSlider->GetPosition();
  auto max = fLeftSlider->GetMaxPosition();
  fLeftVal = max - pos;
  UpdateGraph();
}

void TUserInterface::DoRightSlider()
{
  auto pos = fRightSlider->GetPosition();
  auto max = fRightSlider->GetMaxPosition();
  fRightVal = max - pos;
  UpdateGraph();
}

void TUserInterface::DoSigmaSlider()
{
  auto pos = fSigmaSlider->GetPosition();
  fSigma = pos / fSliderFac;
  UpdateGraph();
}

void TUserInterface::DoMeanSlider()
{
  auto pos = fMeanSlider->GetPosition();
  fMean = pos / fSliderFac;
  UpdateGraph();
}

void TUserInterface::DoFit()
{
  gStyle->SetOptFit(kTRUE);

  if (fHis) {
    fFitFnc->SetParameters(fPeak, fMean, fSigma, fLeftVal, 0., fRightVal, 0.);

    fFitFnc->SetRange(fMean - 5 * fSigma, fMean + 5 * fSigma);
    fHis->Fit(fFitFnc, "R");

    fPeak = fFitFnc->GetParameter(0);
    fPeakSlider->SetPosition(fPeakSlider->GetMaxPosition() - fPeak);

    fMean = fFitFnc->GetParameter(1);
    fMeanSlider->SetPosition(fMean * fSliderFac);

    fSigma = fFitFnc->GetParameter(2);
    fSigmaSlider->SetPosition(fSigma * fSliderFac);

    fLeftVal = fFitFnc->GetParameter(3);
    fLeftSlider->SetPosition(fLeftSlider->GetMaxPosition() - fLeftVal);

    fRightVal = fFitFnc->GetParameter(5);
    fRightSlider->SetPosition(fRightSlider->GetMaxPosition() - fRightVal);

    fGaussian->SetParameters(fPeak, fMean, fSigma);
    fGaussian->SetRange(fMean - 5 * fSigma, fMean + 5 * fSigma);
    fGaussian->Draw("SAME");

    fBackground->SetParameters(0, fMean, fSigma, fLeftVal,
                               fFitFnc->GetParameter(4), fRightVal,
                               fFitFnc->GetParameter(6));
    fBackground->SetRange(fMean - 5 * fSigma, fMean + 5 * fSigma);
    fBackground->Draw("SAME");

    fCanvas->GetCanvas()->Modified();
    fCanvas->GetCanvas()->Update();
  }
}

void TUserInterface::DoUpload()
{
  auto FWHM = fSigma * 2 * sqrt(2 * log(2));
  std::cout << "Mean: " << fMean << "\n"
            << "FWHM: " << FWHM << std::endl;

  // Do image save
  // I expect nobody do at the same time
  // But such the expection is done by only the fool
  auto fileName = TString(Form("fit-%ld.jpg", time(nullptr)));
  auto fullPath = "/home/aogaki/DAQ/Outputs/images/" + fileName;
  fCanvas->GetCanvas()->Print(fullPath, "jpg");

  // Connect to Mongo DB
  mongocxx::client conn{mongocxx::uri{"mongodb://192.168.161.73/"}};
  // mongocxx::client conn{mongocxx::uri{}};

  auto collection = conn["node-angular"]["posts"];

  bsoncxx::builder::stream::document buf{};
  buf << "title" << Form("%2.5f", fMean) << "content" << Form("%2.5f", FWHM)
      << "imagePath"
      << Form("http://192.168.161.73:3000/images/%s", fileName.Data());
  collection.insert_one(buf.view());
  buf.clear();

  auto cursor = collection.find({});
  for (auto &&doc : cursor) {
    std::cout << bsoncxx::to_json(doc) << std::endl;
  }
}

void TUserInterface::UpdateGraph()
{
  fCanvas->GetCanvas()->cd();
  if (fHis) fHis->Draw();

  fGaussian->SetParameters(fPeak, fMean, fSigma);
  fGaussian->SetRange(fMean - 5 * fSigma, fMean + 5 * fSigma);
  fGaussian->Draw("SAME");

  fBackground->SetParameters(0, fMean, fSigma, fLeftVal, 0., fRightVal, 0.);
  fBackground->SetRange(fMean - 5 * fSigma, fMean + 5 * fSigma);
  fBackground->Draw("SAME");

  fCanvas->GetCanvas()->Modified();
  fCanvas->GetCanvas()->Update();
}

void TUserInterface::HandleMenu(Int_t menuID)
{
  MenuMessages id = static_cast<MenuMessages>(menuID);
  switch (id) {
    case MenuMessages::BROWSE:
      new TBrowser();
      break;
    case MenuMessages::EXIT:
      Cleanup();
      gSystem->Exit(0);  // Too rude?
      break;
    default:
      break;
  }
}
