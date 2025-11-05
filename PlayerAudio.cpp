#include <JuceHeader.h>
#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    transportSource.setLooping(false);

  
    deviceManager.initialise(0, 2, nullptr, true);
    resamplerSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false, 2);
    audioSourcePlayer.setSource(resamplerSource.get());
    deviceManager.addAudioCallback(&audioSourcePlayer);
}

PlayerAudio::~PlayerAudio()
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    resamplerSource.reset();
    readerSource.reset();
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
}

void PlayerAudio::clearSources()
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
    resamplerSource->setResamplingRatio(1.0);
    currentFile = {};
    currentPlaylistIndex = -1;
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (!file.existsAsFile()) return false;

    if (auto* reader = formatManager.createReaderFor(file))
    {
        clearSources();
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        currentFile = file;
        transportSource.setPosition(0.0);
        sendChangeMessage();
        return true;
    }
    return false;
}

void PlayerAudio::play()
{
    if (transportSource.getCurrentPosition() >= getLength())
        transportSource.setPosition(0.0);
    transportSource.start();
    sendChangeMessage();
}

void PlayerAudio::pause() { transportSource.stop(); sendChangeMessage(); }
void PlayerAudio::stop() { transportSource.stop(); transportSource.setPosition(0.0); sendChangeMessage(); }
void PlayerAudio::restart() { transportSource.setPosition(0.0); transportSource.start(); sendChangeMessage(); }

void PlayerAudio::setPosition(double seconds)
{
    transportSource.setPosition(seconds);
    sendChangeMessage();
}

double PlayerAudio::getPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLength() const { return transportSource.getLengthInSeconds(); }
bool PlayerAudio::isPlaying() const { return transportSource.isPlaying(); }

void PlayerAudio::setGain(float g) { transportSource.setGain(g); sendChangeMessage(); }
float PlayerAudio::getGain() const { return transportSource.getGain(); }

void PlayerAudio::setLooping(bool shouldLoop)
{
    loopEnabled = shouldLoop;
    if (readerSource) readerSource->setLooping(shouldLoop);
    transportSource.setLooping(shouldLoop);
    sendChangeMessage();
}

bool PlayerAudio::isLooping() const { return loopEnabled; }

void PlayerAudio::setSpeed(double ratio)
{
    speedRatio = ratio;
    if (resamplerSource) resamplerSource->setResamplingRatio((float)speedRatio);
    sendChangeMessage();
}

double PlayerAudio::getSpeed() const { return speedRatio; }

void PlayerAudio::jumpSeconds(double seconds)
{
    double newPos = getPosition() + seconds;
    if (newPos < 0.0) newPos = 0.0;
    if (newPos > getLength()) newPos = getLength();
    setPosition(newPos);
}

void PlayerAudio::addToPlaylist(const juce::File& f) { if (f.existsAsFile()) playlist.push_back(f); }

void PlayerAudio::playIndex(int idx)
{
    if (idx < 0 || idx >= (int)playlist.size()) return;
    if (loadFile(playlist[idx])) { currentPlaylistIndex = idx; play(); }
}

int PlayerAudio::getPlaylistSize() const { return (int)playlist.size(); }
juce::File PlayerAudio::getPlaylistFile(int idx) const { return (idx>=0 && idx < (int)playlist.size()) ? playlist[idx] : juce::File(); }

void PlayerAudio::addMarker(double seconds) { if (seconds >= 0.0 && seconds <= getLength()) markers.push_back(seconds); }
const std::vector<double>& PlayerAudio::getMarkers() const { return markers; }

void PlayerAudio::saveSession(const juce::File& settingsFile)
{
    juce::PropertiesFile props(settingsFile, {});
    props.setValue("lastFile", currentFile.getFullPathName());
    props.setValue("speed", getSpeed());
    props.setValue("gain", getGain());
    props.setValue("loop", isLooping());
    props.saveIfNeeded();
}

void PlayerAudio::restoreSession(const juce::File& settingsFile)
{
    if (!settingsFile.existsAsFile()) return;
    juce::PropertiesFile props(settingsFile, {});
    juce::String last = props.getValue("lastFile", "");
    if (last.isNotEmpty())
    {
        juce::File f(last);
        if (f.existsAsFile()) loadFile(f);
    }
    setGain((float)props.getDoubleValue("gain", 0.5));
    setLooping(props.getBoolValue("loop", false));
    setSpeed(props.getDoubleValue("speed", 1.0));
}

juce::String PlayerAudio::getCurrentFileName() const { return currentFile.existsAsFile() ? currentFile.getFileName() : juce::String(); }
double PlayerAudio::getCurrentFileLengthSeconds() const { return getLength(); }
