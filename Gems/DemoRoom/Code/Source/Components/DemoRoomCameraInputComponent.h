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
    class DemoRoomCameraInputComponent
        : public AZ::Component
        , public AZ::TickBus::Handler
        , public AzFramework::InputChannelEventListener
    {
    public:
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void Reflect(AZ::ReflectContext* reflection);

        AZ_COMPONENT(DemoRoomCameraInputComponent, "{B55F7122-70D8-4FF0-B105-CE1F2CC7BDAA}");
        virtual ~DemoRoomCameraInputComponent();

        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // AZ::TickBus
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        // AzFramework::InputChannelEventListener
        bool OnInputChannelEventFiltered(const AzFramework::InputChannel& inputChannel) override;

    private:
        void MoveEntity(float deltaTime);
        void OnMouseEvent(const AzFramework::InputChannel& inputChannel);
        void OnKeyboardEvent(const AzFramework::InputChannel& inputChannel);
        void OnGamepadEvent(const AzFramework::InputChannel& inputChannel);

        // Editable Properties
        AZ::EntityId m_cameraEntityId;

        float m_moveSpeed = 20.0f;
        float m_rotationSpeed = 0.5f;

        bool m_invertRotationInputAxisY = false;

        // Run-time Properties
        AZ::Vector2 m_movement = AZ::Vector2::CreateZero();
        AZ::Vector2 m_rotation = AZ::Vector2::CreateZero();

        AzPhysics::SceneEvents::OnSceneSimulationStartHandler m_sceneStartSimHandler;
    };
}
