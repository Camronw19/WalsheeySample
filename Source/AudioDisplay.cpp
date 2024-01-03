/*
  ==============================================================================

    AudioDisplay.cpp
    Created: 20 Dec 2023 1:49:50pm
    Author:  camro

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioDisplay.h"

//==============================================================================
AudioDisplay::AudioDisplay()
    : mThumbnailCache(5), mVerticalZoom(1.0f), mHorisontalZoom(0), mHorisontalScroll(0), mShowChan1(true), mShowChan2(true),
    mThumbnail(1024, mFormatManager, mThumbnailCache)
{
    mFormatManager.registerBasicFormats();
    mThumbnail.addChangeListener(this);


}

AudioDisplay::~AudioDisplay()
{
}

void AudioDisplay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    juce::Rectangle<int> thumbnailBounds(bounds);

    if (mThumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void AudioDisplay::paintIfNoFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void AudioDisplay::paintIfFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colour::fromRGB(240, 240, 240));
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colour::fromRGB(0, 102, 204));

    double startTime = mThumbnail.getTotalLength() * mHorisontalScroll;
    double endTime = startTime + (mThumbnail.getTotalLength() * (1 - mHorisontalZoom));

    if (mShowChan1 && !mShowChan2)
    {
        mThumbnail.drawChannel(g, thumbnailBounds, startTime, endTime, 0, mVerticalZoom);
    }
    else if (!mShowChan1 && mShowChan2)
    {
        mThumbnail.drawChannel(g, thumbnailBounds, startTime, endTime, 1, mVerticalZoom);
    }
    else
    {
        mThumbnail.drawChannels(g, thumbnailBounds, startTime, endTime, mVerticalZoom);
    }
}

void AudioDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // If the Thumbnail object has changed
    if (source == &mThumbnail)
    {
        thumbnailChanged();
    }
}

void AudioDisplay::thumbnailChanged()
{
    repaint();
}

void AudioDisplay::setThumbnailSource(const juce::File& inputSource)
{
    mThumbnail.setSource(new juce::FileInputSource(inputSource));
}

void AudioDisplay::setVerticalZoom(float vZoom)
{
    mVerticalZoom = vZoom;
    repaint();
}

void AudioDisplay::setHorisontalZoom(float hZoom)
{
    mHorisontalZoom = hZoom;
    repaint();
}

void AudioDisplay::setHorisontalScroll(float hScroll)
{
    mHorisontalScroll = hScroll;
    repaint();
}


void AudioDisplay::setShowChannels(bool chan1, bool chan2)
{
    mShowChan1 = chan1;
    mShowChan2 = chan2;
    repaint();
}

std::pair<bool, bool> AudioDisplay::getShowChannels()
{
    return std::pair<bool, bool>(mShowChan1, mShowChan2);
}

void AudioDisplay::resized()
{

}

