/*
  ==============================================================================

    ADSRDisplay.cpp
    Created: 12 Jan 2024 10:46:51am
    Author:  liamw

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRDisplay.h"
#include "UIConfig.h"

//==============================================================================
ADSRDisplay::ADSRDisplay(const DataModel& dm)
    :mDataModel(dm)
{
    mDataModel.addListener(*this);

}

ADSRDisplay::~ADSRDisplay()
{
}

void ADSRDisplay::paint (juce::Graphics& g)
{
    g.fillAll(AppColors::backgroundColour);

    if (mActiveSample != nullptr) 
    {
        g.setColour(AppColors::accentColour);
       //g.drawRect(getLocalBounds(), 1.0);


        //create path with points
        juce::Path adsrPath;

        for (int i = 0; i < ADSRPoints.size() - 1; ++i)
        {
            juce::Point<float> start = ADSRPoints[i];
            juce::Point<float> end = ADSRPoints[i + 1];

            if (i == 0)
            {
                adsrPath.startNewSubPath(start);
            }

            adsrPath.lineTo(end);

        }

        juce::Path roundedPath = adsrPath.createPathWithRoundedCorners(15.0f);
        g.setColour(AppColors::accentColour);
        g.strokePath(roundedPath, juce::PathStrokeType(2.0f, juce::PathStrokeType::beveled, juce::PathStrokeType::EndCapStyle::rounded));

        //Draw Points
        float pointSize = 6.0f;
        float correction = pointSize / 2;
        g.setColour(AppColors::yellow);
 
        for (int i = 1; i <= 2; ++i)
        {
            juce::Point<float> startPoint = ADSRPoints[i];
            juce::Point<float> endPoint = ADSRPoints[i + 1];

            juce::Point<float> pointOnPath;

            roundedPath.getNearestPoint(ADSRPoints[i], pointOnPath);
            g.fillEllipse(pointOnPath.getX() - correction, pointOnPath.getY() - correction, pointSize, pointSize);
        }
           
        g.fillEllipse(ADSRPoints[4].getX() - correction, ADSRPoints[4].getY() - correction,pointSize, pointSize);
            
    }
}

void ADSRDisplay::resized()
{
    if (mActiveSample != nullptr)
        updateADSRPoints(mActiveSample.get()->getADSR());
}

void ADSRDisplay::activeSampleChanged(SampleModel& sm)
{

    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(sm.getState());

    if (mActiveSample != nullptr)
        mActiveSample->addListener(*this);
}

void ADSRDisplay::adsrChanged(ADSRParameters params)
{
    updateADSRPoints(params);
}

void ADSRDisplay::updateADSRPoints(ADSRParameters params)
{
    ADSRPoints.clear();

    float width = static_cast<float>(getLocalBounds().getWidth());
    float height = static_cast<float>(getLocalBounds().getHeight());

    float marginX = 0.05f * width;
    float marginY = 0.05f * height;

    float attackTime = params.attack;
    float decayTime = params.decay;
    float sustainLevel = params.sustain;
    float releaseTime = params.release;

    // Define the range for normalization
    float range = 5.0f - 0.1f;

    // Percentage of Window for each parameter to take up
    float attackPercentage = 0.25f;
    float attackWidth = width * attackPercentage;

    float decayPercentage = 0.25f;
    float decayWidth = width * decayPercentage;

    float sustainPercentage = 0.25f;
    float sustainWidth = width * sustainPercentage;

    float releasePercentage = 0.25f;
    float releaseWidth = width * releasePercentage;

    // Normalize the times based on the defined range
    float normalizedAttackTime = (attackTime - 0.1f) / range;
    float normalizedDecayTime = (decayTime - 0.1f) / range;
    float normalizedReleaseTime = (releaseTime - 0.1f) / range;

    // Calculate points based on normalized times
    float attackPoint = normalizedAttackTime * attackWidth;
    float decayPoint = (normalizedAttackTime + normalizedDecayTime) * decayWidth;
    float releaseStart = .75f;
    float releasePoint = releaseStart + (normalizedReleaseTime * (releaseWidth));

    //StartPoint
    ADSRPoints.push_back({ marginX, height - marginY });

    //AttackPoint
    ADSRPoints.push_back({ marginX + attackPoint , marginY });

    //DecayPoint
    ADSRPoints.push_back({ marginX + decayPoint, ((height)-sustainLevel * height) + marginY });

    //sustainPoint
    ADSRPoints.push_back({ releaseStart * (width - marginX),((height)-sustainLevel * height) + marginY });

    //ReleasePoint
    ADSRPoints.push_back({ (releaseStart * (width - marginX)) + releasePoint, height - marginY });

    repaint();
}
