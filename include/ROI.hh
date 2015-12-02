/*

  2015-06-02 AFan

  v0.2 2015-11-29

 */

#ifndef ROI_hh
#define ROI_hh

#include "Module.hh"
#include "EventData.hh"
#include <string>
#include <vector>
#include <utility> //pair

#define MAXNROI 10

class ROI : public Module
{
public:
  ROI(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);


private:

  int _nroi;
  std::vector< std::vector<float> > _roi_times;

  Float_t _roi[MAXNROI];
  Float_t _ch_roi[NCHANS*MAXNROI];
};

#endif
