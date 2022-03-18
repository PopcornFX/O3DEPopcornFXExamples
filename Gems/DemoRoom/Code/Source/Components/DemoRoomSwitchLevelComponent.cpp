//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include "DemoRoomSwitchLevelComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Interface/Interface.h>
#include <AzCore/Console/IConsole.h>

#include <AzFramework/Physics/CharacterBus.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzFramework/Input/Devices/Mouse/InputDeviceMouse.h>
#include <AzFramework/Input/Devices/Gamepad/InputDeviceGamepad.h>

using namespace AzFramework;

namespace DemoRoom
{
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("SwitchLevel", 0xd41af40c));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::Reflect(AZ::ReflectContext* reflection)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
        if (serializeContext)
        {
            serializeContext->Class<DemoRoomSwitchLevelComponent, AZ::Component>()
                ->Version(0)
                ->Field("LevelName", &DemoRoomSwitchLevelComponent::m_levelName)
                ;
    
            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<DemoRoomSwitchLevelComponent>("DemoRoom Switch Level", "Switch between levels")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute("Category", "Gameplay")
                    ->Attribute("AutoExpand", true)
                    ->Attribute("AppearsInAddComponentMenu", AZ_CRC("Game", 0x232b318c))
                    ->DataElement(0, &DemoRoomSwitchLevelComponent::m_levelName, "Level Name", "Name of the level to load")
                    ;
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    DemoRoomSwitchLevelComponent::~DemoRoomSwitchLevelComponent()
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::Init()
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::Activate()
    {
        InputChannelEventListener::Connect();
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::Deactivate()
    {
        InputChannelEventListener::Disconnect();
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool DemoRoomSwitchLevelComponent::OnInputChannelEventFiltered(const InputChannel& inputChannel)
    {
        const InputDeviceId& deviceId = inputChannel.GetInputDevice().GetInputDeviceId();
        if (InputDeviceKeyboard::IsKeyboardDevice(deviceId))
        {
            OnKeyboardEvent(inputChannel);
        }
        else if (AzFramework::InputDeviceGamepad::IsGamepadDevice(deviceId))
        {
            OnGamepadEvent(inputChannel);
        }
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::OnKeyboardEvent(const InputChannel& inputChannel)
    {
        const auto console = AZ::Interface<AZ::IConsole>::Get();
        if (console == nullptr)
            return;
        const InputChannelId& channelId = inputChannel.GetInputChannelId();
        if (channelId == InputDeviceKeyboard::Key::EditTab)
        {
            const AZStd::string loadLevelString = AZStd::string("LoadLevel ") + m_levelName;
            console->PerformCommand(loadLevelString.c_str());
        }
        else if (channelId == InputDeviceKeyboard::Key::Escape)
        {
            AZ::ApplicationTypeQuery applicationType;
            AZ::ComponentApplicationBus::Broadcast(&AZ::ComponentApplicationRequests::QueryApplicationType, applicationType);
            if (!applicationType.IsEditor())
            {
                console->PerformCommand("quit");
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DemoRoomSwitchLevelComponent::OnGamepadEvent(const InputChannel& inputChannel)
    {
        const auto console = AZ::Interface<AZ::IConsole>::Get();
        if (console == nullptr)
            return;
        const InputChannelId& channelId = inputChannel.GetInputChannelId();
        if (channelId == InputDeviceGamepad::Trigger::R2)
        {
            const AZStd::string loadLevelString = AZStd::string("LoadLevel ") + m_levelName;
            console->PerformCommand(loadLevelString.c_str());
        }
    }
}
