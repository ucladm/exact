#include "FFT.hh"
#include "ChannelData.hh"

#include "TDirectory.h"
#include "TMath.h"

using namespace std;

FFT::FFT(const Setting & cfg)
  : Module(cfg)
  , _fft(NULL)
  , _fft_back(NULL)
{
  for (int i=0; i<MAXNCHANS; ++i) _mag[i] = NULL;
}


void FFT::Initialize()
{
  Module::Initialize();

  // create TTree branches for outputs
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
    double* mag = new double[npoints];
    for (int i=0; i<npoints; ++i) mag[i] = TMath::Sqrt(re[i]*re[i]+im[i]*im[i]);

    // create frequency axis
    const int fnpoints = npoints/2+1;
    double* faxis = new double[fnpoints];
    for (int i=0; i<fnpoints; ++i) faxis[i] = i/(npoints*event->us_per_samp*1.e-6);

    // Instantiate histogram using first event
    if (event->event_id == 0) {
      TString suffix = TString::Format("ch%d", ch);
      _mag[ch] = new TH1F("hMag_"+suffix, "", fnpoints-1, faxis);
    }

    // Build average power spectrum
    TH1F* hMag = new TH1F("hMag", "", fnpoints-1, faxis);
    for (int i=0; i<fnpoints; ++i) { hMag->SetBinContent(i+1, mag[i]); }
    _mag[ch]->Add(hMag);

    // Possible step to be implemented: Apply filters and transform back
    double* re_filt = new double[npoints];
    double* im_filt = new double[npoints];
    for (int i=0; i<npoints; ++i) { re_filt[i] = re[i]; im_filt[i] = im[i]; }
    //const int fstart = 50;
    //for (int i=fstart; i<fnpoints; ++i) {
    //  re_filt[i] *= TMath::Gaus(-(i-fstart), 0, 10) / TMath::Gaus(0, 0, 10);
    //  im_filt[i] *= TMath::Gaus(-(i-fstart), 0, 10) / TMath::Gaus(0, 0, 10);
    //}

    
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
    
    
    delete hMag;
  }


  
  
  Module::Process();
}


void FFT::Finalize(TTree* master)
{
  // Save average power spectra to output file
  gDirectory->mkdir("FFT")->cd();
  for (int i=0; i<MAXNCHANS; ++i) {
    if (_mag[i]) _mag[i]->Write();
  }
  gDirectory->cd("/");

  Module::Finalize(master);

  delete _fft;
  delete _fft_back;
}
