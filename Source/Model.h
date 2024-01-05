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
    DECLARE_ID(isActive)
    DECLARE_ID(midiNote)
    DECLARE_ID(attack)
    DECLARE_ID(decay)
    DECLARE_ID(sustain)
    DECLARE_ID(release)

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

struct ADSRParameters
{
    ADSRParameters() = default;

    ADSRParameters(float attackTimeSeconds,
        float decayTimeSeconds,
        float sustainLevel,
        float releaseTimeSeconds)
        : attack(attackTimeSeconds),
        decay(decayTimeSeconds),
        sustain(sustainLevel),
        release(releaseTimeSeconds)
    {
    }

    float attack = 0.1f, decay = 0.1f, sustain = 1.0f, release = 0.1f;

    bool operator==(const ADSRParameters& other) const
    {
        const float epsilon = 0.001f;

        return (std::abs(attack - other.attack) < epsilon) &&
            (std::abs(decay - other.decay) < epsilon) &&
            (std::abs(sustain - other.sustain) < epsilon) &&
            (std::abs(release - other.release) < epsilon);
    }

    bool operator!=(const ADSRParameters& other) const
    {
        return !(*this == other);
    }
};


class Model : public juce::ValueTree::Listener
{
public: 
    Model(const juce::ValueTree& vt)
        : state(vt)
    {
        state.addListener(this); 
    }

    virtual ~Model() {}

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
        virtual void isActiveChanged(bool) {}
        virtual void midiNoteChanged(int) {}
        virtual void attackChanged(float) {}
        virtual void decayChanged(float) {}
        virtual void sustainChanged(float) {}
        virtual void releaseChanged(float) {}
    };

     explicit SampleModel()
        :SampleModel(juce::ValueTree(IDs::SAMPLE)) {}

    SampleModel(const juce::ValueTree& vt)
        :Model(vt),
        id(getState(), IDs::id, nullptr), 
        name(getState(), IDs::name, nullptr),
        audioFile(getState(), IDs::file, nullptr), 
        midiNote(getState(), IDs::midiNote, nullptr),
        attack(getState(), IDs::attack, nullptr),
        decay(getState(), IDs::decay, nullptr),
        sustain(getState(), IDs::sustain, nullptr),
        release(getState(), IDs::release, nullptr),
        isActiveSample(getState(), IDs::isActive, nullptr)
    {
        jassert(getState().hasType(IDs::SAMPLE));
    }

    SampleModel(const SampleModel& other)
        :SampleModel(other.getState()) {}

    //============ Setter Methods ============
    void setName(const juce::String n)
    {
        name.setValue(n, nullptr);
    }

    void setAudioFile(const juce::File& file)
    {
        audioFile.setValue(std::make_shared<juce::File>(file), nullptr); 
    }

    void setIsActive(const bool active) 
    {
        isActiveSample.setValue(active, nullptr);
    }

    void setId(const int i)
    {
        id.setValue(i, nullptr); 
    }

    void setMidiNote(const int m)
    {
        int clipped = juce::Range<int>(0, 127).clipValue(m); 
        midiNote.setValue(clipped, nullptr); 
    }

    void setAttack(const float a)
    {
        DBG("SETTING ATTACK VT"); 
        attack.setValue(a, nullptr); 
    }

    void setDecay(const float d)
    {
        decay.setValue(d, nullptr); 
    }

    void setSustain(const float s)
    {
        sustain.setValue(s, nullptr); 
    }

    void setRelease(const float r)
    {
        release.setValue(r, nullptr); 
    }

    //============ Getter Methods ============
    juce::String getName() const 
    {
        return name;
    }

    std::shared_ptr<juce::File> getAudioFile() const
    {
        return audioFile; 
    }

    bool isActive() const 
    {
        return isActiveSample; 
    }

    int getId() const
    {
        return id; 
    }

    int getMidiNote()
    {
        return midiNote; 
    }

    ADSRParameters getADSR()
    {
        return ADSRParameters(attack, decay, sustain, release); 
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
                return; 
            }
            else if (property == IDs::file)
            {
                audioFile.forceUpdateOfCachedValue();
                listenerList.call([&](Listener& l) { l.fileChanged(); });
                return;
            }
            else if(property == IDs::isActive)
            {
                isActiveSample.forceUpdateOfCachedValue(); 
                listenerList.call([&](Listener& l) { l.isActiveChanged(isActiveSample); });
                return;
            }
            else if (property == IDs::midiNote)
            {
                midiNote.forceUpdateOfCachedValue(); 
                listenerList.call([&](Listener& l) { l.midiNoteChanged(midiNote); });
                return; 
            }
            else if (property == IDs::attack)
            {
                attack.forceUpdateOfCachedValue(); 
                listenerList.call([&](Listener& l) { l.attackChanged(attack); }); 
                return;
            }
            else if (property == IDs::decay)
            {
                decay.forceUpdateOfCachedValue();
                listenerList.call([&](Listener& l) { l.decayChanged(attack); });
                return;
            }
            else if (property == IDs::sustain)
            {
                sustain.forceUpdateOfCachedValue();
                listenerList.call([&](Listener& l) { l.sustainChanged(attack); });
                return;
            }
            else if (property == IDs::release)
            {
                release.forceUpdateOfCachedValue();
                listenerList.call([&](Listener& l) { l.releaseChanged(attack); });
                return;
            }
        }
    }

    juce::CachedValue<int> id; 
    juce::CachedValue<juce::String> name;
    juce::CachedValue<std::shared_ptr<juce::File>> audioFile;
    juce::CachedValue<int> midiNote; 
    juce::CachedValue<float> attack;
    juce::CachedValue<float> decay;
    juce::CachedValue<float> sustain;
    juce::CachedValue<float> release;
    juce::CachedValue<bool> isActiveSample; 

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
        virtual void activeSampleChanged(SampleModel&) {}
    };

    explicit DataModel()
        : DataModel(juce::ValueTree(IDs::DATA_MODEL)) {}

    DataModel(const juce::ValueTree& vt)
        : Model(vt), activeSample() {}

    DataModel(const DataModel& other)
        : DataModel(other.getState()) 
    {
        activeSample = other.activeSample; 
    }

    void  initializeDefaultModel(int numSamples)
    {
        jassert(getState().hasType(IDs::DATA_MODEL));

        // Initialize tree with number of samples
        for (int i = 0; i < numSamples; ++i)
        {
            SampleModel sampleModel;

            const juce::String name("Sample" + juce::String(i + 1));
            sampleModel.setName(name);
            sampleModel.setId(i);
            sampleModel.setMidiNote(36 + i);

            sampleModel.getState().addListener(this);
            getState().addChild(sampleModel.getState(), -1, nullptr);
        }
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
        if (treeChanged.hasType(IDs::SAMPLE))
        {
            if (property == IDs::name)
            {
                listenerList.call([&](Listener& l) { l.nameChanged(SampleModel(treeChanged)); });
            }
            else if (property == IDs::file) 
            {
                listenerList.call([&](Listener& l) { l.fileChanged(SampleModel(treeChanged)); });
            }
            else if (property == IDs::isActive && treeChanged != activeSample && static_cast<bool>((treeChanged[IDs::isActive])) == true)
            {
                if (activeSample.isValid())
                    activeSample.setProperty(IDs::isActive, false, nullptr); 
                        
                activeSample = treeChanged;
                listenerList.call([&](Listener& l) { l.activeSampleChanged(SampleModel(treeChanged)); });
            }
        }
        return; 
    }

    juce::ValueTree activeSample; 
    juce::ListenerList<Listener> listenerList;
};


//void  initializeDefaultModel(DataModel& dataModel, int numSamples)
//{
//    jassert(dataModel.getState().hasType(IDs::DATA_MODEL));
//
//    // Initialize tree with number of samples
//    for (int i = 0; i < numSamples; ++i)
//    {
//        SampleModel sampleModel;
//
//        const juce::String name("Sample" + juce::String(i + 1));
//        sampleModel.setName(name);
//        sampleModel.setId(i);
//        sampleModel.setMidiNote(36 + i);
//
//        sampleModel.getState().addListener(&dataModel);
//        dataModel.getState().addChild(sampleModel.getState(), -1, nullptr);
//    }
//
//    dataModel.setNumSamples(numSamples);
//}
