/*
  2016-02-29 AFan

  Do FFT on waveforms.
  - Need to treat ch7 separately.
  - Can optionally save FFTs to output (do sparingly!).
  - Apply filter on FFT and then back-transform.

 */

#ifndef FFT_hh
#define FFT_hh

#include "Module.hh"
#include "EventData.hh"
#include <string>
#include <vector>

#include "TVirtualFFT.h"
#include "TMath.h"
#include "TF1.h"
#include "TH1F.h"

#define MAXNCHANS 20


class FFT : public Module
{
public:
  FFT(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);

private:

  TVirtualFFT* _fft;

  TH1F* _mag[MAXNCHANS];
  
};








#endif
