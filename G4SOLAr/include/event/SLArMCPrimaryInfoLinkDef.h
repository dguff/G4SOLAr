/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArMCPrimaryInfoLinkDef
 * @created     : venerdì feb 14, 2020 17:28:47 CET
 */


#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ struct trj_point+;
#pragma link C++ class std::vector<trj_point>+;
#pragma link C++ class SLArEventTrajectory+;
#pragma link C++ class std::vector<std::unique_ptr<SLArEventTrajectory>>+;
#pragma link C++ class std::vector<SLArEventTrajectory*>+;
#pragma link C++ class SLArMCPrimaryInfo<SLArEventTrajectory*>+;
#pragma link C++ class SLArMCPrimaryInfo<std::unique_ptr<SLArEventTrajectory>>+;
#pragma link C++ typedef SLArMCPrimaryInfoPtr+;
#pragma link C++ typedef SLArMCPrimaryInfoUniquePtr+;
#endif

