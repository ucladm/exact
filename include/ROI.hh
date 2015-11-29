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

class ROI : public Module
{
public:
  ROI(const Setting & cfg);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);


private:

  double _roi_start;
  double _roi_end;

  Float_t _roi;
  Float_t _ch_roi[NCHANS];
};

#endif
