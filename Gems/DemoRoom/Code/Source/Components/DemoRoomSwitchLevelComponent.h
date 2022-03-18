//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzFramework/Input/Events/InputChannelEventListener.h>
#include <AzFramework/Physics/Common/PhysicsEvents.h>

namespace DemoRoom
{
    class DemoRoomSwitchLevelComponent
        : public AZ::Component
        , public AzFramework::InputChannelEventListener
    {
    public:
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void Reflect(AZ::ReflectContext* reflection);

        AZ_COMPONENT(DemoRoomSwitchLevelComponent, "{B55F7122-70D8-4FF0-B105-CE1F2BB7BDAA}");
        virtual ~DemoRoomSwitchLevelComponent();

        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // AzFramework::InputChannelEventListener
        bool OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel) override;

    private:
        void OnKeyboardEvent(const AzFramework::InputChannel& inputChannel);
        void OnGamepadEvent(const AzFramework::InputChannel& inputChannel);

        // Editable Properties
        AZStd::string m_levelName;
    };
}
