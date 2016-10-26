#include "HLTmatches.h"

HLTmatches::HLTmatches(std::vector<std::string> _HLTlist)
{
  for (i=0; i<_HLTlist.size(); i++) {
    HLTnames.push_back(_HLTlist[i].substr(0, _HLTlist[i].find_first_of("*")));
  }
#ifdef DEBUG
  for (i=0; i<HLTnames.size(); i++) {
    std::cout << i << " ==> " << HLTnames[i] << std::endl;
  }
#endif
}

HLTmatches::~HLTmatches()
{
}

Int_t
HLTmatches::TriggerNum(std::string _trigName)
{
  for (i=0; i<HLTnames.size(); i++) {
    if (_trigName.find(HLTnames[i])!=std::string::npos) return i;
  }
  return -1;
}
