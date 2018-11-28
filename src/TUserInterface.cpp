#include <iostream>

#include <TStyle.h>

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
    : TGMainFrame(gClient->GetRoot(), 1100, 800), fHis(nullptr)
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

  char buf[32];
  SetCleanup(kDeepCleanup);
  // Create an embedded canvas and add to the main frame, centered in x and y
  // and with 30 pixel margins all around
  fCanvas = new TRootEmbeddedCanvas("Canvas", this, 800, 600);
  fLcan = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10);
  AddFrame(fCanvas, fLcan);
  fCanvas->GetCanvas()->SetBorderMode(0);
  fCanvas->GetCanvas()->SetGrid();
  // fCanvas->Connect("ProcessedEvent(Event_t*)", "TUserInterface", this,
  //                  "CanvasEvent()");
  fCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                                "TUserInterface", this, "CanvasEvent()");

  fHframe1 = new TGHorizontalFrame(this, 0, 0, 0);
  fHslider1 = new TGTripleHSlider(fHframe1, 800, kDoubleScaleBoth, HSId1,
                                  kHorizontalFrame, GetDefaultFrameBackground(),
                                  kFALSE, kFALSE, kFALSE, kFALSE);
  fHslider1->Connect("PointerPositionChanged()", "TUserInterface", this,
                     "DoSlider()");
  fHslider1->Connect("PositionChanged()", "TUserInterface", this, "DoSlider()");
  fHslider1->SetRange(0.0, 500.0);
  fHframe1->Resize(800, 25);

  fHframe2 = new TGHorizontalFrame(this, 0, 0, 0);
  fHframe2->SetName("Hframe2");
  fLeftSlider =
      new TGVSlider(fHframe2, 134, kSlider1 | kScaleBoth, -1, kVerticalFrame);
  fLeftSlider->SetName("LeftSlider");
  fLeftSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                       "DoLeftSlider()");
  fHframe2->AddFrame(fLeftSlider,
                     new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));

  fPeakSlider = new TGVSlider(fHframe2, 134, kSlider1 | kScaleDownRight, -1,
                              kVerticalFrame);
  fPeakSlider->SetName("PeakSlider");
  fPeakSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                       "DoPeakSlider()");
  fHframe2->AddFrame(fPeakSlider,
                     new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));

  fRightSlider =
      new TGVSlider(fHframe2, 134, kSlider1 | kScaleBoth, -1, kVerticalFrame);
  fRightSlider->SetName("RightSlider");
  fRightSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                        "DoRightSlider()");
  fHframe2->AddFrame(fRightSlider,
                     new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 2, 2));

  fSigmaSlider =
      new TGHSlider(fHframe2, 134, kSlider1 | kScaleBoth, -1, kHorizontalFrame);
  fSigmaSlider->SetName("SigmaSlider");
  fSigmaSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                        "DoSigmaSlider()");
  fHframe2->AddFrame(fSigmaSlider, new TGLayoutHints(kLHintsNormal));

  fMeanSlider =
      new TGHSlider(fHframe2, 134, kSlider1 | kScaleBoth, -1, kHorizontalFrame);
  fMeanSlider->SetName("MeanSlider");
  fMeanSlider->Connect("PositionChanged(Int_t)", "TUserInterface", this,
                       "DoMeanSlider()");
  fHframe2->AddFrame(fMeanSlider, new TGLayoutHints(kLHintsNormal));

  fFitButton =
      new TGTextButton(fHframe2, "Fit", -1, TGTextButton::GetDefaultGC()(),
                       TGTextButton::GetDefaultFontStruct(), kRaisedFrame);
  fFitButton->SetMargins(10, 10, 5, 5);
  fFitButton->Connect("Clicked()", "TUserInterface", this, "DoFit()");
  fHframe2->AddFrame(fFitButton, new TGLayoutHints(kLHintsNormal));

  fUploadButton =
      new TGTextButton(fHframe2, "Upload", -1, TGTextButton::GetDefaultGC()(),
                       TGTextButton::GetDefaultFontStruct(), kRaisedFrame);
  fUploadButton->SetMargins(10, 10, 5, 5);
  fUploadButton->Connect("Clicked()", "TUserInterface", this, "DoUpload()");
  fHframe2->AddFrame(fUploadButton, new TGLayoutHints(kLHintsNormal));

  // //--- layout for buttons: top align, equally expand horizontally
  fBly = new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5, 5, 5, 5);
  //
  // //--- layout for the frame: place at bottom, right aligned
  // fBfly1 = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 5, 5);
  // fBfly2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
  // fBfly3 = new TGLayoutHints(kLHintsTop | kLHintsRight, 5, 5, 5, 5);
  //
  // fHframe0->AddFrame(fCheck1, fBfly2);
  // fHframe0->AddFrame(fCheck2, fBfly2);
  fHframe1->AddFrame(fHslider1, fBly);
  // fHframe2->AddFrame(fTeh1, fBfly2);
  // fHframe2->AddFrame(fTeh2, fBfly1);
  // fHframe2->AddFrame(fTeh3, fBfly3);
  //
  // AddFrame(fHframe0, fBly);
  AddFrame(fHframe1, fBly);
  AddFrame(fHframe2,
           new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1));
  // AddFrame(fHframe2, fBly);
  //
  // Set main frame name, map sub windows (buttons), initialize layout
  // algorithm via Resize() and map main frame
  SetWindowName("Fitter for GBS");
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  //
  // fFitFcn = new TF1("fFitFcn", "TMath::LogNormal(x, [0], [1], [2])", 0, 5);
  // fFitFcn->SetRange(0.0, 2.5);
  // fFitFcn->SetParameters(1.0, 0, 1);
  // fFitFcn->SetMinimum(1.0e-3);
  // fFitFcn->SetMaximum(10.0);
  // fFitFcn->SetLineColor(kRed);
  // fFitFcn->SetLineWidth(1);
  // fFitFcn->Draw();
  //
  fHslider1->SetPosition(0., 500.);
  fHslider1->SetPointerPosition(250.);
  //
  // sprintf(buf, "%.3f", fHslider1->GetMinPosition());
  // fTbh1->Clear();
  // fTbh1->AddText(0, buf);
  // sprintf(buf, "%.3f", fHslider1->GetPointerPosition());
  // fTbh2->Clear();
  // fTbh2->AddText(0, buf);
  // sprintf(buf, "%.3f", fHslider1->GetMaxPosition());
  // fTbh3->Clear();
  // fTbh3->AddText(0, buf);
}

//______________________________________________________________________________
TUserInterface::~TUserInterface()
{
  // Clean up

  Cleanup();
}

//______________________________________________________________________________
void TUserInterface::CloseWindow()
{
  // Called when window is closed via the window manager.

  delete this;
}

//______________________________________________________________________________
void TUserInterface::DoText(const char * /*text*/)
{
  // Handle text entry widgets.

  TGTextEntry *te = (TGTextEntry *)gTQSender;
  Int_t id = te->WidgetId();

  switch (id) {
    case HId1:
      fHslider1->SetPosition(atof(fTbh1->GetString()),
                             fHslider1->GetMaxPosition());
      break;
    case HId2:
      fHslider1->SetPointerPosition(atof(fTbh2->GetString()));
      break;
    case HId3:
      fHslider1->SetPosition(fHslider1->GetMinPosition(),
                             atof(fTbh1->GetString()));
      break;
    default:
      break;
  }
  fCanvas->GetCanvas()->Modified();
  fCanvas->GetCanvas()->Update();
}

void TUserInterface::HandleButtons()
{
  // Handle different buttons.

  TGButton *btn = (TGButton *)gTQSender;
  Int_t id = btn->WidgetId();

  switch (id) {
    case HCId1:
      fHslider1->SetConstrained(fCheck1->GetState());
      break;
    case HCId2:
      fHslider1->SetRelative(fCheck2->GetState());
      break;
    default:
      break;
  }
}

void TUserInterface::CanvasEvent()
{
  // This function just check the histogram ploted in the canvas
  // If the histogram is not same as fHis, replace
  // This call all events now.  I have no idea about other signal.
  std::cout << "CanvasEvent" << std::endl;
  auto list = fCanvas->GetCanvas()->GetListOfPrimitives();
  std::cout << list->GetSize() << std::endl;
  for (auto &&obj : *list) {
    if (obj->InheritsFrom("TH1")) {
      auto his = (TH1 *)fCanvas->GetCanvas()->GetPrimitive(obj->GetName());
      if (fHis == nullptr || fHis != his) {
        fHis = his;
        fHslider1->SetRange(fHis->GetXaxis()->GetBinCenter(1),
                            fHis->GetXaxis()->GetBinCenter(fHis->GetNbinsX()));
        fHslider1->SetPosition(
            fHis->GetXaxis()->GetBinCenter(1),
            fHis->GetXaxis()->GetBinCenter(fHis->GetNbinsX()));

        fMean = fHis->GetXaxis()->GetBinCenter(fHis->GetMaximumBin());
        // fHslider1->SetPointerPosition(fMean);

        fPeak = fHis->GetMaximum();
        fLeftSlider->SetRange(0, fPeak);
        fLeftSlider->SetPosition(fPeak - fLeftVal);
        fPeakSlider->SetRange(0, fPeak);
        fPeakSlider->SetPosition(0);
        fRightSlider->SetRange(0, fPeak);
        fRightSlider->SetPosition(fPeak - fRightVal);
        fSigmaSlider->SetRange(0, fHis->GetRMS() * 100);
        fSigmaSlider->SetPosition(fSigma * 100);
        fMeanSlider->SetRange(
            fHis->GetXaxis()->GetBinCenter(1) * 100,
            fHis->GetXaxis()->GetBinCenter(fHis->GetNbinsX()) * 100);
        fMeanSlider->SetPosition(fMean * 100);
      }
    }
  }

  // if (fHis) UpdateGraph();
}

void TUserInterface::DoSlider()
{
  if (fHis) {
    fHis->GetXaxis()->SetRange(
        fHis->GetXaxis()->FindBin(fHslider1->GetMinPosition() - 0.05),
        fHis->GetXaxis()->FindBin(fHslider1->GetMaxPosition()));
    UpdateGraph();
  }
}

void TUserInterface::DoPeakSlider()
{
  auto pos = fPeakSlider->GetPosition();
  auto max = fHis->GetMaximum();
  fPeak = max - pos;
  std::cout << "Peak" << std::endl;
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
  fSigma = pos / 100.;
  std::cout << "Sigma" << std::endl;
  UpdateGraph();
}

void TUserInterface::DoMeanSlider()
{
  auto pos = fMeanSlider->GetPosition();
  fMean = pos / 100.;
  std::cout << "Mean" << std::endl;
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
    fMeanSlider->SetPosition(fMean);

    fSigma = fFitFnc->GetParameter(2);
    fSigmaSlider->SetPosition(fSigma * 100);

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
  // Connect to Mongo DB
  // Upload
  auto FWHM = fSigma * 2 * sqrt(2 * log(2));
  std::cout << "Mean: " << fMean << "\n"
            << "FWHM: " << FWHM << std::endl;
}

void TUserInterface::UpdateGraph()
{
  std::cout << "hit" << std::endl;
  std::cout << fPeak << "\t" << fMean << "\t" << fSigma << std::endl;
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
  std::cout << "end" << std::endl;
}
