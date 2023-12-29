/*
  ==============================================================================

    Identifiers.h
    Created: 28 Dec 2023 9:19:29am
    Author:  camro

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ValueTreeObjectList.h"
#include "ReaderFactory.h"

namespace IDs
{

#define DECLARE_ID(name) const juce::Identifier name (#name); 

    DECLARE_ID(DATA_MODEL)

    DECLARE_ID(SAMPLE)
    DECLARE_ID(id)
    DECLARE_ID(name)
    DECLARE_ID(file)

    DECLARE_ID(SETTINGS)

#undef DECLARE_ID

}

class SampleModel : public juce::ValueTree::Listener
{
public:
     explicit SampleModel()
        :SampleModel(juce::ValueTree(IDs::SAMPLE)) {}

    SampleModel(const juce::ValueTree& vt)
        :state(vt),
        name(state, IDs::name, nullptr),
        id(state, IDs::id, nullptr), 
        audioFile(state, IDs::file, nullptr)
    {
        jassert(state.hasType(IDs::SAMPLE));
        state.addListener(this);
    }

    //============ Setter Methods ============
    void setName(const juce::String n)
    {
        name.setValue(n, nullptr);
    }

    void setId(const int i)
    {
        id.setValue(i, nullptr);
    }

    void setAudioFile(juce::File& file)
    {
        audioFile = std::make_shared<juce::File>(file); 
    }

    //============ Getter Methods ============
    juce::String getName()
    {
        return name;
    }

    int getId()
    {
        return id;
    }

    std::shared_ptr<juce::File> getAudioFile()
    {
        if (audioFile.get() == nullptr)
            return nullptr;
        else
            return audioFile; 
    }

    //=======================================
    juce::AudioFormatManager* audioFormatManager; 
    juce::ValueTree state;
private:
    void valueTreePropertyChanged(juce::ValueTree& treeChanged, const juce::Identifier& property)
    {
        if (treeChanged == state)
        {
            if (property == IDs::name)
                name.forceUpdateOfCachedValue();
            else if (property == IDs::id)
            {
                id.forceUpdateOfCachedValue();
            }
            else if (property == IDs::file)
                audioFile.forceUpdateOfCachedValue();
        }
    }

    juce::CachedValue<int> id;
    juce::CachedValue<juce::String> name;
    juce::CachedValue<std::shared_ptr<juce::File>> audioFile;
};


class DataModel : public juce::ValueTree::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void nameChanged(SampleModel&) {}
        virtual void fileChanged(SampleModel&) {}
    };

    explicit DataModel()
        : DataModel(juce::ValueTree(IDs::DATA_MODEL))
    {

    }

    DataModel(const juce::ValueTree& vt)
        : state(vt), numSamples(0)
    {
        state.addListener(this);
    }

    DataModel(const DataModel& other)
        :DataModel(other.state)
    {

    }

    // Move this method outside of the class? 
    void initializeDefualtModel(int numSamples)
    {
        jassert(state.hasType(IDs::DATA_MODEL));

        // Initialize tree with number of samples
        for (int i = 0; i < numSamples; ++i)
        {
            SampleModel sampleModel;

            const juce::String name("Sample" + juce::String(i + 1));
            sampleModel.setName(name);
            sampleModel.setId(i);

            sampleModel.state.addListener(this);
            state.addChild(sampleModel.state, -1, nullptr);
 
        }
        this->numSamples = numSamples; 
    }
    
    

    //============Listener Methods============
    void addListener(Listener& listener)
    {
        listenerList.add(&listener);
    }

    void removeListener(Listener& listener)
    {
        listenerList.remove(&listener);
    }

    juce::ValueTree state;

private:
    void valueTreePropertyChanged(juce::ValueTree& treeChanged, const juce::Identifier& property) override
    {
        
        for (const auto& subTree : state)
        {
            if (treeChanged == subTree)
            {
                if (treeChanged.hasType(IDs::SAMPLE))
                {
                    if (property == IDs::name)
                    {
                        listenerList.call([&](Listener& l) { l.nameChanged(SampleModel(subTree)); });
                    }
                    else if (property == IDs::file)
                    {
                        listenerList.call([&](Listener& l) { l.fileChanged(SampleModel(subTree)); }); 
                    }
                }

                return; 
            }
        }
    }


    int numSamples;
    juce::ListenerList<Listener> listenerList;
};


