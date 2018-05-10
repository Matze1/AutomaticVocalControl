/*
  ==============================================================================

    newLookAndFeel.h
    Created: 2 May 2018 2:03:14pm
    Author:  Nils Heine

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class newLookAndFeel : public LookAndFeel_V4
{
public:
    newLookAndFeel();
    ~newLookAndFeel();
    
    void drawLinearSlider(Graphics& g,
                          int x, int y, int w, int h,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const Slider::SliderStyle style, Slider& slider) override;
    
    void drawLinearSliderBackground(Graphics& g,
                                    int x, int y, int width, int height,
                                    float sliderPos, float minSliderPos,float maxSliderPos,
                                    const Slider::SliderStyle style, Slider& slider) override;
    
    void drawButtonBackground (Graphics& g,
                               Button& button, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;
    
};
