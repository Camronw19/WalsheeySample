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

    juce::CachedValue<int> id;
    juce::CachedValue<juce::String> name;
};

//List that holds sampleModel objects that are constructed from the children of the data model tree
class SampleModelList : public ValueTreeObjectList<SampleModel>
{
public:
    SampleModelList(const juce::ValueTree& vt)
        : ValueTreeObjectList<SampleModel>(vt)
    {
        rebuildObjects();
    }

    ~SampleModelList()
    {
        freeObjects();
    }

    bool isSuitableType(const juce::ValueTree& vt) const override
    {
        return vt.hasType(IDs::SAMPLE);
    }

    SampleModel* createNewObject(const juce::ValueTree& vt) override
    {
        return new SampleModel(vt);
    }

    void deleteObject(SampleModel* sample) override
    {
        if (sample != nullptr)
        {
            delete sample;
            sample = nullptr;
        }
    }

    void newObjectAdded(SampleModel*) override {}
    void objectRemoved(SampleModel*) override {}
    void objectOrderChanged() override {}
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
        : valueTree(vt), sampleList(vt), numSamples(0)
    {
        valueTree.addListener(this);
    }

    DataModel(const DataModel& other)
        :DataModel(other.valueTree)
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
            valueTree.addChild(sample, -1, nullptr);
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

    SampleModelList sampleList;
    juce::ValueTree valueTree;

private:
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override
    {
        if (tree.hasType(IDs::SAMPLE))
        {
            for (auto sample : sampleList.objects)
            {
                if (tree == sample->state)
                {
                    listenerList.call([&](Listener& l) { l.nameChanged(*sample); });
                }
            }
        }
    }


    int numSamples;
    juce::ListenerList<Listener> listenerList;
};


