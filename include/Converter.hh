/*

  Defines the Converter class module.
  Read a raw DAQ file, fill identifier info in EventData object

  v0.1 AFan  2013-08-17
  

 */

#ifndef Converter_hh
#define Converter_hh

#include "EventData.hh"
#include "CfgReader.hh"
#include "DAQheader.hh"
#include <string>

class Converter
{
public:
  Converter();
  void Initialize(CfgReader cfg);
  int Process(EventData* event, DAQheader & DAQ_header);


  std::string module_name;
  bool enabled;
  
};

#endif
