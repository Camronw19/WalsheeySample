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
    DECLARE_ID(adsr)
    DECLARE_ID(pitchShift)
    DECLARE_ID(totalRange)
    DECLARE_ID(hZoom)
    DECLARE_ID(hScroll)

    DECLARE_ID(VISIBLE_RANGE)
    DECLARE_ID(visibleRange)

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

template<>
struct juce::VariantConverter<juce::Range<double>>
    : GenericVariantConverter<juce::Range<double>>
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

template<>
struct juce::VariantConverter<ADSRParameters>
    : GenericVariantConverter<ADSRParameters>
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

    virtual ~Model() {}

    juce::ValueTree getState() const
    {
        return state; 
    }

    virtual void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) = 0; 
private: 
    juce::ValueTree state; 
};

class VisibleRangeDataModel : public Model
{
public: 
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void visibleRangeChanged(juce::Range<double>) {}
    };
    
    explicit VisibleRangeDataModel()
        : VisibleRangeDataModel(juce::ValueTree(IDs::VISIBLE_RANGE)) {}

    VisibleRangeDataModel(const juce::ValueTree& vt)
        : Model(vt), 
          totalRange(getState(), IDs::totalRange, nullptr),
          visibleRange(getState(), IDs::visibleRange, nullptr) 
    {
        jassert(getState().hasType(IDs::VISIBLE_RANGE));
        setTotalRange(juce::Range<double>(0, 0)); 
    }

    VisibleRangeDataModel(const VisibleRangeDataModel& other)
        :VisibleRangeDataModel(other.getState()) {}

    //============Accessor Methods============
    void setVisibleRange(const juce::Range<double> newRange)
    {
        visibleRange.setValue(newRange, nullptr); 
    }

    juce::Range<double> getVisibleRange()
    {
        return visibleRange; 
    }

    void setTotalRange(const juce::Range<double> newRange)
    {
        totalRange.setValue(newRange, nullptr);
        visibleRange.setValue(newRange, nullptr); 
    }

    juce::Range<double> getTotalRange()
    {
        return totalRange;
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
            if (property == IDs::visibleRange)
            {
                visibleRange.forceUpdateOfCachedValue();
                listenerList.call([&](Listener& l) { l.visibleRangeChanged(visibleRange); });
                return;
            }
        }
    }

    juce::CachedValue<juce::Range<double>> totalRange;
    juce::CachedValue<juce::Range<double>> visibleRange;
    juce::ListenerList<Listener> listenerList;
};

class SampleModel : public Model
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void nameChanged(juce::String) {}
        virtual void fileChanged(std::shared_ptr<juce::File>) {}
        virtual void isActiveChanged(bool) {}
        virtual void midiNoteChanged(int) {}
        virtual void adsrChanged(ADSRParameters) {}
        virtual void pitchShiftChanged(int) {}
    };

     explicit SampleModel()
        :SampleModel(juce::ValueTree(IDs::SAMPLE)) {}

    SampleModel(const juce::ValueTree& vt)
        :Model(vt),
        id(getState(), IDs::id, nullptr), 
        name(getState(), IDs::name, nullptr),
        audioFile(getState(), IDs::file, nullptr), 
        midiNote(getState(), IDs::midiNote, nullptr),
        adsr(getState(), IDs::adsr, nullptr),
        isActiveSample(getState(), IDs::isActive, nullptr),
        totalRange(getState(), IDs::totalRange, nullptr), 
        pitchShift(getState(), IDs::pitchShift, nullptr),
        hZoom(getState(), IDs::hZoom, nullptr),
        hScroll(getState(), IDs::hScroll, nullptr)
    {
        jassert(getState().hasType(IDs::SAMPLE));
        formatManager.registerBasicFormats(); 
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
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file)); 
        
        setTotalRange(juce::Range<double>(0, static_cast<double>(reader->lengthInSamples / reader->sampleRate)));
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
        ADSRParameters params = getADSR(); 
        params.attack = a; 
        adsr.setValue(params, nullptr); 
    }

    void setDecay(const float d)
    {
        ADSRParameters params = getADSR();
        params.decay = d;
        adsr.setValue(params, nullptr);
    }

    void setSustain(const float s)
    {
        ADSRParameters params = getADSR();
        params.sustain = s;
        adsr.setValue(params, nullptr);
    }

    void setRelease(const float r)
    {
        ADSRParameters params = getADSR();
        params.release = r;
        adsr.setValue(params, nullptr);
    }

    void setTotalRange(const juce::Range<double> range)
    {
        totalRange.setValue(range, nullptr); 
    }

    void setPitchShift(int semitones)
    {
        int constrainedSemitones = juce::Range<int>(-12, 12).clipValue(semitones); 
        pitchShift.setValue(constrainedSemitones, nullptr); 
    }

    void setHZoom(double zoom)
    {
        hZoom.setValue(zoom, nullptr); 
    }

    void setHScroll(double scroll)
    {
        hScroll.setValue(scroll, nullptr);
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

    int getMidiNote() const 
    {
        return midiNote; 
    }

    ADSRParameters getADSR() const
    {
        return adsr; 
    }

    juce::Range<double> getTotalRange()
    {
        return totalRange; 
    }

    bool sampleExists()
    {
        if (audioFile.get())
            return true;
        else
            return false; 
    }

    int getPitchShift()
    {
        return pitchShift; 
    }

    double getHZoom()
    {
        return hZoom; 
    }

    double getHScroll()
    {
        return hScroll; 
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
                listenerList.call([&](Listener& l) { l.fileChanged(audioFile); });
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
            else if (property == IDs::adsr)
            {
                adsr.forceUpdateOfCachedValue(); 
                listenerList.call([&](Listener& l) { l.adsrChanged(adsr); });
            }
            else if (property == IDs::pitchShift)
            {
                pitchShift.forceUpdateOfCachedValue(); 
                listenerList.call([&](Listener& l) { l.pitchShiftChanged(pitchShift); });
            }
            else if (property == IDs::hZoom)
            {
                hZoom.forceUpdateOfCachedValue(); 
            }
            else if (property == IDs::hScroll)
            {
                hScroll.forceUpdateOfCachedValue(); 
            }
        }
    }

    juce::CachedValue<int> id; 
    juce::CachedValue<juce::String> name;
    juce::CachedValue<std::shared_ptr<juce::File>> audioFile;
    juce::CachedValue<int> midiNote; 
    juce::CachedValue<ADSRParameters> adsr; 
    juce::CachedValue<int> pitchShift; 
    juce::CachedValue<bool> isActiveSample; 
    juce::CachedValue<juce::Range<double>> totalRange;
    juce::CachedValue<double> hZoom;
    juce::CachedValue<double> hScroll;

    juce::ListenerList<Listener> listenerList;
    juce::AudioFormatManager formatManager; 
};


class DataModel : public Model
{
public:
    class Listener
    {
    public:
        virtual ~Listener() noexcept = default;
        virtual void activeSampleChanged(SampleModel&) {}
        virtual void fileChanged(SampleModel&) {}
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
            sampleModel.setTotalRange(juce::Range<double>(0, 0));
            sampleModel.setHZoom(0); 
            sampleModel.setHScroll(0); 

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
            if (property == IDs::isActive && treeChanged != activeSample && static_cast<bool>((treeChanged[IDs::isActive])) == true)
            {
                if (activeSample.isValid())
                    activeSample.setProperty(IDs::isActive, false, nullptr); 
                        
                activeSample = treeChanged;
                listenerList.call([&](Listener& l) { l.activeSampleChanged(SampleModel(treeChanged)); });
            }
            else if (property == IDs::file)
            {
                listenerList.call([&](Listener& l) { l.fileChanged(SampleModel(treeChanged)); });
            }
            
        }
        return; 
    }

    juce::ValueTree activeSample; 
    juce::ListenerList<Listener> listenerList;
};


struct PlaybackData
{
    float playbackPosition; 
    float midiNote; 
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
