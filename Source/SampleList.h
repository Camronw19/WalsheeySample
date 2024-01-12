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
#include "AudioDisplay.h"

//==============================================================================
/*
*/
class SampleButton : public juce::TextButton, 
                     public juce::FileDragAndDropTarget,
                     private SampleModel::Listener
{
public:
    SampleButton(juce::ValueTree sampleTree);
    ~SampleButton() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    SampleModel model; // Don't change this name! It is typed to the ValueTreeObjectList
private:
    // Sample model listener 
    void nameChanged(juce::String) override;  
    void isActiveChanged(bool) override; 

    void filesDropped(const juce::StringArray& files, int x, int y) override;
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void clicked() override; 
    void updateText();


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

private:
    // ValueTreeObjectList methods
    bool isSuitableType(const juce::ValueTree&) const override; 
    SampleButton* createNewObject(const juce::ValueTree&) override;
    void deleteObject(SampleButton*) override;
    void newObjectAdded(SampleButton*) override;
    void objectRemoved(SampleButton*) override;
    void objectOrderChanged() override;
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleList)
};


