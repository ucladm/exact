/*
  2015-11-25 AFan
 */

#ifndef CONVERTER2_H
#define CONVERTER2_H


#include "Module.hh"
#include "EventData.hh"

class Converter2 : public Module
{
public:
  Converter2(TString name);
  void Initialize();
  void Process(EventData* event);
  void Finalize(TTree* master);


private:
  Float_t trigger_offset;
  Float_t var;
  
};



#endif
