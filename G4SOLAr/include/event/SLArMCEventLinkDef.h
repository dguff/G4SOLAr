#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::vector<std::unique_ptr<SLArMCPrimaryInfoUniquePtr>>+;
#pragma link C++ class std::vector<SLArMCPrimaryInfoPtr*>+;
#pragma link C++ class std::map<int, std::unique_ptr<SLArEventSuperCellArrayUniquePtr>>++;
#pragma link C++ class std::map<int, SLArEventSuperCellArrayPtr*>++;
#pragma link C++ class std::map<int, std::unique_ptr<SLArEventAnodeUniquePtr>>++;
#pragma link C++ class std::map<int, SLArEventAnodePtr*>++;
#pragma link C++ class SLArMCEvent<SLArMCPrimaryInfoPtr*, SLArEventAnodePtr*, SLArEventSuperCellArrayPtr*>+;
#pragma link C++ class SLArMCEvent<std::unique_ptr<SLArMCPrimaryInfoUniquePtr>, std::unique_ptr<SLArEventAnodeUniquePtr>, std::unique_ptr<SLArEventSuperCellArrayUniquePtr>>+;
#pragma link C++ typedef SLArMCEventPtr+;
#pragma link C++ typedef SLArMCEventUniquePtr+;
#endif

