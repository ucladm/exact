#include "Module.hh"

Module::Module(const Setting & cfg)
{
  cfg.lookupValue("name", module_name);
  enabled = cfg.lookupValue("enabled", enabled) ? enabled : true;
}

Module::~Module()
{}

void Module::Initialize()
{
  // This function is to be overridden by each module.
  // Within override, call Module::Initialize().
  tree = new TTree(module_name.c_str(), module_name.c_str());
}


void Module::Process()
{
  // This function is to be overridden by each module.
  // Within override, call Module::Process() as FINAL step;
  if (enabled) tree->Fill();
}


void Module::Finalize(TTree* master)
{
  // This function is to be overridden by each module.
  // Within override, call Module::Finalize().
  if (enabled) {
    if (tree->GetEntries() == master->GetEntries()) {
      master->AddFriend(tree);
      tree->Write();
    }
    else
      std::cout << "ERROR: tree for "<<module_name<<" has mis-matching number "
                << "of entries! This is a serious error."<<std::endl;
  }
}


TTree* Module::GetTree()
{
  return tree;
}
