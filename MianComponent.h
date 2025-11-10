#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class MainComponent : public juce::Component,
                      public juce::Button::Listener,
                      public juce::Slider::Listener,
                      public juce::Timer,
                      public juce::ChangeListener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    PlayerAudio audio;

    // Buttons
    juce::TextButton loadButton{ "Load" };
    juce::TextButton playPauseButton{ "Play/Pause" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton startButton{ "|◄ Start" };
    juce::TextButton endButton{ "End ►|" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton jumpBack{ "-10s" };
    juce::TextButton jumpForward{ "+10s" };
    juce::TextButton addMarkerButton{ "Add Marker" };

    // Sliders
    juce::Slider volumeSlider;   // للتحكم  بالصوت
    juce::Slider positionSlider; // لموضع  التشغيل
    juce::Slider speedSlider;    // لسرعة  التشغيل

    // Labels
    juce::Label fileLabel{ "file", "No file loaded" };
    juce::Label timeLabel{ "time", "00:00 / 00:00" };
    juce::Label markersLabel{ "markers", "" };

    // Playlist box
    juce::ListBox playlistBox;
    juce::StringArray playlistItems;

    juce::File sessionFile;
    std::unique_ptr<juce::FileChooser> fileChooser;

    void updateUI();
    void refreshPlaylistItems();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
