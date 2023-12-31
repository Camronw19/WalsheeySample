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

namespace IDs
{

#define DECLARE_ID(name) const juce::Identifier name (#name); 

    DECLARE_ID(DATA_MODEL)

    DECLARE_ID(SAMPLE)
    DECLARE_ID(id)
    DECLARE_ID(name)

    DECLARE_ID(SETTINGS)

#undef DECLARE_ID

}

//Provides an abstraction for SAMPLE value trees
class SampleModel : public juce::ValueTree::Listener
{
public:
    SampleModel(const juce::ValueTree& vt)
        :state(vt),
        name(state, IDs::name, nullptr),
        id(state, IDs::id, nullptr)
    {
        jassert(state.hasType(IDs::SAMPLE));
        state.addListener(this);
    }

    void setName(const juce::String n)
    {
        name.setValue(n, nullptr);
    }

    void setId(const int i)
    {
        id.setValue(i, nullptr);
    }

    juce::String getName()
    {
        return name;
    }

    int getId()
    {
        return id;
    }

    juce::ValueTree state;
private:
    void valueTreePropertyChanged(juce::ValueTree& treeChanged, const juce::Identifier& property)
    {
        if (property == IDs::name)
        {
            name.forceUpdateOfCachedValue(); 
        }
        else if (property == IDs::id)
        {
            id.forceUpdateOfCachedValue(); 
        }
    }

    juce::CachedValue<int> id;
    juce::CachedValue<juce::String> name;
};

class DataModel : public juce::ValueTree::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void nameChanged(SampleModel&) {}
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

    void initializeDefualtModel(int numSamples)
    {
        jassert(valueTree.hasType(IDs::DATA_MODEL));

        // Initialize tree with number of samples
        for (int i = 0; i < numSamples; ++i)
        {
            juce::ValueTree sample(IDs::SAMPLE);
            const juce::String name("Sample" + juce::String(i + 1));
            sample.setProperty(IDs::name, name, nullptr);
            sample.setProperty(IDs::id, i, nullptr);
            sample.addListener(this);
            state.addChild(sample, -1, nullptr);
        }
        this->numSamples = numSamples; 
    }
    
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
                }

                return; 
            }
        }
    }

    int numSamples;
    juce::ListenerList<Listener> listenerList;
};


