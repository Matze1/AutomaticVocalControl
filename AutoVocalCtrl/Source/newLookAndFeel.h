/*
  ==============================================================================

    newLookAndFeel.h
    Created: 2 May 2018 2:03:14pm
    Author:  Nils Matze Heine

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class newLookAndFeel : public LookAndFeel_V4
{
public:
    newLookAndFeel();
    ~newLookAndFeel();
    
    void drawLinearSliderThumb (Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider& slider) override;
    
};
