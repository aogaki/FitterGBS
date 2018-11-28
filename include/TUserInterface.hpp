#ifndef TUSERINTERFACE_HPP
#define TUSERINTERFACE_HPP 1

// All-in-one Class
// Have to be divided into display, fitting, and something?
// Re-write this!

#include "TCanvas.h"
#include "TF1.h"
#include "TGButton.h"
#include "TGLayout.h"
#include "TGSlider.h"
#include "TGTextEntry.h"
#include "TGTripleSlider.h"
#include "TH1.h"
#include "TMath.h"
#include "TRootEmbeddedCanvas.h"

enum ETestCommandIdentifiers {
  HId1,
  HId2,
  HId3,
  HCId1,
  HCId2,

  HSId1
};

Double_t fitFnc(Double_t *pos, Double_t *par);

class TUserInterface : public TGMainFrame
{
 public:
  TUserInterface();
  virtual ~TUserInterface();

  void CloseWindow();
  void DoText(const char *text);
  void DoSlider();
  void HandleButtons();

  void CanvasEvent();

  void DoPeakSlider();
  void DoLeftSlider();
  void DoRightSlider();
  void DoSigmaSlider();
  void DoMeanSlider();

  void UpdateGraph();

  void DoFit();
  void DoUpload();

  ClassDef(TUserInterface, 1);

 private:
  TRootEmbeddedCanvas *fCanvas;
  TGTripleHSlider *fHslider1;
  TGLayoutHints *fLcan;
  TGHorizontalFrame *fHframe0, *fHframe1, *fHframe2;
  TGLayoutHints *fBly, *fBfly1, *fBfly2, *fBfly3;
  TGTextEntry *fTeh1, *fTeh2, *fTeh3;
  TGTextBuffer *fTbh1, *fTbh2, *fTbh3;
  TGCheckButton *fCheck1, *fCheck2;

  TGVSlider *fLeftSlider;
  TGVSlider *fRightSlider;
  TGVSlider *fPeakSlider;
  TGHSlider *fSigmaSlider;
  TGHSlider *fMeanSlider;

  TGTextButton *fFitButton;
  TGTextButton *fUploadButton;

  TH1 *fHis;
  TF1 *fGaussian;
  TF1 *fBackground;
  TF1 *fFitFnc;

  Double_t fPeak;
  Double_t fSigma;
  Double_t fMean;
  Double_t fLeftVal;
  Double_t fRightVal;
};

#endif
