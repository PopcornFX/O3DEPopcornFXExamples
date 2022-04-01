//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include "DrawPopcornFXVersionTextComponent.h"

#include <DemoRoom/DemoRoomDrawTextRequests.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Interface/Interface.h>
#include <AzCore/Console/IConsole.h>
#include <AzCore/Settings/SettingsRegistryMergeUtils.h>
#include <AzCore/Settings/SettingsRegistryImpl.h>
#include <AzCore/Utils/Utils.h>

//////////////////////////////////////////////////////////////////////////////
void DrawPopcornFXVersionTextComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
{
}

//////////////////////////////////////////////////////////////////////////////
void DrawPopcornFXVersionTextComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
{
    provided.push_back(AZ_CRC("DrawPopcornFXVersionText", 0xd41af40c));
}

//////////////////////////////////////////////////////////////////////////////
void DrawPopcornFXVersionTextComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
{
}

//////////////////////////////////////////////////////////////////////////////
void DrawPopcornFXVersionTextComponent::Reflect(AZ::ReflectContext* reflection)
{
    AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
    if (serializeContext)
    {
        serializeContext->Class<DrawPopcornFXVersionTextComponent, AZ::Component>()
            ->Version(0)
            ->Field("Color", &DrawPopcornFXVersionTextComponent::m_color)
            ->Field("O3DE Version", &DrawPopcornFXVersionTextComponent::m_displayO3DEVersion)
            ;

        AZ::EditContext* editContext = serializeContext->GetEditContext();
        if (editContext)
        {
            editContext->Class<DrawPopcornFXVersionTextComponent>("Draw PopcornFX Version Text", "Draw PopcornFX verison with a 3D text")
                ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute("Category", "Gameplay")
                ->Attribute("AutoExpand", true)
                ->Attribute("AppearsInAddComponentMenu", AZ_CRC("Game", 0x232b318c))
                ->DataElement(0, &DrawPopcornFXVersionTextComponent::m_color, "Color", "Text color")
                ->DataElement(0, &DrawPopcornFXVersionTextComponent::m_displayO3DEVersion, "O3DE version", "Display O3DE version")
                ;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
DrawPopcornFXVersionTextComponent::~DrawPopcornFXVersionTextComponent()
{
}

//////////////////////////////////////////////////////////////////////////////
void DrawPopcornFXVersionTextComponent::Init()
{
}

//////////////////////////////////////////////////////////////////////////////
void DrawPopcornFXVersionTextComponent::Activate()
{
    if (m_displayO3DEVersion)
    {
        m_versionToDisplay = GetEngineVersion();
    }
    else
    {
        AZ::Interface<AZ::IConsole>::Get()->GetCvarValue<AZStd::string>("p_PopcornFXVersion", m_versionToDisplay);
    }
    AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
    SRenderText3D   textToRender;
    textToRender.m_Text = m_versionToDisplay;
    textToRender.m_Color = m_color.GetAsVector3();
    textToRender.m_EntityId = GetEntityId();
    AZ::TransformBus::EventResult(textToRender.m_Transform, textToRender.m_EntityId, &AZ::TransformBus::Events::GetWorldTM);
    DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::UpdateText, textToRender);
}

//////////////////////////////////////////////////////////////////////////////
void DrawPopcornFXVersionTextComponent::Deactivate()
{
    AZ::TransformNotificationBus::Handler::BusDisconnect(GetEntityId());
    DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::RemoveText, GetEntityId());
}

void DrawPopcornFXVersionTextComponent::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, const AZ::Transform& world)
{
    SRenderText3D   textToRender;
    textToRender.m_Text = m_versionToDisplay;
    textToRender.m_Color = m_color.GetAsVector3();
    textToRender.m_Transform = world;
    textToRender.m_EntityId = GetEntityId();
    DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::UpdateText, textToRender);
}

AZStd::string   DrawPopcornFXVersionTextComponent::GetEngineVersion() const
{
    constexpr const char* EngineVersionJsonKey = "O3DEVersion";
    auto engineVersionKey = AZ::SettingsRegistryInterface::FixedValueString(AZ::SettingsRegistryMergeUtils::EngineSettingsRootKey) + "/" + EngineVersionJsonKey;
    AZStd::string engineVersion;
    if (auto settingsRegistry = AZ::SettingsRegistry::Get(); settingsRegistry != nullptr && settingsRegistry->Get(engineVersion, engineVersionKey))
    {
        return engineVersion;
    }

    auto engineSettingsPath = AZ::IO::FixedMaxPath{ AZ::Utils::GetEnginePath() } / "engine.json";
    if (AZ::IO::SystemFile::Exists(engineSettingsPath.c_str()))
    {
        AZ::SettingsRegistryImpl settingsRegistry;
        if (settingsRegistry.MergeSettingsFile(
            engineSettingsPath.Native(), AZ::SettingsRegistryInterface::Format::JsonMergePatch, AZ::SettingsRegistryMergeUtils::EngineSettingsRootKey))
        {
            settingsRegistry.Get(engineVersion, engineVersionKey);
        }
    }
    return engineVersion;
}