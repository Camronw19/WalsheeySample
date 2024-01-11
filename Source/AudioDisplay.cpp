/*
  ==============================================================================

    AudioDisplay.cpp
    Created: 20 Dec 2023 1:49:50pm
    Author:  camro

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioDisplay.h"


PlaybackPositionOverlay::PlaybackPositionOverlay(const DataModel& dm, Providor providorIn)
    :dataModel(dm), providor(std::move(providorIn))
{
    dataModel.addListener(*this); 
    startTimer(16);
}

void PlaybackPositionOverlay::paint(juce::Graphics& g)
{
    if (activeSample != nullptr)
    {
        g.setColour(juce::Colours::red);

        auto xPos = timeToXPosition(); 
        juce::Path playhead; 
        playhead.addTriangle(juce::Point<float>(xPos - 10, 0), juce::Point<float>(xPos + 10, 0), juce::Point<float>(xPos, 10)); 
        g.fillPath(playhead); 
        g.drawVerticalLine(timeToXPosition(), 0, static_cast<float>(getHeight())); 
    }
}

void PlaybackPositionOverlay::timerCallback()
{
    repaint(); 
}

void PlaybackPositionOverlay::activeSampleChanged(SampleModel& sm)
{
    if (activeSample != nullptr)
        activeSample->removeListener(*this);

    activeSample = std::make_unique<SampleModel>(sm.getState());

    if (activeSample != nullptr)
        activeSample->addListener(*this);
}

double PlaybackPositionOverlay::timeToXPosition()
{
    auto time = providor();  
    return (time - activeSample->getVisibleRange().getStart()) * getWidth()
        / activeSample->getVisibleRange().getLength();
}

//==============================================================================
AudioDisplay::AudioDisplay()
    : mThumbnailCache(5), mVerticalZoom(1.0f), mHorizontalZoom(0), mHorizontalScroll(0), mShowChan1(true), mShowChan2(true),
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
    g.setColour(AppColors::backgroundColour);
    g.fillRect(thumbnailBounds);
    g.setColour(AppColors::accentColour);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void AudioDisplay::paintIfFileLoaded(juce::Graphics& g, juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(AppColors::backgroundColour);
    g.fillRect(thumbnailBounds);

    g.setColour(AppColors::accentColour);


    double startTime = mThumbnail.getTotalLength() * mHorizontalScroll;
    double endTime = startTime + (mThumbnail.getTotalLength() * (1 - mHorizontalZoom));

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

void AudioDisplay::setHorizontalZoom(float hZoom)
{
    mHorizontalZoom = hZoom;
    repaint();
}

void AudioDisplay::setHorizontalScroll(float hScroll)
{
    mHorizontalScroll = hScroll;
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

