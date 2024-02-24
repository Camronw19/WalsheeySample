/*
  ==============================================================================

    ADSRView.cpp
    Created: 5 Jan 2024 9:40:11am
    Author:  Camron

  ==============================================================================
*/

#include "ADSRView.h"

ADSRGraph::ADSRGraph(const DataModel& dm)
    : mDataModel(dm)
{
    mDataModel.addListener(*this);
}

ADSRGraph::~ADSRGraph()
{
}

void ADSRGraph::paint(juce::Graphics& g)
{
    g.setColour(getLookAndFeel().findColour(AppColors::ColourIds::BackgroundDefault));
    g.fillAll(); 

    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(spacing::padding3), spacing::padding4);

    g.setColour(AppColors::Dark::divider); 
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(spacing::padding3), spacing::padding4, 3); 

    if (mActiveSample != nullptr)
        paintIfFileLoaded(g); 
    else
        paintIfNoFileLoaded(g); 
}

void ADSRGraph::paintIfNoFileLoaded(juce::Graphics& g)
{
    g.setColour(getLookAndFeel().findColour(AppColors::ColourIds::Accent));
    g.drawFittedText("No Clip Selected", getLocalBounds(), juce::Justification::centred, 1);
}

void ADSRGraph::paintIfFileLoaded(juce::Graphics& g)
{
    //create path with points
    juce::Path adsrPath;

    for (int i = 0; i < ADSRPoints.size() - 1; ++i)
    {
        juce::Point<float> start = ADSRPoints[i];
        juce::Point<float> end = ADSRPoints[i + 1];

        if (i == 0)
            adsrPath.startNewSubPath(start);

        adsrPath.lineTo(end);
    }

    g.setColour(getLookAndFeel().findColour(AppColors::ColourIds::Accent));
    juce::Path roundedPath = adsrPath.createPathWithRoundedCorners(15.0f);
    g.strokePath(roundedPath, juce::PathStrokeType(2.0f, juce::PathStrokeType::beveled, juce::PathStrokeType::EndCapStyle::rounded));

    //Draw Points
    float pointSize = 6.0f;
    float correction = pointSize / 2;
    g.setColour(getLookAndFeel().findColour(AppColors::ColourIds::ActionActive));

    for (int i = 1; i <= 2; ++i)
    {
        juce::Point<float> startPoint = ADSRPoints[i];
        juce::Point<float> endPoint = ADSRPoints[i + 1];

        juce::Point<float> pointOnPath;

        roundedPath.getNearestPoint(ADSRPoints[i], pointOnPath);
        g.fillEllipse(pointOnPath.getX() - correction, pointOnPath.getY() - correction, pointSize, pointSize);
    }

    g.fillEllipse(ADSRPoints[4].getX() - correction, ADSRPoints[4].getY() - correction, pointSize, pointSize);
}

void ADSRGraph::resized()
{
    if (mActiveSample != nullptr)
        updateADSRPoints(mActiveSample.get()->getADSR());
}

void ADSRGraph::activeSampleChanged(SampleModel& sm)
{

    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(sm.getState());

    if (mActiveSample != nullptr)
    {
        mActiveSample->addListener(*this);
        updateADSRPoints(mActiveSample.get()->getADSR());
    }
}

void ADSRGraph::adsrChanged(ADSRParameters params)
{
    updateADSRPoints(params);
}

void ADSRGraph::updateADSRPoints(ADSRParameters params)
{
    ADSRPoints.clear();

    float width = static_cast<float>(getLocalBounds().reduced(spacing::padding3).getWidth());
    float height = static_cast<float>(getLocalBounds().reduced(spacing::padding3).getHeight());

    float marginX = .05 * width;
    float marginY = .05 * height;

    float attackTime = params.attack;
    float decayTime = params.decay;
    float sustainLevel = params.sustain;
    float releaseTime = params.release;

    // Define the range for normalization
    float range = 5.0f - 0.1f;

    // Percentage of Window for each parameter to take up
    float adsrWidthPercentage = 0.25f;
    float adsrWidth = width * adsrWidthPercentage;

    // Normalize the times based on the defined range
    float normalizedAttackTime = (attackTime - 0.1f) / range;
    float normalizedDecayTime = (decayTime - 0.1f) / range;
    float normalizedReleaseTime = (releaseTime - 0.1f) / range;

    // Calculate points based on normalized times
    float attackPoint = normalizedAttackTime * adsrWidth;
    float decayPoint = (normalizedAttackTime + normalizedDecayTime) * adsrWidth;
    float releaseStart = .75f;
    float releasePoint = releaseStart + (normalizedReleaseTime * (adsrWidth));

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


//==============================================================================

ADSRControls::ADSRControls(const DataModel& dm)
    : mDataModel(dm)
{
    mDataModel.addListener(*this); 

    // Initialize sliders 
    for (auto& slider : getSliders())
    {
        slider->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->addListener(this);
        addAndMakeVisible(slider);
    }

    mAttackSlider.setRange(.1, 5, .1);
    mDecaySlider.setRange(.1, 5, .1);
    mSustainSlider.setRange(.1, 1, .01);
    mReleaseSlider.setRange(0, 5, .1);

    // Initialize labels
    for (auto& label : getLabels())
    {
        label->setColour(juce::Label::textColourId, AppColors::Dark::textPrimary);
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }

    mSampleLabel.setJustificationType(juce::Justification::centredBottom);
    mAttackLabel.setText("Attack", juce::dontSendNotification);
    mDecayLabel.setText("Decay", juce::dontSendNotification);
    mSustainLabel.setText("Sustain", juce::dontSendNotification);
    mReleaseLabel.setText("Release", juce::dontSendNotification);
}

ADSRControls::~ADSRControls()
{
    mDataModel.removeListener(*this); 
}

void ADSRControls::paint(juce::Graphics& g)
{
    g.setColour(AppColors::Dark::divider); 
    auto rectBounds = getLocalBounds().toFloat(); 
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(spacing::padding3, 0), spacing::padding4);
}

void ADSRControls::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromBottom(spacing::padding3); 

    //Attack Component
    juce::FlexBox attackFlexBox;
    attackFlexBox.flexDirection = juce::FlexBox::Direction::column;
    attackFlexBox.items.add(juce::FlexItem(mAttackSlider).withFlex(5));
    attackFlexBox.items.add(juce::FlexItem(mAttackLabel).withFlex(1));

    //Decay Component
    juce::FlexBox decayFlexBox;
    decayFlexBox.flexDirection = juce::FlexBox::Direction::column;
    decayFlexBox.items.add(juce::FlexItem(mDecaySlider).withFlex(5));
    decayFlexBox.items.add(juce::FlexItem(mDecayLabel).withFlex(1));

    //Sustain Component
    juce::FlexBox sustainFlexBox;
    sustainFlexBox.flexDirection = juce::FlexBox::Direction::column;
    sustainFlexBox.items.add(juce::FlexItem(mSustainSlider).withFlex(5));
    sustainFlexBox.items.add(juce::FlexItem(mSustainLabel).withFlex(1));

    //Release Component
    juce::FlexBox releaseFlexBox;
    releaseFlexBox.flexDirection = juce::FlexBox::Direction::column;
    releaseFlexBox.items.add(juce::FlexItem(mReleaseSlider).withFlex(5));
    releaseFlexBox.items.add(juce::FlexItem(mReleaseLabel).withFlex(1));

    //Slider Layout
    juce::FlexBox sliderFlexBox;
    sliderFlexBox.items.add(juce::FlexItem(attackFlexBox).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(decayFlexBox).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(sustainFlexBox).withFlex(1.0, 1.0));
    sliderFlexBox.items.add(juce::FlexItem(releaseFlexBox).withFlex(1.0, 1.0));

    sliderFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    sliderFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    sliderFlexBox.alignContent = juce::FlexBox::AlignContent::center;

    sliderFlexBox.performLayout(bounds);
}

std::vector<juce::Slider*> ADSRControls::getSliders()
{
    return { &mAttackSlider, &mDecaySlider, &mSustainSlider, &mReleaseSlider };
}

std::vector<juce::Label*> ADSRControls::getLabels()
{
    return { &mSampleLabel, &mAttackLabel, &mDecayLabel, &mSustainLabel, &mReleaseLabel };
}

void ADSRControls::sliderValueChanged(juce::Slider* slider)
{
    if (mActiveSample != nullptr)
    {
        if (slider == &mAttackSlider)
        {
            mActiveSample->setAttack(mAttackSlider.getValue());
        }
        else if (slider == &mDecaySlider)
        {
            mActiveSample->setDecay(mDecaySlider.getValue());
        }
        else if (slider == &mSustainSlider)
        {
            mActiveSample->setSustain(mSustainSlider.getValue());
        }
        else if (slider == &mReleaseSlider)
        {
            mActiveSample->setRelease(mReleaseSlider.getValue());
        }
    }
}

void ADSRControls::activeSampleChanged(SampleModel& sm)
{
    if (mActiveSample != nullptr)
        mActiveSample->removeListener(*this);

    mActiveSample = std::make_unique<SampleModel>(sm.getState());

    if (mActiveSample != nullptr)
        mActiveSample->addListener(*this);

    //Set ADSR parameters
    ADSRParameters params = mActiveSample->getADSR();
    mAttackSlider.setValue(params.attack);
    mDecaySlider.setValue(params.decay);
    mSustainSlider.setValue(params.sustain);
    mReleaseSlider.setValue(params.release);

    //Set sample label 
    mSampleLabel.setText(mActiveSample->getName(), juce::dontSendNotification);
}


//==============================================================================

ADSRView::ADSRView(const DataModel& dm)
    :mADSRGraph(dm), mADSRControls(dm)
{
    addAndMakeVisible(mADSRGraph);
    addAndMakeVisible(mADSRControls);
}

ADSRView::~ADSRView()
{
}

void ADSRView::paint (juce::Graphics& g)
{
    g.setColour(AppColors::Dark::backgroundDefault);
    g.fillAll(); 
}

void ADSRView::resized()
{
    auto bounds = getLocalBounds(); 
    mADSRGraph.setBounds(bounds.removeFromTop(bounds.getHeight() * .7)); 
    mADSRControls.setBounds(bounds); 
}



