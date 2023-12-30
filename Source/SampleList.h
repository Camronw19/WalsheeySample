/*
  ==============================================================================

    SampleList.h
    Created: 29 Dec 2023 7:30:49am
    Author:  camro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Model.h"
#include "ValueTreeObjectList.h"

//==============================================================================
/*
*/
class SampleButton : public juce::TextButton, 
                     public juce::FileDragAndDropTarget,
                     public SampleModel::Listener
{
public:
    SampleButton(juce::ValueTree sampleTree);
    ~SampleButton() override;

    void resized() override;
    void clicked() override; 

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void updateText();

    void nameChanged(juce::String) override; 
    void fileChanged() override; 

    SampleModel model; 
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleButton)
};


class SampleList  : public juce::Component, 
                    public ValueTreeObjectList<SampleButton> 
{
public:
    SampleList(juce::ValueTree dataModel);
    ~SampleList() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool isSuitableType(const juce::ValueTree&) const override; 
    SampleButton* createNewObject(const juce::ValueTree&) override;
    void deleteObject(SampleButton*) override;

    void newObjectAdded(SampleButton*) override;
    void objectRemoved(SampleButton*) override;
    void objectOrderChanged() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleList)
};


