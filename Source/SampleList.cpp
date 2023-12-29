/*
  ==============================================================================

    SampleList.cpp
    Created: 29 Dec 2023 7:30:49am
    Author:  camro

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SampleList.h"
//=========================== Sample Button ===========================

SampleButton::SampleButton(juce::ValueTree vt)
    :state(vt)
{
    jassert(vt.hasType(IDs::SAMPLE)); 
    state.addListener(this); 
    updateText(); 
}

SampleButton::~SampleButton()
{

}

void SampleButton::resized()
{

}

void SampleButton::clicked()
{
    state.setProperty(IDs::name, "TEST", nullptr); 
}


void SampleButton::valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property)
{
    if (tree == state)
    {
        if (property == IDs::name)
        {
            updateText(); 
        }
    }
}

void SampleButton::updateText()
{
    juce::String name = state[IDs::name]; 
    DBG(name);
    setButtonText(name); 
    repaint(); 
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
    g.setColour(juce::Colours::burlywood); 
    g.drawRect(getLocalBounds().toFloat()); 
}

void SampleList::resized()
{
    auto bounds = getLocalBounds();
    auto numSampleButtons = objects.size(); 
    auto sbHeight = getHeight() / numSampleButtons; 

    for (auto* sb : objects)
    {
        sb->setBounds(bounds.removeFromTop(sbHeight));
    }
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

