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
      type: "monoenergetic",
      neutrino: "ve",
      energy: 15
    },
    executable_settings: {events:1000000 , output:[{file:"mono_nue_ES.root" ,format:"root" , mode:"overwrite" }] },
}


