//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include "DemoRoomCameraInputComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Interface/Interface.h>

#include <AzFramework/Physics/CharacterBus.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <AzFramework/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzFramework/Input/Devices/Mouse/InputDeviceMouse.h>
#include <AzFramework/Input/Devices/Gamepad/InputDeviceGamepad.h>

using namespace AzFramework;

namespace DemoRoom
{
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("TransformService", 0x8ee22c50));
        required.push_back(AZ_CRC("PhysXCharacterGameplayService", 0xfacd7876));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("InputService", 0xd41af40c));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("NonUniformScaleService"));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::Reflect(AZ::ReflectContext* reflection)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
        if (serializeContext)
        {
            serializeContext->Class<DemoRoomCameraInputComponent, AZ::Component>()
                ->Version(0)
                ->Field("Camera Entity", &DemoRoomCameraInputComponent::m_cameraEntityId)
                ->Field("Move Speed", &DemoRoomCameraInputComponent::m_moveSpeed)
                ->Field("Rotation Speed", &DemoRoomCameraInputComponent::m_rotationSpeed)
                ->Field("Invert Rotation Input Y", &DemoRoomCameraInputComponent::m_invertRotationInputAxisY)
                ;
    
            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<DemoRoomCameraInputComponent>("DemoRoom Camera Input", "The DemoRoom Camera Input allows you to control the controller and the camera")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute("Category", "Gameplay")
                    //->Attribute("Icon", "Icons/Components/DemoRoomCameraInput.svg")
                    //->Attribute("ViewportIcon", "Icons/Components/Viewport/DemoRoomCameraInput.svg")
                    ->Attribute("AutoExpand", true)
                    ->Attribute("AppearsInAddComponentMenu", AZ_CRC("Game", 0x232b318c))
                    ->DataElement(0, &DemoRoomCameraInputComponent::m_cameraEntityId, "Camera Entity", "Camera entity to rotate")
                    ->DataElement(0, &DemoRoomCameraInputComponent::m_moveSpeed, "Move Speed", "Speed at which the camera moves")
                    ->DataElement(0, &DemoRoomCameraInputComponent::m_rotationSpeed, "Rotation Speed", "Speed at which the camera rotates")
                    ->DataElement(0, &DemoRoomCameraInputComponent::m_invertRotationInputAxisY, "Invert Rotation Input Y", "Invert rotation input y-axis")
                    ;
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    DemoRoomCameraInputComponent::~DemoRoomCameraInputComponent()
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::Init()
    {
        m_sceneStartSimHandler = AzPhysics::SceneEvents::OnSceneSimulationStartHandler(
            [this](
                [[maybe_unused]] AzPhysics::SceneHandle sceneHandle,
                float fixedDeltatime
                )
        {
            this->MoveEntity(fixedDeltatime);
        }, aznumeric_cast<int32_t>(AzPhysics::SceneEvents::PhysicsStartFinishSimulationPriority::Components));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::Activate()
    {
        InputChannelEventListener::Connect();
        AZ::TickBus::Handler::BusConnect();

        if (auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get())
        {
            AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
            sceneInterface->RegisterSceneSimulationStartHandler(sceneHandle, m_sceneStartSimHandler);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        InputChannelEventListener::Disconnect();
        m_sceneStartSimHandler.Disconnect();
    }

    // Physics::SystemEvent
    void DemoRoomCameraInputComponent::MoveEntity(float /*deltaTime*/)
    {
        AZ::Transform worldTransform = AZ::Transform::Identity();
        EBUS_EVENT_ID_RESULT(worldTransform, GetEntityId(), AZ::TransformBus, GetWorldTM);

        // Update movement
        const AZ::Vector3 right = worldTransform.GetBasisX();
        const AZ::Vector3 forward = worldTransform.GetBasisY();
        const AZ::Vector3 movement = (forward * m_movement.GetY()) + (right * m_movement.GetX());

        //Gravity
        const AZ::Vector3 gravity = AZ::Vector3(0.0f, 0.0f, -13.0f);

        Physics::CharacterRequestBus::Event(GetEntityId(), &Physics::CharacterRequests::AddVelocity, movement * m_moveSpeed + gravity);
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::OnTick(float /*deltaTime*/, AZ::ScriptTimePoint /*time*/)
    {
        AZ::Quaternion rotation = AZ::Quaternion::CreateIdentity();
        EBUS_EVENT_ID_RESULT(rotation, GetEntityId(), AZ::TransformBus, GetWorldRotationQuaternion);
        rotation *= AZ::Quaternion::CreateFromEulerAnglesDegrees(AZ::Vector3(0.0f, 0.0f, -m_rotation.GetX()) * m_rotationSpeed);
        EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetWorldRotationQuaternion, rotation);

        if (m_cameraEntityId.IsValid())
        {
            const float invertedRotation = m_invertRotationInputAxisY ? m_rotation.GetY() : -m_rotation.GetY();

            AZ::Vector3 camRotation = AZ::Vector3::CreateZero();
            EBUS_EVENT_ID_RESULT(camRotation, m_cameraEntityId, AZ::TransformBus, GetLocalRotation);
            camRotation = AZ::Vector3RadToDeg(camRotation);
            camRotation += AZ::Vector3(invertedRotation, 0.0f, 0.0f) * m_rotationSpeed;
            camRotation.SetX(AZ::GetClamp(camRotation.GetX(), -90.f, 90.0f));
            camRotation = AZ::Vector3DegToRad(camRotation);
            EBUS_EVENT_ID(m_cameraEntityId, AZ::TransformBus, SetLocalRotation, camRotation);
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool DemoRoomCameraInputComponent::OnInputChannelEventFiltered(const InputChannel& inputChannel)
    {
        const InputDeviceId& deviceId = inputChannel.GetInputDevice().GetInputDeviceId();
        if (InputDeviceMouse::IsMouseDevice(deviceId))
        {
            OnMouseEvent(inputChannel);
        }
        else if (InputDeviceKeyboard::IsKeyboardDevice(deviceId))
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
    void DemoRoomCameraInputComponent::OnMouseEvent(const InputChannel& inputChannel)
    {
        const InputChannelId& channelId = inputChannel.GetInputChannelId();
        if (channelId == InputDeviceMouse::Movement::X)
        {
            m_rotation.SetX(inputChannel.GetValue());
        }
        else if (channelId == InputDeviceMouse::Movement::Y)
        {
            m_rotation.SetY(inputChannel.GetValue());
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::OnKeyboardEvent(const InputChannel& inputChannel)
    {
        const InputChannelId& channelId = inputChannel.GetInputChannelId();
        if (channelId == InputDeviceKeyboard::Key::AlphanumericW)
        {
            m_movement.SetY(inputChannel.GetValue());
        }
        
        if (channelId == InputDeviceKeyboard::Key::AlphanumericA)
        {
            m_movement.SetX(-inputChannel.GetValue());
        }
        
        if (channelId == InputDeviceKeyboard::Key::AlphanumericS)
        {
            m_movement.SetY(-inputChannel.GetValue());
        }
        
        if (channelId == InputDeviceKeyboard::Key::AlphanumericD)
        {
            m_movement.SetX(inputChannel.GetValue());
        }
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void DemoRoomCameraInputComponent::OnGamepadEvent(const InputChannel& inputChannel)
    {
        const InputChannelId& channelId = inputChannel.GetInputChannelId();
        if (channelId == InputDeviceGamepad::ThumbStickAxis1D::LX)
        {
            m_movement.SetX(inputChannel.GetValue());
        }
        
        if (channelId == InputDeviceGamepad::ThumbStickAxis1D::LY)
        {
            m_movement.SetY(inputChannel.GetValue());
        }
        
        if (channelId == InputDeviceGamepad::ThumbStickAxis1D::RX)
        {
            m_rotation.SetX(inputChannel.GetValue());
        }
        
        if (channelId == InputDeviceGamepad::ThumbStickAxis1D::RY)
        {
            m_rotation.SetY(inputChannel.GetValue());
        }
    }
}
