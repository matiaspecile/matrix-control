#include "SettingsMidiPage.h"

#include "Core/MIDI/MidiActivityTracker.h"

#include "GUI/Helpers/MidiActivityLedLevels.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    constexpr int kHostMidiDiagnosticsConsoleHeight = 72;
    constexpr int kHostMidiInquiryTraceStageCount = 4;
}

SettingsMidiPage::SettingsMidiPage(Config config)
    : config_(std::move(config))
    , skin_(config_.skin)
{
    jassert(config_.skin != nullptr);

    setOpaque(false);
    buildWidgets();
    wirePopupRefresh();
    updateMidiRouteVisibility();
    if (! config_.isPluginMode)
        refreshPortLists();
    setMonitoringActive(true);
}

SettingsMidiPage::~SettingsMidiPage()
{
    setMonitoringActive(false);
}

int SettingsMidiPage::getDesignHeight() const noexcept
{
    int height = SettingsShellMetrics::midiPageContentHeight(config_.isPluginMode);
    if (config_.isPluginMode && config_.isVst3Mode)
        height += SettingsShellMetrics::kRowGap + kHostMidiDiagnosticsConsoleHeight;
    return height;
}

void SettingsMidiPage::buildWidgets()
{
    keyboardFromLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kKeyboardFromLabel);
    keyboardFromCombo_ = makeCombo(*skin_);
    keyboardFromLed_ = std::make_unique<TSS::Led>(SettingsShellMetrics::kPeakWidth,
                                                  SettingsShellMetrics::kPeakWidth);
    keyboardFromLed_->setSkin(*skin_);
    addAndMakeVisible(*keyboardFromLed_);

    synthFromLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kSynthFromLabel);
    synthFromCombo_ = makeCombo(*skin_);
    synthFromLed_ = std::make_unique<TSS::Led>(SettingsShellMetrics::kPeakWidth,
                                               SettingsShellMetrics::kPeakWidth);
    synthFromLed_->setSkin(*skin_);
    addAndMakeVisible(*synthFromLed_);

    synthToLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kSynthToLabel);
    synthToCombo_ = makeCombo(*skin_);
    synthToLed_ = std::make_unique<TSS::Led>(SettingsShellMetrics::kPeakWidth,
                                             SettingsShellMetrics::kPeakWidth);
    synthToLed_->setSkin(*skin_);
    addAndMakeVisible(*synthToLed_);

    hostMidiRouteLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kMidiHostRouteLabel);
    hostMidiRouteValueField_ = std::make_unique<TSS::ReadOnlyValueField>(*skin_);
    hostMidiRouteValueField_->setText(PluginDisplayNames::Settings::kMidiHostRouteValue);
    addChildComponent(*hostMidiRouteValueField_);

    hostMidiOutLed_ = std::make_unique<TSS::Led>(SettingsShellMetrics::kPeakWidth,
                                                  SettingsShellMetrics::kPeakWidth);
    hostMidiOutLed_->setSkin(*skin_);
    addChildComponent(*hostMidiOutLed_);

    hostMidiInquiryButton_ = std::make_unique<TSS::Button>(
        SettingsShellMetrics::kControlColumnWidth,
        SettingsShellMetrics::kControlHeight,
        TSS::buttonLookFromSkin(*skin_),
        PluginDisplayNames::Settings::kMidiHostInquiryButton);
    hostMidiInquiryButton_->setTooltip(PluginDisplayNames::Settings::kMidiHostInquiryTooltip);
    hostMidiInquiryButton_->onClick = [this]
    {
        if (hostMidiDiagnosticsConsole_ != nullptr)
        {
            displayedInquiryDiagnosticSequences_.fill(0);
            if (config_.activityTrackerProvider != nullptr)
            {
                const auto& activityTracker = config_.activityTrackerProvider();
                for (int i = 0; i < kHostMidiInquiryTraceStageCount; ++i)
                {
                    const auto stage = static_cast<Core::HostMidiInquiryStage>(i);
                    displayedInquiryDiagnosticSequences_[static_cast<std::size_t>(i)] =
                        activityTracker.getHostMidiInquiryDiagnostic(stage).sequence;
                }
            }

            waitingForInquiryTrace_ = true;
            hostMidiDiagnosticsConsole_->setText(
                "Waiting for Device Inquiry SysEx trace...",
                juce::dontSendNotification);
        }

        if (config_.onHostMidiInquiryRequested != nullptr)
            config_.onHostMidiInquiryRequested();
    };
    addChildComponent(*hostMidiInquiryButton_);

    hostMidiDiagnosticsConsole_ = std::make_unique<juce::TextEditor>();
    hostMidiDiagnosticsConsole_->setMultiLine(true, false);
    hostMidiDiagnosticsConsole_->setReadOnly(true);
    hostMidiDiagnosticsConsole_->setCaretVisible(false);
    hostMidiDiagnosticsConsole_->setPopupMenuEnabled(false);
    hostMidiDiagnosticsConsole_->setScrollbarsShown(true);
    hostMidiDiagnosticsConsole_->setFont(juce::Font(
        juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::plain));
    hostMidiDiagnosticsConsole_->setColour(juce::TextEditor::backgroundColourId,
                                            juce::Colour(0xff101510));
    hostMidiDiagnosticsConsole_->setColour(juce::TextEditor::textColourId,
                                            juce::Colour(0xff9ee7a3));
    hostMidiDiagnosticsConsole_->setColour(juce::TextEditor::outlineColourId,
                                            juce::Colour(0xff465247));
    hostMidiDiagnosticsConsole_->setColour(juce::TextEditor::focusedOutlineColourId,
                                            juce::Colour(0xff465247));
    hostMidiDiagnosticsConsole_->setText(
        "Click Check Connection to capture the full SysEx trace.",
        juce::dontSendNotification);
    addChildComponent(*hostMidiDiagnosticsConsole_);
}

void SettingsMidiPage::wirePopupRefresh()
{
    const auto refreshBeforeOpen = [this]
    {
        if (config_.onPortListsRefreshRequested)
            config_.onPortListsRefreshRequested();
        else
            refreshPortLists();
    };

    if (keyboardFromCombo_ != nullptr)
        keyboardFromCombo_->onAboutToShowPopup = refreshBeforeOpen;
    synthFromCombo_->onAboutToShowPopup = refreshBeforeOpen;
    synthToCombo_->onAboutToShowPopup = refreshBeforeOpen;
}

void SettingsMidiPage::updateMidiRouteVisibility()
{
    const bool showKeyboard = SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode);
    const bool showDirectSynthPorts = ! config_.isPluginMode;

    keyboardFromLabel_->setVisible(showKeyboard);
    keyboardFromCombo_->setVisible(showKeyboard);
    keyboardFromLed_->setVisible(showKeyboard);

    synthFromLabel_->setVisible(showDirectSynthPorts);
    synthFromCombo_->setVisible(showDirectSynthPorts);
    synthFromLed_->setVisible(showDirectSynthPorts);
    synthToLabel_->setVisible(showDirectSynthPorts);
    synthToCombo_->setVisible(showDirectSynthPorts);
    synthToLed_->setVisible(showDirectSynthPorts);

    hostMidiRouteLabel_->setVisible(config_.isPluginMode);
    hostMidiRouteValueField_->setVisible(config_.isPluginMode);
    hostMidiOutLed_->setVisible(config_.isPluginMode && config_.isVst3Mode);
    hostMidiInquiryButton_->setVisible(config_.isPluginMode && config_.isVst3Mode);
    if (hostMidiDiagnosticsConsole_ != nullptr)
        hostMidiDiagnosticsConsole_->setVisible(config_.isPluginMode && config_.isVst3Mode);
}

void SettingsMidiPage::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    keyboardFromLabel_->setLook(TSS::darkPanelLabelLookFromSkin(skin));
    keyboardFromCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    keyboardFromCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    keyboardFromLed_->setSkin(skin);
    synthFromLabel_->setLook(TSS::darkPanelLabelLookFromSkin(skin));
    synthFromCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    synthFromCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    synthFromLed_->setSkin(skin);
    synthToLabel_->setLook(TSS::darkPanelLabelLookFromSkin(skin));
    synthToCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    synthToCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    synthToLed_->setSkin(skin);
    hostMidiRouteLabel_->setLook(TSS::darkPanelLabelLookFromSkin(skin));
    hostMidiRouteValueField_->setSkin(skin);
    hostMidiOutLed_->setSkin(skin);
    hostMidiInquiryButton_->setLook(TSS::buttonLookFromSkin(skin));
}

void SettingsMidiPage::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
}

void SettingsMidiPage::setVisible(bool shouldBeVisible)
{
    Component::setVisible(shouldBeVisible);
    setMonitoringActive(shouldBeVisible);
}

void SettingsMidiPage::setMonitoringActive(bool shouldBeActive)
{
    if (shouldBeActive == monitoringActive_)
        return;

    monitoringActive_ = shouldBeActive;
    if (monitoringActive_)
        startTimerHz(30);
    else
        stopTimer();
}

void SettingsMidiPage::timerCallback()
{
    if (config_.activityTrackerProvider == nullptr)
        return;

    const auto& activityTracker = config_.activityTrackerProvider();
    TSS::MidiActivityLedLevels::apply(activityTracker,
                                      *keyboardFromLed_,
                                      *synthFromLed_,
                                      *synthToLed_);
    hostMidiOutLed_->setLevel(
        activityTracker.getActivityLevel(Core::MidiActivityTracker::Path::kHostMidiOutput));
    if (config_.isVst3Mode && hostMidiDiagnosticsConsole_ != nullptr)
        updateHostMidiInquiryDiagnostics(activityTracker);
}

void SettingsMidiPage::populatePortLists(const juce::String& keepOpenSynthFromId,
                                         const juce::String& keepOpenSynthToId,
                                         const juce::String& keepOpenKeyboardFromId)
{
    TSS::MidiPortComboPopulation::populateInputPortCombo(
        *synthFromCombo_, synthFromPortIdentifiers_, keepOpenSynthFromId);
    TSS::MidiPortComboPopulation::populateOutputPortCombo(
        *synthToCombo_, synthToPortIdentifiers_, keepOpenSynthToId);

    if (SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode)
        && keyboardFromCombo_ != nullptr)
    {
        TSS::MidiPortComboPopulation::populateInputPortCombo(
            *keyboardFromCombo_, keyboardFromPortIdentifiers_, keepOpenKeyboardFromId);
    }
}

juce::String SettingsMidiPage::getSelectedSynthFromPortId() const
{
    return TSS::MidiPortComboPopulation::selectedPortId(*synthFromCombo_, synthFromPortIdentifiers_);
}

juce::String SettingsMidiPage::getSelectedSynthToPortId() const
{
    return TSS::MidiPortComboPopulation::selectedPortId(*synthToCombo_, synthToPortIdentifiers_);
}

juce::String SettingsMidiPage::getSelectedKeyboardFromPortId() const
{
    if (! SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode)
        || keyboardFromCombo_ == nullptr)
        return {};

    return TSS::MidiPortComboPopulation::selectedPortId(*keyboardFromCombo_,
                                                        keyboardFromPortIdentifiers_);
}

void SettingsMidiPage::selectSynthFromPort(const juce::String& portId)
{
    TSS::MidiPortComboPopulation::selectPortInCombo(
        *synthFromCombo_, synthFromPortIdentifiers_, portId);
}

void SettingsMidiPage::selectSynthToPort(const juce::String& portId)
{
    TSS::MidiPortComboPopulation::selectPortInCombo(
        *synthToCombo_, synthToPortIdentifiers_, portId);
}

void SettingsMidiPage::selectKeyboardFromPort(const juce::String& portId)
{
    if (! SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode)
        || keyboardFromCombo_ == nullptr)
        return;

    TSS::MidiPortComboPopulation::selectPortInCombo(
        *keyboardFromCombo_, keyboardFromPortIdentifiers_, portId);
}

bool SettingsMidiPage::isAnyPortPopupOpen() const noexcept
{
    if (synthFromCombo_->isPopupOpen() || synthToCombo_->isPopupOpen())
        return true;

    return keyboardFromCombo_ != nullptr && keyboardFromCombo_->isPopupOpen();
}

void SettingsMidiPage::paint(juce::Graphics&)
{
}

void SettingsMidiPage::resized()
{
    const int rowGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kRowGap), uiScale_);
    const int labelWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kLabelWidth), uiScale_);
    const int controlHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kControlHeight), uiScale_);
    const int comboWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kSynthFromComboWidth), uiScale_);
    const int ledWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPeakWidth), uiScale_);
    const int ledGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPeakGap), uiScale_);

    auto bounds = getLocalBounds();

    const auto placeRow = [&](TSS::Label& label, TSS::ComboBox& combo, TSS::Led& led, bool visible)
    {
        if (! visible)
            return;

        auto row = bounds.removeFromTop(controlHeight);
        label.setBounds(row.getX(), row.getY(), labelWidth, controlHeight);
        label.setUiScale(uiScale_);
        const int controlX = row.getX() + labelWidth;
        combo.setBounds(controlX, row.getY(), comboWidth, controlHeight);
        combo.setUiScale(uiScale_);
        const int ledY = row.getY() + (controlHeight - ledWidth) / 2;
        led.setBounds(controlX + comboWidth + ledGap, ledY, ledWidth, ledWidth);
        led.setUiScale(uiScale_);
        bounds.removeFromTop(rowGap);
    };

    if (config_.isPluginMode)
    {
        layoutHostMidiRouteRows();
        return;
    }

    placeRow(*keyboardFromLabel_, *keyboardFromCombo_, *keyboardFromLed_, true);
    placeRow(*synthFromLabel_, *synthFromCombo_, *synthFromLed_, true);
    placeRow(*synthToLabel_, *synthToCombo_, *synthToLed_, true);
}

void SettingsMidiPage::layoutHostMidiRouteRows()
{
    const int rowGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kRowGap), uiScale_);
    const int labelWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kLabelWidth), uiScale_);
    const int controlHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kControlHeight), uiScale_);
    const int comboWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kSynthFromComboWidth), uiScale_);
    const int ledWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPeakWidth), uiScale_);
    const int ledGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPeakGap), uiScale_);

    auto bounds = getLocalBounds();
    auto row = bounds.removeFromTop(controlHeight);
    hostMidiRouteLabel_->setBounds(row.getX(), row.getY(), labelWidth, controlHeight);
    hostMidiRouteLabel_->setUiScale(uiScale_);
    const int controlX = row.getX() + labelWidth;
    hostMidiRouteValueField_->setBounds(
        controlX, row.getY(), comboWidth + ledGap + ledWidth, controlHeight);
    hostMidiRouteValueField_->setUiScale(uiScale_);
    if (! config_.isVst3Mode)
        return;

    bounds.removeFromTop(rowGap);
    auto buttonRow = bounds.removeFromTop(controlHeight);
    hostMidiInquiryButton_->setBounds(
        controlX, buttonRow.getY(), comboWidth, controlHeight);
    hostMidiInquiryButton_->setUiScale(uiScale_);
    const int ledY = buttonRow.getY() + (controlHeight - ledWidth) / 2;
    hostMidiOutLed_->setBounds(controlX + comboWidth + ledGap, ledY, ledWidth, ledWidth);
    hostMidiOutLed_->setUiScale(uiScale_);

    if (hostMidiDiagnosticsConsole_ != nullptr)
    {
        bounds.removeFromTop(rowGap);
        const int consoleHeight = TSS::ScaledLayout::scaledInt(
            static_cast<float>(kHostMidiDiagnosticsConsoleHeight), uiScale_);
        hostMidiDiagnosticsConsole_->setBounds(bounds.removeFromTop(consoleHeight));
    }
}

void SettingsMidiPage::updateHostMidiInquiryDiagnostics(
    const Core::MidiActivityTracker& activityTracker)
{
    static constexpr std::array<Core::HostMidiInquiryStage, kHostMidiInquiryTraceStageCount> stages {
        Core::HostMidiInquiryStage::kGenerated,
        Core::HostMidiInquiryStage::kOutboundQueue,
        Core::HostMidiInquiryStage::kHostEventQueue,
        Core::HostMidiInquiryStage::kOutputMidiBuffer
    };
    static constexpr std::array<const char*, kHostMidiInquiryTraceStageCount> stageNames {
        "GENERATED",
        "OUTBOUND QUEUE",
        "HOST EVENT QUEUE",
        "OUTPUT MIDI BUFFER"
    };

    for (std::size_t stageIndex = 0; stageIndex < stages.size(); ++stageIndex)
    {
        const auto diagnostic = activityTracker.getHostMidiInquiryDiagnostic(stages[stageIndex]);
        if (diagnostic.sequence <= displayedInquiryDiagnosticSequences_[stageIndex])
            continue;

        displayedInquiryDiagnosticSequences_[stageIndex] = diagnostic.sequence;
        juce::String line(stageNames[stageIndex]);
        line << " (" << diagnostic.totalSize << " bytes): ";

        for (int byteIndex = 0; byteIndex < diagnostic.capturedSize; ++byteIndex)
        {
            if (byteIndex > 0)
                line << ((byteIndex % 16 == 0) ? "\n  " : " ");
            line << juce::String::toHexString(
                        static_cast<int>(diagnostic.bytes[static_cast<std::size_t>(byteIndex)]))
                        .paddedLeft('0', 2)
                        .toUpperCase();
        }

        if (diagnostic.capturedSize < diagnostic.totalSize)
            line << "\n  [trace capped at " << diagnostic.capturedSize << " bytes]";

        if (waitingForInquiryTrace_)
        {
            hostMidiDiagnosticsConsole_->setText(juce::String(), juce::dontSendNotification);
            waitingForInquiryTrace_ = false;
        }

        line << "\n";
        hostMidiDiagnosticsConsole_->setText(
            hostMidiDiagnosticsConsole_->getText() + line,
            juce::dontSendNotification);
        hostMidiDiagnosticsConsole_->moveCaretToEnd();
    }
}

std::unique_ptr<TSS::Label> SettingsMidiPage::makeLabel(TSS::ISkin& skin, const juce::String& text)
{
    auto label = std::make_unique<TSS::Label>(SettingsShellMetrics::kLabelWidth,
                                              SettingsShellMetrics::kControlHeight,
                                              TSS::darkPanelLabelLookFromSkin(skin),
                                              text);
    addAndMakeVisible(*label);
    return label;
}

std::unique_ptr<TSS::ComboBox> SettingsMidiPage::makeCombo(TSS::ISkin& skin)
{
    auto combo = std::make_unique<TSS::ComboBox>(SettingsShellMetrics::kSynthFromComboWidth,
                                                 SettingsShellMetrics::kControlHeight,
                                                 TSS::comboBoxLookFromSkin(skin),
                                                 TSS::ComboBox::Style::ButtonLike);
    combo->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    combo->setUsesPortSentinelPopupChrome(true);
    combo->setUsesContentSizedPopupWidth(true);
    addAndMakeVisible(*combo);
    return combo;
}
