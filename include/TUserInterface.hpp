#ifndef TUSERINTERFACE_HPP
#define TUSERINTERFACE_HPP 1

#include "TCanvas.h"
#include "TF1.h"
#include "TGButton.h"
#include "TGLayout.h"
#include "TGTextEntry.h"
#include "TGTripleSlider.h"
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

class TUserInterface : public TGMainFrame
{
 private:
  TRootEmbeddedCanvas *fCanvas;
  TGLayoutHints *fLcan;
  TF1 *fFitFcn;
  TGHorizontalFrame *fHframe0, *fHframe1, *fHframe2;
  TGLayoutHints *fBly, *fBfly1, *fBfly2, *fBfly3;
  TGTripleHSlider *fHslider1;
  TGTextEntry *fTeh1, *fTeh2, *fTeh3;
  TGTextBuffer *fTbh1, *fTbh2, *fTbh3;
  TGCheckButton *fCheck1, *fCheck2;

 public:
  TUserInterface();
  virtual ~TUserInterface();

  void CloseWindow();
  void DoText(const char *text);
  void DoSlider();
  void HandleButtons();

  ClassDef(TUserInterface, 1);
};

#endif
