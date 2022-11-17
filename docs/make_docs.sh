#!/usr/bin/env bash

######################################################################
# @author      : Daniele Guffanti (daniele.guffanti@mib.infn.it)
# @file        : make_docs.sh
# @created     : Tue Nov 15, 2022 16:11:27 CET
#
# @description : Build G4SOLAr documentation
######################################################################

# copy awesome doxygen style
git submodule add https://github.com/jothepro/doxygen-awesome-css.git ./ext

# call doxygen executable
doxygen ./g4solar-doc.doxy

