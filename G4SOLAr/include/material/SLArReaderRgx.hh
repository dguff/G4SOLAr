/**
 * @author      : guff (guff@guff-gssi)
 * @file        : SLArReaderRgx
 * @created     : giovedì giu 20, 2019 20:44:32 CEST
 */

#ifndef SLArREADERRGX_HH

#define SLArREADERRGX_HH

#include "TPRegexp.h"

TPRegexp rgxNum("\\s*\\d+(\\.\\d*)?([eE][+-]?\\d+)?");
TPRegexp rgxPar("\\w+");
TPRegexp rgxNist("G4_\\w+");



#endif /* end of include guard SLArREADERRGX_HH */

