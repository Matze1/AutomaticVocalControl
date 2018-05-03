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

void newLookAndFeel::drawLinearSlider(Graphics& g,
                                      int x, int y, int w, int h,
                                      float sliderPos, float minSliderPos, float maxSliderPos,
                                      const Slider::SliderStyle style,
                                      Slider& slider)
{
    if (style == Slider::LinearVertical) {
        g.setColour (slider.findColour (Slider::thumbColourId));
        Path triangle;
        triangle.addTriangle(x, sliderPos - (0.5 * w) , x + w, sliderPos, x, sliderPos + (0.5 * w));
        g.fillPath(triangle);
    } else if (slider.getMinimum() < -55.0) {
        g.fillAll(Colours::black);
        g.setGradientFill(ColourGradient(Colours::skyblue, x, y, Colour(11, 52, 56), x, y + h, false));
//        g.setColour(Colour(0.5f, 0.8f, 1-sliderPos/h, 1.0f));
        g.fillRect(x, y + h, w, (int)(-h + sliderPos - 1));
    } else {
        g.fillAll(Colours::black);
        g.setColour(Colours::skyblue);
        g.fillRect(x, y + h/2, w, (int)(-h/2 + sliderPos - 1));
    }
}

void newLookAndFeel::drawLinearSliderBackground(Graphics& g,
                                                int x, int y, int w, int h,
                                                float sliderPo, float minSliderPos, float maxSliderPos,
                                                const Slider::SliderStyle style, Slider& slider)
{
    // DRAW NOTHING
}
