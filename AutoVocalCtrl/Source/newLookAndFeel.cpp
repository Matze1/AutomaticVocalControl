/*
  ==============================================================================

    newLookAndFeel.cpp
    Created: 2 May 2018 2:03:49pm
    Author:  Nils Matze Heine

  ==============================================================================
*/

#include "newLookAndFeel.h"

newLookAndFeel::newLookAndFeel()
{
//    setColour(Slider::backgroundColourId, Colours::red);
//    setColour(Slider::thumbColourId, Colours::red);
//    setColour(Slider::textBoxOutlineColourId, Colours::red);
}

newLookAndFeel::~newLookAndFeel()
{
    
}

void newLookAndFeel::drawLinearSliderThumb (Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle, Slider& slider)
{
    g.setColour(Colours::red);
    Path triangle;
    triangle.addTriangle(x, y, x + width, y + height / 2, x, y + height);
    g.fillPath(triangle);
}
