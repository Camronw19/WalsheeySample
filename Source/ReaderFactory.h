/*
  ==============================================================================

    ReaderFactory.h
    Created: 29 Dec 2023 3:13:02pm
    Author:  camro

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>


inline std::unique_ptr<juce::AudioFormatReader> makeAudioFormatReader(juce::AudioFormatManager& manager,
    const void* sampleData,
    size_t dataSize)
{
    return std::unique_ptr<juce::AudioFormatReader>(manager.createReaderFor(std::make_unique<juce::MemoryInputStream>(sampleData,
        dataSize,
        false)));
}

inline std::unique_ptr<juce::AudioFormatReader> makeAudioFormatReader(juce::AudioFormatManager& manager,
    const juce::File& file)
{
    return std::unique_ptr<juce::AudioFormatReader>(manager.createReaderFor(file));
}

class AudioFormatReaderFactory
{
public:
    AudioFormatReaderFactory() = default;
    AudioFormatReaderFactory(const AudioFormatReaderFactory&) = default;
    AudioFormatReaderFactory(AudioFormatReaderFactory&&) = default;
    AudioFormatReaderFactory& operator= (const AudioFormatReaderFactory&) = default;
    AudioFormatReaderFactory& operator= (AudioFormatReaderFactory&&) = default;

    virtual ~AudioFormatReaderFactory() noexcept = default;

    virtual std::unique_ptr<juce::AudioFormatReader> make(juce::AudioFormatManager&) const = 0;
    virtual std::unique_ptr<AudioFormatReaderFactory> clone() const = 0;
};

//==============================================================================
class MemoryAudioFormatReaderFactory : public AudioFormatReaderFactory
{
public:
    MemoryAudioFormatReaderFactory(const void* sampleDataIn, size_t dataSizeIn)
        : sampleData(sampleDataIn),
        dataSize(dataSizeIn)
    {
    }

    std::unique_ptr<juce::AudioFormatReader> make(juce::AudioFormatManager& manager) const override
    {
        return makeAudioFormatReader(manager, sampleData, dataSize);
    }

    std::unique_ptr<AudioFormatReaderFactory> clone() const override
    {
        return std::unique_ptr<AudioFormatReaderFactory>(new MemoryAudioFormatReaderFactory(*this));
    }

private:
    const void* sampleData;
    size_t dataSize;
};

//==============================================================================
class FileAudioFormatReaderFactory : public AudioFormatReaderFactory
{
public:
    explicit FileAudioFormatReaderFactory(juce::File fileIn)
        : file(std::move(fileIn))
    {
    }

    std::unique_ptr<juce::AudioFormatReader> make(juce::AudioFormatManager& manager) const override
    {
        return makeAudioFormatReader(manager, file);
    }

    std::unique_ptr<AudioFormatReaderFactory> clone() const override
    {
        return std::unique_ptr<AudioFormatReaderFactory>(new FileAudioFormatReaderFactory(*this));
    }

private:
    juce::File file;
};
