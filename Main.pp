#include <JuceHeader.h>
#include "MainComponent.h"
joj
class AudioPlayerApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override       { return "Audio Player - Part 2"; }
    const juce::String getApplicationVersion() override    { return "2.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override { mainWindow = nullptr; }
    void systemRequestedQuit() override { quit(); }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name, juce::Colours::darkgrey, DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            centreWithSize(900, 360);
            setVisible(true);
        }
        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(AudioPlayerApplication)
