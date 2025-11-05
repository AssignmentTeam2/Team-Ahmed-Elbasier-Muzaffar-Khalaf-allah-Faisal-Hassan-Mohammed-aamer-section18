#pragma once
#include <JuceHeader.h>
#include <vector>

class PlayerAudio : private juce::ChangeBroadcaster
{
public:
    PlayerAudio();
    ~PlayerAudio();

    bool loadFile(const juce::File& file);
    void play();
    void pause();
    void stop();
    void restart();
    void setPosition(double seconds);
    double getPosition() const;
    double getLength() const;
    bool isPlaying() const;

    void setGain(float g);
    float getGain() const;

    void setLooping(bool shouldLoop);
    bool isLooping() const;

    void setSpeed(double ratio);
    double getSpeed() const;

    void jumpSeconds(double seconds);

    void addToPlaylist(const juce::File& f);
    void playIndex(int idx);
    int getPlaylistSize() const;
    juce::File getPlaylistFile(int idx) const;

    void addMarker(double seconds);
    const std::vector<double>& getMarkers() const;

    void saveSession(const juce::File& settingsFile);
    void restoreSession(const juce::File& settingsFile);

    juce::String getCurrentFileName() const;
    double getCurrentFileLengthSeconds() const;

    using ChangeBroadcaster::addChangeListener;
    using ChangeBroadcaster::removeChangeListener;

private:
    juce::AudioFormatManager formatManager;
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplerSource;

    juce::File currentFile;
    double speedRatio = 1.0;
    bool loopEnabled = false;

    std::vector<juce::File> playlist;
    int currentPlaylistIndex = -1;

    std::vector<double> markers;

    void clearSources();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
