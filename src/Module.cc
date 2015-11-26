#include "Module.hh"

Module::Module(TString name)
{
  module_name = name;
}

Module::~Module()
{}

void Module::Initialize()
{
  // This function is to be overridden by each module.
  // Within override, call Module::Initialize().
  tree = new TTree(module_name, module_name);
  return;
}


void Module::Process()
{
  // This function is to be overridden by each module.
  // Within override, call Module::Process() as FINAL step;
  tree->Fill();
  return;
}


void Module::Finalize(TTree* master)
{
  // This function is to be overridden by each module.
  // Within override, call Module::Finalize().
  if (tree->GetEntries() == master->GetEntries())
    tree->Write();
  else
    std::cout << "ERROR: tree for "<<module_name<<" has mis-matching number "
              << "of entries! This is a serious error."<<std::endl;
}


TTree* Module::GetTree()
{
  return tree;
}
