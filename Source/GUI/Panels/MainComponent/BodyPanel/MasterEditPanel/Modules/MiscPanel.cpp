#include "MiscPanel.h"

#include "Core/Services/EpromTypePolicy.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Helpers/GrayedControlHelper.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout MiscPanel::createLayout()
{
    return makeMasterEditModuleLayout(
        PluginIDs::MasterEditSection::MiscModule::kGroupId,
        PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
        {
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBendRange,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable
        });
}

MiscPanel::MiscPanel(const Config& config)
    : BaseModulePanel(BaseModulePanel::Config{
          .skin = config.skin,
          .widgetFactory = config.widgetFactory,
          .apvts = config.apvts,
          .layout = createLayout(),
          .width = config.width,
          .height = config.height,
          .moduleHeaderDims = config.moduleHeaderDims,
          .parameterCellDims = config.parameterCellDims})
{
    registerContextualHelp();
    apvts_.state.addListener(this);
    refreshUnisonDetuneGraying();
}

MiscPanel::~MiscPanel()
{
    apvts_.state.removeListener(this);
}

void MiscPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::MasterEditSection::MiscModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitOnly(*contextualHelpBinder_, moduleHeader_.get(), Help::kInit);
    TSS::bindModuleHeaderTitle(*contextualHelpBinder_, moduleHeader_.get(), Help::kTitle);

    static constexpr const char* kCellHelps[] = {
        Help::kMasterTune,
        Help::kMasterTranspose,
        Help::kBendRange,
        Help::kUnison,
        Help::kUnisonDetune,
        Help::kVolumeInvert,
        Help::kBankLock,
        Help::kMemoryProtect,
    };
    TSS::bindParameterCellHelps(*contextualHelpBinder_, *this, kCellHelps, std::size(kCellHelps));
}

void MiscPanel::valueTreePropertyChanged(juce::ValueTree&,
                                         const juce::Identifier& property)
{
    if (property == juce::Identifier(PluginIDs::Settings::kEpromType)
        || property == juce::Identifier(MatrixDeviceTypes::kApvtsPropertyName)
        || property == juce::Identifier("deviceDetected"))
        refreshUnisonDetuneGraying();
}

void MiscPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshUnisonDetuneGraying();
}

void MiscPanel::refreshUnisonDetuneGraying()
{
    const int epromType = Core::EpromTypePolicy::normalize(static_cast<int>(
        apvts_.state.getProperty(PluginIDs::Settings::kEpromType,
                                 PluginIDs::Settings::EpromType::kDefault)));
    const bool grayed = ! Core::EpromTypePolicy::supportsUnisonDetune(epromType);
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts_.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const bool matrix6 = MatrixDeviceTypes::isMatrix6Family(deviceType);

    for (size_t cellIndex = 1; cellIndex <= 7; ++cellIndex)
    {
        auto* cell = getParameterCellAt(cellIndex);
        if (cell == nullptr)
            continue;

        const bool isDetune = cellIndex == static_cast<size_t>(kUnisonDetuneCellIndex);
        cell->setAlpha(matrix6 ? 0.45f : 1.0f);
        if (auto* slider = cell->getSlider())
            slider->setEnabled(! matrix6 && (! isDetune || ! grayed));
        if (auto* comboBox = cell->getComboBox())
            comboBox->setEnabled(! matrix6);

        if (! isDetune)
            continue;

        if (! matrix6 && grayed)
        {
            TSS::GrayedControlHelper::setGrayedClickHandler(*cell, true, [this]
            {
                TSS::GrayedControlHelper::setFooterInfoMessage(
                    apvts_,
                    PluginDisplayNames::MasterEditSection::MiscModule::kUnisonDetuneOptimisedEpromFooter);
            });
        }
        else
        {
            TSS::GrayedControlHelper::clearGrayedClickHandler(*cell);
        }
    }
}
