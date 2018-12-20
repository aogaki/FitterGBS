#ifndef TUSERINTERFACE_HPP
#define TUSERINTERFACE_HPP 1

// All-in-one Class
// Have to be divided into display, fitting, and something?
// Re-write this!
#include <TBrowser.h>
#include "TCanvas.h"
#include "TF1.h"
#include "TGButton.h"
#include "TGLayout.h"
#include "TGMenu.h"
#include "TGSlider.h"
#include "TGTextEntry.h"
#include "TGTripleSlider.h"
#include "TH1.h"
#include "TMath.h"
#include "TRootEmbeddedCanvas.h"

enum class MenuMessages {
  BROWSE,
  EXIT,
};

Double_t fitFnc(Double_t *pos, Double_t *par);

class TUserInterface : public TGMainFrame
{
 public:
  TUserInterface();
  virtual ~TUserInterface();

  void CloseWindow();
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

  void HandleMenu(Int_t menuID);

  ClassDef(TUserInterface, 1);

 private:
  TRootEmbeddedCanvas *fCanvas;
  TGLayoutHints *fLHint;
  TGHorizontalFrame *fHFrame;
  TGVerticalFrame *fVFrame;

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

  Double_t fSliderFac;

  TGMenuBar *fMenuBar;
  TGPopupMenu *fMenuFile;

  TBrowser *fBrowser;
};

#endif
