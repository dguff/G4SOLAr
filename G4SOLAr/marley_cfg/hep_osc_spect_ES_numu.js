/**
 * @class       : executable
 * @created     : Tuesday Apr 12, 2022 09:47:05 CEST
 * @description : executable
 */

{
  seed: 12345,
  direction: {x:0.0 , y:0.0 , z:1.0},
  target: {nuclides:[1000180400] , atom_fractions:[1.0]},
  reactions: ["ES.react"],
  source: {
     type: "tgraph",
     neutrino: "vu",
     tfile: "hep_oscillated_file.root",  // Name of the ROOT file containing

                                  // the TGraph object
  
     namecycle: "hep_numu",        // Name of the TGraph object (used to
                                  // retrieve it from the ROOT file)
    },
    executable_settings: {events:5000000 , output:[{file:"events_osc_hep_numu_ES.root" ,format:"root" , mode:"overwrite" }] },
}


