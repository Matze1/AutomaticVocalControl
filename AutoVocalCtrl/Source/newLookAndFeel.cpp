/*
  ==============================================================================

    newLookAndFeel.cpp
    Created: 2 May 2018 2:03:49pm
    Author:  Nils Heine

  ==============================================================================
*/

#include "newLookAndFeel.h"

newLookAndFeel::newLookAndFeel()
{
    // Definition of basic LookAndFeel Colours
    Colour myRed = Colour(106, 15, 15);
    Colour myGreen = Colour(109, 151, 110);
    Colour myGreenDark = Colour(100,140, 100);
    
    // Set basic component colours
    setColour(Slider::textBoxOutlineColourId, myGreen);
    setColour(Slider::thumbColourId, myRed);
    setColour(ComboBox::outlineColourId, myRed);
    setColour(TextButton::buttonColourId, myRed);
    setColour(TextButton::buttonOnColourId, myGreenDark);
    setColour(TextButton::textColourOnId, myRed);
    setColour(TextButton::textColourOffId, myGreen);
}

newLookAndFeel::~newLookAndFeel()
{
    
}


// Draws the appearence of a Linear Slider
void newLookAndFeel::drawLinearSlider(Graphics& g,
                                      int x, int y, int w, int h,
                                      float sliderPos, float minSliderPos, float maxSliderPos,
                                      const Slider::SliderStyle style,
                                      Slider& slider)
{
    if (style == Slider::LinearVertical) {
        // Draws a triangle as slider thumb
        g.setColour (slider.findColour(Slider::thumbColourId));
        Path triangle;
        triangle.addTriangle(x, sliderPos - (0.5 * w) , x + w, sliderPos, x, sliderPos + (0.5 * w));
        g.fillPath(triangle);
    
    } else if (slider.getMinimum() < -55.0) {
        // Slider filled with gradient for I/O metering
        g.fillAll(Colours::black);
        g.setGradientFill(ColourGradient(Colour(170,215,170), x, y, Colour(43, 63, 43), x, y + h, false));
        g.fillRect(x,(int)sliderPos,w,(int)(h - sliderPos + 2));
    
    } else {
        // Multidirectional slider
        g.fillAll(Colours::black);
        const bool which = (slider.getMinimum() < -9.9);
        Colour c = which ? Colour(109, 151, 110):slider.findColour(Slider::thumbColourId);
        g.setColour(c);
        // Slider Bar must not be drawn with negative values
        if (sliderPos < h/2)
            g.fillRect(x,(int)sliderPos,w,(int)(h/2 - sliderPos + 2));
        else
            g.fillRect(x,y + h/2,w,(int)(-h/2 + sliderPos));
        if (!which) {
            g.fillRect(x, y + h/2, w, 1.0);
        }
    }
}

// Removes the default slider background
void newLookAndFeel::drawLinearSliderBackground(Graphics& g,
                                                int x, int y, int w, int h,
                                                float sliderPo, float minSliderPos, float maxSliderPos,
                                                const Slider::SliderStyle style, Slider& slider)
{
    // Draw nothing
}

// Draws the appearence of a Button
void newLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                           bool isMouseOverButton, bool isButtonDown)
{
    // remove round corners
    auto cornerSize = 0.0f;
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);
    
    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
    .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);
    
    if (isButtonDown || isMouseOverButton)
        baseColour = baseColour.contrasting (isButtonDown ? 0.2f : 0.05f);
    
    g.setColour (baseColour);
    
    if (button.isConnectedOnLeft() || button.isConnectedOnRight())
    {
        Path path;
        path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                  bounds.getWidth(), bounds.getHeight(),
                                  cornerSize, cornerSize,
                                  ! button.isConnectedOnLeft(),
                                  ! button.isConnectedOnRight(),
                                  ! button.isConnectedOnLeft(),
                                  ! button.isConnectedOnRight());
        
        g.fillPath (path);
        
        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.strokePath (path, PathStrokeType (1.0f));
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornerSize);
        
        g.setColour (button.findColour (ComboBox::outlineColourId));
        g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
    }
}
