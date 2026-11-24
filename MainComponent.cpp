#include "MainComponent.h"

MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    setSize(1100, 720);
    setAudioChannels(0, 2);

    titleLabel.setText("SA5RA STUDIO  |  SESSION 01", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(25.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe8edf5));
    addAndMakeVisible(titleLabel);

    playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff167d65));
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff8c3540));
    playButton.onClick = [this] { startPlayback(); };
    stopButton.onClick = [this] { stopPlayback(); };
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);

    statusLabel.setText("Ready  |  Load WAV/AIFF/FLAC files into tracks", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colour(0xffaebbd0));
    addAndMakeVisible(statusLabel);

    for (int i = 0; i < trackCount; ++i)
    {
        auto& t = tracks[(size_t)i];
        t.loadButton = std::make_unique<juce::TextButton>("LOAD AUDIO");
        t.loadButton->onClick = [this, i] { loadTrack(i); };
        addAndMakeVisible(*t.loadButton);

        t.nameLabel = std::make_unique<juce::Label>();
        t.nameLabel->setText("Track " + juce::String(i + 1) + "  |  Empty", juce::dontSendNotification);
        t.nameLabel->setColour(juce::Label::textColourId, juce::Colour(0xffe8edf5));
        t.nameLabel->setFont(juce::Font(15.0f, juce::Font::bold));
        addAndMakeVisible(*t.nameLabel);

        t.gainSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                                                       juce::Slider::TextBoxRight);
        t.gainSlider->setRange(0.0, 1.5, 0.01);
        t.gainSlider->setValue(t.gain);
        t.gainSlider->setTextValueSuffix(" gain");
        t.gainSlider->onValueChange = [this, i]
        {
            const juce::ScopedLock lock(audioLock);
            tracks[(size_t)i].gain = (float)tracks[(size_t)i].gainSlider->getValue();
        };
        addAndMakeVisible(*t.gainSlider);
    }

    startTimerHz(10);
}

MainComponent::~MainComponent()
{
    stopTimer();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int, double sampleRate)
{
    const juce::ScopedLock lock(audioLock);
    currentSampleRate = sampleRate;
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    info.clearActiveBufferRegion();
    const juce::ScopedLock lock(audioLock);

    if (!isPlaying)
        return;

    auto* output = info.buffer;
    const int outputChannels = output->getNumChannels();

    for (auto& track : tracks)
    {
        if (!track.loaded || track.audio.getNumSamples() == 0)
            continue;

        const int available = (int)juce::jmin<juce::int64>(
            info.numSamples, track.audio.getNumSamples() - track.position);

        if (available <= 0)
            continue;

        const int sourceChannels = track.audio.getNumChannels();
        for (int ch = 0; ch < outputChannels; ++ch)
        {
            const int sourceChannel = juce::jmin(ch, sourceChannels - 1);
            output->addFrom(ch, info.startSample,
                            track.audio, sourceChannel,
                            (int)track.position, available, track.gain);
        }
        track.position += available;
    }

    globalSamples += info.numSamples;
}

void MainComponent::releaseResources() {}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff171c25));
    g.setColour(juce::Colour(0xff242d3a));
    g.fillRoundedRectangle(22.0f, 112.0f, (float)getWidth() - 44.0f,
                           (float)getHeight() - 150.0f, 12.0f);

    g.setColour(juce::Colour(0xff9eacc0));
    g.setFont(13.0f);
    g.drawText("AUDIO TRACKS", 42, 126, 250, 22, juce::Justification::left);
    g.drawText("LEVEL", getWidth() - 270, 126, 180, 22, juce::Justification::left);
    g.setColour(juce::Colour(0xff394657));
    for (int i = 0; i < trackCount; ++i)
    {
        const float y = 160.0f + i * 112.0f;
        g.drawRoundedRectangle(36.0f, y, (float)getWidth() - 72.0f, 96.0f, 8.0f, 1.0f);
    }
}

void MainComponent::resized()
{
    titleLabel.setBounds(28, 22, 650, 42);
    playButton.setBounds(28, 72, 100, 30);
    stopButton.setBounds(138, 72, 100, 30);
    statusLabel.setBounds(260, 72, getWidth() - 290, 30);

    for (int i = 0; i < trackCount; ++i)
    {
        const int y = 174 + i * 112;
        auto& t = tracks[(size_t)i];
        t.nameLabel->setBounds(52, y, 400, 28);
        t.loadButton->setBounds(52, y + 38, 130, 30);
        t.gainSlider->setBounds(getWidth() - 360, y + 28, 270, 38);
    }
}

void MainComponent::loadTrack(int index)
{
    chooser = std::make_unique<juce::FileChooser>(
        "Choose audio file",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.wav;*.aiff;*.aif;*.flac;*.ogg");

    chooser->launchAsync(juce::FileBrowserComponent::openMode
                         | juce::FileBrowserComponent::canSelectFiles,
        [this, index](const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();
            if (!file.existsAsFile())
                return;

            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
            if (reader == nullptr)
            {
                statusLabel.setText("Could not read this audio file.", juce::dontSendNotification);
                return;
            }

            const auto length = (juce::int64)reader->lengthInSamples;
            if (length <= 0 || length > (juce::int64)reader->sampleRate * 60 * 60 * 3)
            {
                statusLabel.setText("File is empty or longer than the 3-hour prototype limit.", juce::dontSendNotification);
                return;
            }

            juce::AudioBuffer<float> loaded((int)reader->numChannels, (int)length);
            reader->read(&loaded, 0, (int)length, 0, true, true);

            {
                const juce::ScopedLock lock(audioLock);
                auto& t = tracks[(size_t)index];
                t.audio = std::move(loaded);
                t.position = 0;
                t.loaded = true;
                t.fileName = file.getFileName();
            }

            tracks[(size_t)index].nameLabel->setText(
                "Track " + juce::String(index + 1) + "  |  " + file.getFileName(),
                juce::dontSendNotification);
            statusLabel.setText("Loaded: " + file.getFileName(), juce::dontSendNotification);
        });
}

void MainComponent::startPlayback()
{
    const juce::ScopedLock lock(audioLock);
    bool hasAudio = false;
    for (auto& t : tracks)
    {
        if (t.loaded)
        {
            hasAudio = true;
            if (t.position >= t.audio.getNumSamples())
                t.position = 0;
        }
    }

    if (!hasAudio)
    {
        statusLabel.setText("Load at least one audio file first.", juce::dontSendNotification);
        return;
    }

    isPlaying = true;
    statusLabel.setText("Playing  |  Prototype audio engine", juce::dontSendNotification);
}

void MainComponent::stopPlayback()
{
    const juce::ScopedLock lock(audioLock);
    isPlaying = false;
    for (auto& t : tracks)
        t.position = 0;
    statusLabel.setText("Stopped  |  Playhead returned to start", juce::dontSendNotification);
}

void MainComponent::timerCallback()
{
    // Reserved for transport clock, meters, and timeline refresh.
}
