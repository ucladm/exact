#include "FFT.hh"
#include "ChannelData.hh"

#include "TFile.h"
#include "TDirectory.h"
#include "TMath.h"
#include "TString.h"
#include "TComplex.h"

using namespace std;

FFT::FFT(const Setting & cfg)
  : Module(cfg)
  , _fft(NULL)
  , _fft_back(NULL)
    //, _noise_spectra_file(NULL)
{
  //cfg.lookupValue("noise_spectra_filename", _noise_spectra_filename);
  //cfg.lookupValue("subtractor", _subtractor);
  
  for (int i=0; i<MAXNCHANS; ++i) {
    _re[i] = NULL;
    _im[i] = NULL;
    _mag[i] = NULL;
    //_noise_spectra[i] = NULL;
  }
}


void FFT::Initialize()
{
  Module::Initialize();

  // create TTree branches for outputs

  //TFile* current = TFile::CurrentFile();
  
  // Open noise spectra file
  //_noise_spectra_file = new TFile(_noise_spectra_filename.c_str());
  //for (int i=0; i<8; ++i) {
  //  TString path = TString::Format("FFT/hMag_ch%d", i);
  //  _noise_spectra[i] = (TH1F*) _noise_spectra_file->Get(path);
  //}
  
  //current->cd();
}


void FFT::Process(EventData * event)
{
  // reset tree variables

  for (int ch=0; ch<event->nchans; ++ch) {

    //ChannelData* channel = event->GetChannel(ch);
    
    const std::vector<double> & wfm = event->GetChannel(ch)->raw_waveform;

    int npoints = wfm.size();
    //const double xmin = event->SampleToTime(0);
    //const double xmax = event->SampleToTime(npoints-1);
    //const double dx = (xmax-xmin)/npoints;

    // initialize FFT; only need to do once during a job
    if (!_fft) {
      cout << "Constructing first FFT takes a while..."<<endl;
      _fft = TVirtualFFT::FFT(1, &npoints, "R2C EX K");
      if (!_fft) cout << "ERROR: FFT: _fft not set."<<endl;
    }

    _fft->SetPoints(&wfm[0]);
    _fft->Transform();

  
    double* re = new double[npoints];
    double* im = new double[npoints];
    _fft->GetPointsComplex(re,im);


    // rescale FFT by npoints
    for (int i=0; i<npoints; ++i) {
      re[i] /= npoints;
      im[i] /= npoints;
    }


    // generate magnitude array
    //double* mag = new double[npoints];
    //for (int i=0; i<npoints; ++i) mag[i] = TMath::Sqrt(re[i]*re[i]+im[i]*im[i]);
    TComplex* c = new TComplex[npoints];
    for (int i=0; i<npoints; ++i) c[i] = TComplex(re[i], im[i]);
    double* mag = new double[npoints];
    double* ph = new double[npoints];
    for (int i=0; i<npoints; ++i) { mag[i] = c[i].Rho(); ph[i] = c[i].Theta(); }

    
    // create frequency axis
    const int fnpoints = npoints/2+1;
    double* faxis = new double[fnpoints];
    for (int i=0; i<fnpoints; ++i) faxis[i] = i/(npoints*event->us_per_samp*1.e-6);

    // Instantiate histogram using first event
    if (event->event_id == 0) {
      TString suffix = TString::Format("ch%d", ch);
      _re[ch] = new TH1F("hRe_"+suffix, "", fnpoints-1, faxis);
      _im[ch] = new TH1F("hIm_"+suffix, "", fnpoints-1, faxis);
      _mag[ch] = new TH1F("hMag_"+suffix, "", fnpoints-1, faxis);
    }

    // Build average power spectrum
    TH1F* hRe = new TH1F("hRe", "", fnpoints-1, faxis);
    TH1F* hIm = new TH1F("hIm", "", fnpoints-1, faxis);
    TH1F* hMag = new TH1F("hMag", "", fnpoints-1, faxis);
    for (int i=0; i<fnpoints; ++i) {
      hRe->SetBinContent(i+1, re[i]);
      hIm->SetBinContent(i+1, im[i]);
      hMag->SetBinContent(i+1, mag[i]);
    }
    _re[ch]->Add(hRe);
    _im[ch]->Add(hIm);
    _mag[ch]->Add(hMag);

    // Possible step to be implemented: Apply filters and transform back
    double* re_filt = new double[npoints];
    double* im_filt = new double[npoints];
    double* mag_filt = new double[npoints];
    for (int i=0; i<npoints; ++i) { re_filt[i] = re[i]; im_filt[i] = im[i]; mag_filt[i] = mag[i]; }
    const int fstart = 1;
    for (int i=fstart; i<fnpoints; ++i) {
      //mag_filt[i] -= _subtractor*_noise_spectra[ch]->GetBinContent(i+1);
      if (mag_filt[i] < 0) mag_filt[i] *= -1;
      re_filt[i] = mag_filt[i] * TMath::Cos(ph[i]);
      im_filt[i] = mag_filt[i] * TMath::Sin(ph[i]);
    }

    
    if (!_fft_back) {
      _fft_back = TVirtualFFT::FFT(1, &npoints, "C2R EX K");
      if (!_fft_back) cout << "ERROR: FFT: _fft_back not set."<<endl;
    }

    _fft_back->SetPointsComplex(re_filt, im_filt);
    _fft_back->Transform();
    double* back = _fft_back->GetPointsReal();
    
    std::vector<double> & filtered_wfm = event->GetChannel(ch)->filtered_waveform;
    filtered_wfm.clear();
    for (int i=0; i<npoints; ++i) filtered_wfm.push_back(back[i]);

    delete hRe;
    delete hIm;
    delete hMag;
    
  }


  
  
  Module::Process();
}


void FFT::Finalize(TTree* master)
{

  // scale average FFTs by nevents
  const int nevents = master->GetEntries();
  for (int i=0; i<MAXNCHANS; ++i) {
    if (_re[i]) _re[i]->Scale(1./nevents);
    if (_im[i]) _im[i]->Scale(1./nevents);
    if (_mag[i]) _mag[i]->Scale(1./nevents);
  }


  
  // Save average power spectra to output file
  gDirectory->mkdir("FFT")->cd();
  for (int i=0; i<MAXNCHANS; ++i) {
    if (_re[i]) _re[i]->Write();
    if (_im[i]) _im[i]->Write();
    if (_mag[i]) _mag[i]->Write();
  }
  gDirectory->cd("/");

  delete _fft;
  delete _fft_back;

  Module::Finalize(master);

  //TFile* current = TFile::CurrentFile();
  //_noise_spectra_file->Close();
  //current->cd();
}
