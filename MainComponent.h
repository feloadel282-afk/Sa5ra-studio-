#pragma once
#include <JuceHeader.h>
#include <array>

class MainComponent final : public juce::AudioAppComponent,
                            private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    static constexpr int trackCount = 4;

    struct Track
    {
        juce::AudioBuffer<float> audio;
        juce::String fileName = "Empty track";
        juce::int64 position = 0;
        float gain = 0.75f;
        bool loaded = false;
        std::unique_ptr<juce::TextButton> loadButton;
        std::unique_ptr<juce::Label> nameLabel;
        std::unique_ptr<juce::Slider> gainSlider;
    };

    std::array<Track, trackCount> tracks;
    juce::AudioFormatManager formatManager;
    juce::TextButton playButton { "PLAY" };
    juce::TextButton stopButton { "STOP" };
    juce::Label titleLabel;
    juce::Label statusLabel;
    std::unique_ptr<juce::FileChooser> chooser;
    juce::CriticalSection audioLock;
    bool isPlaying = false;
    double currentSampleRate = 44100.0;
    juce::int64 globalSamples = 0;

    void loadTrack(int index);
    void startPlayback();
    void stopPlayback();
    void timerCallback() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
