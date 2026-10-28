#include "VibratoPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout VibratoPanel::createLayout()
{
    return makeMasterEditModuleLayout(
        PluginIDs::MasterEditSection::VibratoModule::kGroupId,
        PluginIDs::MasterEditSection::VibratoModule::StandaloneWidgets::kInit,
        {
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeed,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmplitude,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModAmount,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModAmount
        });
}

VibratoPanel::VibratoPanel(const Config& config)
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
    refreshModulationAmountRanges();
}

VibratoPanel::~VibratoPanel()
{
    apvts_.state.removeListener(this);
}

void VibratoPanel::valueTreePropertyChanged(juce::ValueTree&,
                                            const juce::Identifier& property)
{
    if (property == juce::Identifier(MatrixDeviceTypes::kApvtsPropertyName))
        refreshModulationAmountRanges();
}

void VibratoPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshModulationAmountRanges();
}

void VibratoPanel::refreshModulationAmountRanges()
{
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts_.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const bool matrix6 = MatrixDeviceTypes::isMatrix6Family(deviceType);

    for (const auto cellIndex : { size_t { 4 }, size_t { 6 } })
    {
        auto* cell = getParameterCellAt(cellIndex);
        if (cell == nullptr)
            continue;

        if (auto* slider = cell->getSlider())
            slider->setRange(matrix6 ? 0.0 : -63.0,
                             63.0,
                             1.0,
                             juce::dontSendNotification);
    }
}

void VibratoPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::MasterEditSection::VibratoModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitOnly(*contextualHelpBinder_, moduleHeader_.get(), Help::kInit);
    TSS::bindModuleHeaderTitle(*contextualHelpBinder_, moduleHeader_.get(), Help::kTitle);

    static constexpr const char* kCellHelps[] = {
        Help::kSpeed,
        Help::kWaveform,
        Help::kAmplitude,
        Help::kSpeedModSource,
        Help::kSpeedModAmount,
        Help::kAmpModSource,
        Help::kAmpModAmount,
    };
    TSS::bindParameterCellHelps(*contextualHelpBinder_, *this, kCellHelps, std::size(kCellHelps));
}

