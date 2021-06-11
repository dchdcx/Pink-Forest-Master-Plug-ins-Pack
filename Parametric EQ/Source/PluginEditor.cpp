#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

ParametricEQAudioProcessorEditor::ParametricEQAudioProcessorEditor (ParametricEQAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    const Array<AudioProcessorParameter*> parameters = processor.getParameters();
    int comboBoxCounter = 0;

    int editorHeight = 2 * editorMargin;
    for (int i = 0; i < parameters.size(); ++i) {
        if (const AudioProcessorParameterWithID* parameter =
                dynamic_cast<AudioProcessorParameterWithID*> (parameters[i])) {

            if (processor.parameters.parameterTypes[i] == "Slider") {
                Slider* aSlider;
                sliders.add (aSlider = new Slider());
                aSlider->setTextValueSuffix (parameter->label);
                aSlider->setTextBoxStyle (Slider::TextBoxLeft,
                                          false,
                                          sliderTextEntryBoxWidth,
                                          sliderTextEntryBoxHeight);

                SliderAttachment* aSliderAttachment;
                sliderAttachments.add (aSliderAttachment =
                    new SliderAttachment (processor.parameters.apvts, parameter->paramID, *aSlider));

                components.add (aSlider);
                editorHeight += sliderHeight;
            }

            //======================================

            else if (processor.parameters.parameterTypes[i] == "ToggleButton") {
                ToggleButton* aButton;
                toggles.add (aButton = new ToggleButton());
                aButton->setToggleState (parameter->getDefaultValue(), dontSendNotification);

                ButtonAttachment* aButtonAttachment;
                buttonAttachments.add (aButtonAttachment =
                    new ButtonAttachment (processor.parameters.apvts, parameter->paramID, *aButton));

                components.add (aButton);
                editorHeight += buttonHeight;
            }

            //======================================

            else if (processor.parameters.parameterTypes[i] == "ComboBox") {
                ComboBox* aComboBox;
                comboBoxes.add (aComboBox = new ComboBox());
                aComboBox->setEditableText (false);
                aComboBox->setJustificationType (Justification::left);
                aComboBox->addItemList (processor.parameters.comboBoxItemLists[comboBoxCounter++], 1);

                ComboBoxAttachment* aComboBoxAttachment;
                comboBoxAttachments.add (aComboBoxAttachment =
                    new ComboBoxAttachment (processor.parameters.apvts, parameter->paramID, *aComboBox));

                components.add (aComboBox);
                editorHeight += comboBoxHeight;
            }

            //======================================

            Label* aLabel;
            labels.add (aLabel = new Label (parameter->name, parameter->name));
            aLabel->attachToComponent (components.getLast(), true);
            addAndMakeVisible (aLabel);

            components.getLast()->setName (parameter->name);
            components.getLast()->setComponentID (parameter->paramID);
            addAndMakeVisible (components.getLast());
        }
    }

    addAndMakeVisible (&bandwidthLabel);
    editorHeight += 20;

    //======================================

    editorHeight += components.size() * editorPadding;
    setSize (editorWidth, editorHeight);
    startTimer (50);
}

ParametricEQAudioProcessorEditor::~ParametricEQAudioProcessorEditor()
{
}

//==============================================================================

void ParametricEQAudioProcessorEditor::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (editorMargin);
    r = r.removeFromRight (r.getWidth() - labelWidth);

    for (int i = 0; i < components.size(); ++i) {
        if (Slider* aSlider = dynamic_cast<Slider*> (components[i]))
            components[i]->setBounds (r.removeFromTop (sliderHeight));

        if (ToggleButton* aButton = dynamic_cast<ToggleButton*> (components[i]))
            components[i]->setBounds (r.removeFromTop (buttonHeight));

        if (ComboBox* aComboBox = dynamic_cast<ComboBox*> (components[i]))
            components[i]->setBounds (r.removeFromTop (comboBoxHeight));

        r = r.removeFromBottom (r.getHeight() - editorPadding);
    }

    bandwidthLabel.setBounds (0, getBottom() - 20, getWidth(), 20);
}

//==============================================================================

void ParametricEQAudioProcessorEditor::timerCallback()
{
    updateUIcomponents();
}

void ParametricEQAudioProcessorEditor::updateUIcomponents()
{
    String bandwidthText = String::formatted ("Bandwidth: %.1fHz",
                                              processor.paramFrequency.getTargetValue() /
                                              processor.paramQfactor.getTargetValue());

    bandwidthLabel.setText (bandwidthText, dontSendNotification);

    //======================================

    bool filterTypeDoesNotHaveQfactor =
        processor.paramFilterType.getTargetValue() == processor.filterTypeLowPass ||
        processor.paramFilterType.getTargetValue() == processor.filterTypeHighPass ||
        processor.paramFilterType.getTargetValue() == processor.filterTypeLowShelf ||
        processor.paramFilterType.getTargetValue() == processor.filterTypeHighShelf;
    bool filterTypeDoesNotHaveGain =
        processor.paramFilterType.getTargetValue() == processor.filterTypeLowPass ||
        processor.paramFilterType.getTargetValue() == processor.filterTypeHighPass ||
        processor.paramFilterType.getTargetValue() == processor.filterTypeBandPass ||
        processor.paramFilterType.getTargetValue() == processor.filterTypeBandStop;

    if (filterTypeDoesNotHaveQfactor) {
        findChildWithID (processor.paramQfactor.paramID)->setEnabled (false);
        bandwidthLabel.setVisible (false);
    } else {
        findChildWithID (processor.paramQfactor.paramID)->setEnabled (true);
        bandwidthLabel.setVisible (true);
    }

    if (filterTypeDoesNotHaveGain)
        findChildWithID (processor.paramGain.paramID)->setEnabled (false);
    else
        findChildWithID (processor.paramGain.paramID)->setEnabled (true);
}

//==============================================================================
