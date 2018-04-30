// configure the test here
var TestConfig = {
  "TestName": "Side Chain Advantage Test",
  "RateScalePng": "img/scale_abs.png",
  "RateScaleBgPng": "img/scale_abs_background.png",
  "RateMinValue": 0,
  "RateMaxValue": 100,
  "RateDefaultValue":50,
  "ShowFileIDs": false,
  "ShowResults": false,
  "LoopByDefault": true,
  "EnableABLoop": true,
  "EnableOnlineSubmission": false,
  "BeaqleServiceURL": "",
  "SupervisorContact": "",
  "RandomizeTestOrder": false,
  "MaxTestsPerRun": -1,
  "RequireMaxRating": false,
  "Testsets": [
    //
   {
       "Name": "Lautstärkeverhältnis Leise 1",
       "TestID": "l1_leise",
       "Files": {
           "Reference": "audio/1_leise_oc.wav",
           "1": "audio/1_leise_anchor.wav",
           "2": "audio/1_leise_sc.wav",
           "3": "audio/1_leise_leise.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Laut 1",
       "TestID": "l1_laut",
       "Files": {
           "Reference": "audio/1_laut_oc.wav",
           "1": "audio/1_laut_anchor.wav",
           "2": "audio/1_laut_leise.wav",
           "3": "audio/1_laut_sc.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Break 1",
       "TestID": "l1_break",
       "Files": {
           "Reference": "audio/1_break_oc.wav",
           "1": "audio/1_break_anchor.wav",
           "2": "audio/1_break_leise.wav",
           "3": "audio/1_break_sc.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Übergang 1",
       "TestID": "l1_uebergang",
       "Files": {
           "Reference": "audio/1_uebergang_oc.wav",
           "1": "audio/1_uebergang_anchor.wav",
           "2": "audio/1_uebergang_sc.wav",
           "3": "audio/1_uebergang_leise.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Leise 2",
       "TestID": "l2_leise",
       "Files": {
           "Reference": "audio/2_leise_oc.wav",
           "1": "audio/2_leise_laut.wav",
           "2": "audio/2_leise_anchor.wav",
           "3": "audio/2_leise_sc.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Laut/Break 2",
       "TestID": "l2_laut_break",
       "Files": {
           "Reference": "audio/2_laut_break_oc.wav",
           "1": "audio/2_laut_break_sc.wav",
           "2": "audio/2_laut_break_laut.wav",
           "3": "audio/2_laut_break_anchor.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Übergang 2",
       "TestID": "l2_uebergang",
       "Files": {
           "Reference": "audio/2_uebergang_oc.wav",
           "1": "audio/2_uebergang_sc.wav",
           "2": "audio/2_uebergang_laut.wav",
           "3": "audio/2_uebergang_anchor.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Übergang 3",
       "TestID": "l3_uebergang",
       "Files": {
           "Reference": "audio/3_uebergang_oc.wav",
           "1": "audio/3_uebergang_sc.wav",
           "2": "audio/3_uebergang_anchor.wav",
           "3": "audio/3_uebergang_laut.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis Laut 3",
       "TestID": "l3_laut",
       "Files": {
           "Reference": "audio/3_laut_oc.wav",
           "1": "audio/3_laut_sc.wav",
           "2": "audio/3_laut_anchor.wav",
           "3": "audio/3_laut_laut.wav",
       }
   },
   {
       "Name": "Lautstärkeverhältnis 3",
       "TestID": "l3_leise",
       "Files": {
           "Reference": "audio/3_leise_oc.wav",
           "1": "audio/3_leise_sc.wav",
           "2": "audio/3_leise_anchor.wav",
           "3": "audio/3_leise_laut.wav",
       }
   },
   {
       "Name": "Klarheit Leise 1",
       "TestID": "vk1_leise",
       "Files": {
           "Reference": "audio/1_leise_oc.wav",
           "1": "audio/1_leise_anchor.wav",
           "2": "audio/1_leise_sc.wav",
           "3": "audio/1_leise_leise.wav",
       }
   },
   {
       "Name": "Klarheit Break 1",
       "TestID": "vk1_break",
       "Files": {
           "Reference": "audio/1_break_oc.wav",
           "1": "audio/1_break_anchor.wav",
           "2": "audio/1_break_leise.wav",
           "3": "audio/1_break_sc.wav",
       }
   },
   {
       "Name": "Klarheit Laut/Break 2",
       "TestID": "vk2_laut_break",
       "Files": {
           "Reference": "audio/2_laut_break_oc.wav",
           "1": "audio/2_laut_break_sc.wav",
           "2": "audio/2_laut_break_laut.wav",
           "3": "audio/2_laut_break_anchor.wav",
       }
   },
   {
       "Name": "Klarheit Übergang 3",
       "TestID": "vk3_uebergang",
       "Files": {
           "Reference": "audio/3_uebergang_oc.wav",
           "1": "audio/3_uebergang_sc.wav",
           "2": "audio/3_uebergang_anchor.wav",
           "3": "audio/3_uebergang_laut.wav",
       }
   },
  ]
}
