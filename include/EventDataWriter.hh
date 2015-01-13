/*

  Writes the EventData file to file.

  v0.1 AFan 2014-01-12


 */


#ifndef EventDataWriter_hh
#define EventDataWriter_hh

#include "EventData.hh"
#include "CfgReader.hh"

#include <string>

#include "TTree.h"
#include "TFile.h"

#endif


class EventDataWriter
{
public:
  EventDataWriter(CfgReader const& cfg);
  void Initialize(TFile* f);
  int Process(EventData* event);
  void Finalize();

  std::string module_name;
  
private:
  
  bool _enabled;

  TFile* _outfile;
  EventData* _event;
  TTree* _tree;
};


