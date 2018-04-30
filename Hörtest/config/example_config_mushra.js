// configure the test here
var TestConfig = {
  "TestName": "Mushra Demo Test",
  "RateScalePng": "img/scale_abs.png",
  "RateScaleBgPng": "img/scale_abs_background.png",
  "RateMinValue": 0,
  "RateMaxValue": 100,
  "RateDefaultValue":0,
  "ShowFileIDs": false,
  "ShowResults": false,
  "LoopByDefault": true,
  "EnableABLoop": true,
  "EnableOnlineSubmission": false,
  "BeaqleServiceURL": "",
  "SupervisorContact": "",
  "RandomizeTestOrder": false,
  "MaxTestsPerRun": 3,
  "RequireMaxRating": false,
  "Testsets": [
    //    
    {
      "Name": "Schubert 1",
      "TestID": "id1_1",
      "Files": {
            "Reference": "audio/1_leise_oc.wav",
            "1": "audio/1_leise_anchor.wav",
            "2": "audio/1_leise_sc.wav",
        }
    },
  ]
}
