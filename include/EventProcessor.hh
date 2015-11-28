
#ifndef EventProcessor_hh
#define EventProcessor_hh

//#include "Module.hh"
#include "LVDAQHeader.hh"
#include "EventData.hh"
#include "Converter.hh"
#include "BaselineFinder.hh"
#include "ZeroSuppressor.hh"
#include "Integrator.hh"
#include "SumChannel.hh"
#include "PulseFinder.hh"
#include "ProcessedPlotter.hh"
//#include "ROI.hh"
//#include "AverageWaveforms.hh"

#include <string>
#include <libconfig.h++>

class EventProcessor
{
public:
  EventProcessor(const Config & cfg);
  
  void Initialize();
  void ProcessEvent(int event_id);
  void Finalize();

  void SetDataFile(std::string datafile);
  void CloseDataFile();

  LVDAQHeader const& GetDAQHeader() { return daqHeader; }
  
private:

  EventData* event;
  
  LVDAQHeader daqHeader;
  Converter converter;
  BaselineFinder baselineFinder;
  ZeroSuppressor zeroSuppressor;
  SumChannel sumChannel;
  Integrator integrator;
  PulseFinder pulseFinder;
  ProcessedPlotter plotter;

  

};

#endif
