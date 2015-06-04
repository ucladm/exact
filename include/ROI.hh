/*

  2015-06-02 AFan

  

 */

#ifndef ROI_hh
#define ROI_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include <string>
#include <vector>

class ROI
{
public:
  ROI(CfgReader const& cfg);
  int Process(EventData* event);

  std::string module_name;


private:
  bool _enabled;

  double _roi_start;
  double _roi_end;


};

#endif
