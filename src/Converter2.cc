#include "Converter2.hh"

#include "cfg.h"
#include <libconfig.h++>

#include <iostream>

using namespace std;

Converter2::Converter2(TString name)
: Module(name)
  , trigger_offset(cfg::trigger_offset)
  , var(-1)
{ }

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
