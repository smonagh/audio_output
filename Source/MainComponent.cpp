/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    freqSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    freqSlider.setRange(50.0, 500.0);
    freqSlider.addListener(this);
    freqSlider.setValue(200);
    freqSlider.setTextValueSuffix("hz");
    addAndMakeVisible(freqSlider);

    freqLabel.setText("Frequency", dontSendNotification);
    freqLabel.attachToComponent(&freqSlider, true);

    ampSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    ampSlider.setRange(0.0, 1.0);
    ampSlider.addListener(this);
    ampSlider.setValue(0.25);
    addAndMakeVisible(ampSlider);

    ampLabel.setText("Volume", dontSendNotification);
    ampLabel.attachToComponent(&ampSlider, true);



    addAndMakeVisible(ampSlider);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    frequency = freqSlider.getValue();
    phase = 0.0;
    waveTableSize = 2048;
    amplitude = 0.25;
    currentSampleRate = sampleRate;

    // This computes one cycle of a sine wave
    for (int i = 0; i < waveTableSize; i++){
      waveTable.insert(i, sin(2.0 * double_Pi * i / waveTableSize));
    }
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
  float* const leftSpeaker = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
  float* const rightSpeaker = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

  for (int sample = 0; sample < bufferToFill.numSamples; ++sample){
    leftSpeaker[sample] = waveTable[(int)phase] * amplitude;
    rightSpeaker[sample] = waveTable[(int)phase] * amplitude;
    updateFrequency();
    updateAmplitude();
  }

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    const int labelSpace = 100;
    freqSlider.setBounds(labelSpace, 20, getWidth() - 100, 20);
    ampSlider.setBounds(labelSpace, 50, getWidth() - 100, 20);
}

void MainComponent::sliderValueChanged(Slider* slider){
  if (slider == &freqSlider){
    frequency = freqSlider.getValue();
    std::cout << frequency << std::endl;
  }
}

void MainComponent::updateFrequency(){
  increment = frequency * waveTableSize / currentSampleRate;
  phase = fmod(phase + increment, waveTableSize);
}

void MainComponent::updateAmplitude(){
  amplitude = ampSlider.getValue();
}
