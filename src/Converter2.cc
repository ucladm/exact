#include "Converter2.hh"

#include "cfg.h"

#include <iostream>

using namespace std;
//using namespace libconfig;

Converter2::Converter2(const Setting & cfg) : Module(cfg)
{
  cfg.lookupValue("trigger_offset", trigger_offset);
}

void Converter2::Initialize()
{
  Module::Initialize();
  Module::tree->Branch("var", &var, "var/F");
}

void Converter2::Process(EventData* event)
{
  var = 3.14159;
  cout << "Hello. "<<module_name<<" "<<trigger_offset<<endl;


  // This must be the last call within this function.
  Module::Process();
}

void Converter2::Finalize(TTree* master)
{
  Module::Finalize(master);
}
