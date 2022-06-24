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
      neutrino: "ve",
      tfile: "b8_oscillated_file.root",  // Name of the ROOT file containing
                                         // the TGraph object

      namecycle: "b8_nue",              // Name of the TGraph object (used to
                                        // retrieve it from the ROOT file)
    },
    executable_settings: {events:5000000 , output:[{file:"events_osc_b8_nue_ES.root" ,format:"root" , mode:"overwrite" }] },
}


