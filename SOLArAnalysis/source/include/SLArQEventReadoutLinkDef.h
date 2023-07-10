/**
 * @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
 * @file        : SLArQReadoutLinkDef.h
 * @created     : Mon Jun 27, 2022 13:47:38 CEST
 */

#ifdef __MAKECINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace slarq+;
#pragma link C++ class slarq::cluster_point+;
#pragma link C++ class std::vector<slarq::cluster_point>+;
#pragma link C++ class slarq::SLArQCluster+; 
#pragma link C++ class std::vector<slarq::SLArQCluster*>+;
#pragma link C++ struct slarq::slarq_axis+; 
#pragma link C++ class std::vector<slarq::slarq_axis>+; 
#pragma link C++ class slarq::SLArQEventReadout+;
#endif

