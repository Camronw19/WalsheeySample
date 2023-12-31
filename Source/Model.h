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
    DECLARE_ID(file)

    DECLARE_ID(SETTINGS)

#undef DECLARE_ID

}

template <typename Contents>
class ReferenceCountingAdapter : public juce::ReferenceCountedObject
{
public:
    template <typename... Args>
    explicit ReferenceCountingAdapter(Args&&... args)
        : contents(std::forward<Args>(args)...)
    {
    }

    const Contents& get() const
    {
        return contents;
    }

    Contents& get()
    {
        return contents;
    }

private:
    Contents contents;
};

template <typename Contents, typename... Args>
std::unique_ptr<ReferenceCountingAdapter<Contents>>
make_reference_counted(Args&&... args)
{
    auto adapter = new ReferenceCountingAdapter<Contents>(std::forward<Args>(args)...);
    return std::unique_ptr<ReferenceCountingAdapter<Contents>>(adapter);
}

template <typename Wrapped>
struct GenericVariantConverter
{
    static Wrapped fromVar(const juce::var& v)
    {
        auto cast = dynamic_cast<ReferenceCountingAdapter<Wrapped>*> (v.getObject());
        jassert(cast != nullptr);
        return cast->get();
    }

    static juce::var toVar(Wrapped range)
    {
        return { make_reference_counted<Wrapped>(std::move(range)).release() };
    }
};


template<>
struct juce::VariantConverter<std::shared_ptr<juce::File>>
    : GenericVariantConverter<std::shared_ptr<juce::File>>
{
};


class Model : public juce::ValueTree::Listener
{
public: 
    Model(const juce::ValueTree& vt)
        : state(vt)
    {
        state.addListener(this); 
    }

    juce::ValueTree getState() const
    {
        return state; 
    }

    virtual void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) = 0; 
private: 
    juce::ValueTree state; 
};

class SampleModel : public Model
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void nameChanged(juce::String) {}
        virtual void fileChanged() {}
    };

     explicit SampleModel()
        :SampleModel(juce::ValueTree(IDs::SAMPLE)) {}

    SampleModel(const juce::ValueTree& vt)
        :Model(vt),
        name(getState(), IDs::name, nullptr),
        id(getState(), IDs::id, nullptr),
        audioFile(getState(), IDs::file, nullptr)
    {
        jassert(getState().hasType(IDs::SAMPLE));
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

    void setAudioFile(const juce::File& file)
    {
        audioFile.setValue(std::make_shared<juce::File>(file), nullptr); 
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
        return audioFile; 
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

private:
    void valueTreePropertyChanged(juce::ValueTree& treeChanged, const juce::Identifier& property)
    {
        if (treeChanged == getState())
        {
            if (property == IDs::name)
            {
                name.forceUpdateOfCachedValue();
                listenerList.call([&](Listener& l) { l.nameChanged(name); });
            }
            else if (property == IDs::file)
            {
                audioFile.forceUpdateOfCachedValue();
                listenerList.call([&](Listener& l) { l.fileChanged(); });
            }
        }
    }

    juce::CachedValue<int> id;
    juce::CachedValue<juce::String> name;
    juce::CachedValue<std::shared_ptr<juce::File>> audioFile;

    juce::ListenerList<Listener> listenerList;
};


class DataModel : public Model
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
        : Model(vt), numSamples(0)
    {
       
    }

    DataModel(const DataModel& other)
        :DataModel(other.getState())
    {

    }

    // Move this method outside of the class? 
    void initializeDefualtModel(int numSamples)
    {
        jassert(getState().hasType(IDs::DATA_MODEL));

        // Initialize tree with number of samples
        for (int i = 0; i < numSamples; ++i)
        {
            SampleModel sampleModel;

            const juce::String name("Sample" + juce::String(i + 1));
            sampleModel.setName(name);
            sampleModel.setId(i);

            sampleModel.getState().addListener(this);
            getState().addChild(sampleModel.getState(), -1, nullptr);
 
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

private:
    void valueTreePropertyChanged(juce::ValueTree& treeChanged, const juce::Identifier& property) override
    {
        
        for (const auto& subTree : getState())
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


