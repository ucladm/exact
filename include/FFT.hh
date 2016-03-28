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
  TVirtualFFT* _fft_back;

  TH1F* _re[MAXNCHANS];
  TH1F* _im[MAXNCHANS];
  TH1F* _mag[MAXNCHANS];

  // Noise spectrum for subtraction
  //std::string _noise_spectra_filename;
  //TFile* _noise_spectra_file;
  //TH1F* _noise_spectra[MAXNCHANS];

  //double _subtractor;
  
};








#endif
