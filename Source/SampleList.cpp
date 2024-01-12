/*
  ==============================================================================

    SampleList.cpp
    Created: 29 Dec 2023 7:30:49am
    Author:  camro

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SampleList.h"
#include "UIConfig.h"
//=========================== Sample Button ===========================

SampleButton::SampleButton(juce::ValueTree vt)
    :model (vt)
{
    jassert(vt.hasType(IDs::SAMPLE)); 
    model.addListener(*this); 
    updateText(); 
}

SampleButton::~SampleButton()
{

}

void SampleButton::paint(juce::Graphics& g)
{

        g.setColour(AppColors::backgroundColour);

    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10);

    //Draw outline
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 10, 1);

    //Draw key
    g.setColour(juce::Colours::white);

    if (model.isActive() == true)
        g.setColour(AppColors::accentColour);

    g.setFont(12.0f);
    g.drawText("Button", getLocalBounds().reduced(10), juce::Justification::topLeft, true);

    //Draw file name
    g.drawText(model.getName(), getLocalBounds(), juce::Justification::centred, true);
}

void SampleButton::resized()
{

}

void SampleButton::clicked()
{
    model.setIsActive(true); 
}


void SampleButton::nameChanged(juce::String)
{
    updateText();
}

void SampleButton::isActiveChanged(bool isActive)
{
    repaint();
}


void SampleButton::updateText()
{
    juce::String name = model.getName(); 
    DBG(name);
    setButtonText(name); 
    repaint(); 
}

bool SampleButton::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto file : files)
        if (file.contains(".wav") || file.contains(".mp3") || file.contains(".aif"))
            return true;

    // If no valid files are found, show an error message
    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::WarningIcon,
        "Invalid File Format",
        "Please drag and drop only WAV, MP3, or AIF files.",
        "OK"
    );

    return false;
}

void SampleButton::filesDropped(const juce::StringArray& files, int x, int y)
{
    auto localPoint = getLocalPoint(this, juce::Point<int>(x, y));

    for (auto filePath : files)
    {
        if (isInterestedInFileDrag(filePath))
        {
            // Check if the drop location is within the bounds of the button
            if (getLocalBounds().contains(localPoint))
            {
                const juce::File file(filePath);
                model.setAudioFile(file);
                model.setName(file.getFileNameWithoutExtension());

                // Update button or perform other actions
                model.setIsActive(true);
                DBG("File dropped on button: " + model.getName());
            }
        }
    }
}


//=========================== Sample List ===========================
SampleList::SampleList(juce::ValueTree dataModel)
    :ValueTreeObjectList<SampleButton>(dataModel)
{
    rebuildObjects(); 
}

SampleList::~SampleList()
{
    freeObjects(); 
}

void SampleList::paint (juce::Graphics& g)
{
    g.setColour(juce::Colour::fromRGB(11, 12, 14));
    g.drawRect(getLocalBounds().toFloat()); 
}

void SampleList::resized()
{
    juce::Grid grid;

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    grid.templateRows = { Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.items = { juce::GridItem(*objects[0]).withArea(1,1),
                   juce::GridItem(*objects[1]).withArea(1,2),
                   juce::GridItem(*objects[2]).withArea(1,3),
                   juce::GridItem(*objects[3]).withArea(1,4),
                   juce::GridItem(*objects[4]).withArea(2,1),
                   juce::GridItem(*objects[5]).withArea(2,2),
                   juce::GridItem(*objects[6]).withArea(2,3),
                   juce::GridItem(*objects[7]).withArea(2,4),
    };


    int gapSize = std::min(getWidth() / 25, getHeight() / 25);
    grid.setGap(juce::Grid::Px(gapSize));
    grid.performLayout(getLocalBounds().reduced(10));


}

bool SampleList::isSuitableType(const juce::ValueTree& sample) const
{
    return sample.hasType(IDs::SAMPLE); 
}

SampleButton* SampleList::createNewObject(const juce::ValueTree& vt)
{
    auto* sampleButton = new SampleButton(vt);
    addAndMakeVisible(sampleButton);
    return sampleButton;
}

void SampleList::deleteObject(SampleButton* sampleButton)
{
    delete sampleButton; 
}

void SampleList::newObjectAdded(SampleButton*)
{
    resized(); 
}

void SampleList::objectRemoved(SampleButton*)
{
    resized(); 
}

void SampleList::objectOrderChanged()
{
    resized(); 
}

