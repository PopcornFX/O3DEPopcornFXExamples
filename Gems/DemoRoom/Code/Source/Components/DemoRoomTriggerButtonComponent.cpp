//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include "DemoRoomTriggerButtonComponent.h"

#include <DemoRoom/DemoRoomTriggerButtonReceiverComponentBus.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzFramework/Physics/PhysicsSystem.h>
#include <AzFramework/Physics/Collision/CollisionEvents.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>

namespace DemoRoom
{
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("TransformService", 0x8ee22c50));
        required.push_back(AZ_CRC("PhysXTriggerService", 0x3a117d7b));
        required.push_back(AZ_CRC("MeshService", 0x71d8a455));
        required.push_back(AZ_CRC("MaterialProviderService", 0x64849a6b));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("DemoRoomTriggerButtonService"));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("DemoRoomTriggerButtonService"));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonComponent::Reflect(AZ::ReflectContext* reflection)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
        if (serializeContext)
        {
            serializeContext->Class<DemoRoomTriggerButtonComponent, AZ::Component>()
                ->Version(0)
                ->Field("Receivers", &DemoRoomTriggerButtonComponent::m_receivers)
                ->Field("SlotLabel", &DemoRoomTriggerButtonComponent::m_slotLabel)
                ->Field("ActivatedColor", &DemoRoomTriggerButtonComponent::m_activatedColor)
                ->Field("DeactivatedColor", &DemoRoomTriggerButtonComponent::m_deactivatedColor)
                ;
    
            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<DemoRoomTriggerButtonComponent>("DemoRoom Trigger Button", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute("Category", "Gameplay")
                    //->Attribute("Icon", "Icons/Components/DemoRoomTriggerButton.svg")
                    //->Attribute("ViewportIcon", "Icons/Components/Viewport/DemoRoomTriggerButton.svg")
                    ->Attribute("AutoExpand", true)
                    ->Attribute("AppearsInAddComponentMenu", AZ_CRC("Game", 0x232b318c))
                    ->DataElement(0, &DemoRoomTriggerButtonComponent::m_receivers, "Receiver Entities", "Entities which will receive trigger enter/exit notifications")
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Button material properties")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, false)
                    ->DataElement(0, &DemoRoomTriggerButtonComponent::m_slotLabel, "Slot label", "")
                    ->DataElement(0, &DemoRoomTriggerButtonComponent::m_activatedColor, "Activated Color", "")
                    ->DataElement(0, &DemoRoomTriggerButtonComponent::m_deactivatedColor, "Deactivated Color", "")
                    ;
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    DemoRoomTriggerButtonComponent::~DemoRoomTriggerButtonComponent()
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonComponent::Init()
    {
        m_onTriggerEnterHandler = AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler([this](
            [[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const  AzPhysics::TriggerEvent& triggerEvent)
        {
            OnTriggerEnter(triggerEvent);
        });

        m_onTriggerExitHandler = AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler([this](
            [[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const  AzPhysics::TriggerEvent& triggerEvent)
        {
            OnTriggerExit(triggerEvent);
        });
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonComponent::Activate()
    {
        if (auto* physicsSystem = AZ::Interface<AzPhysics::SystemInterface>::Get())
        {
            AZStd::pair<AzPhysics::SceneHandle, AzPhysics::SimulatedBodyHandle> foundBody = physicsSystem->FindAttachedBodyHandleFromEntityId(GetEntityId());
            if (foundBody.first != AzPhysics::InvalidSceneHandle)
            {
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(
                    foundBody.first, foundBody.second, m_onTriggerEnterHandler);
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerExitHandler(
                    foundBody.first, foundBody.second, m_onTriggerExitHandler);
            }
        }

        SetButtonColor(m_deactivatedColor);
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonComponent::Deactivate()
    {
        m_onTriggerEnterHandler.Disconnect();
        m_onTriggerExitHandler.Disconnect();
    }

    void DemoRoomTriggerButtonComponent::OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent) const
    {
        if (triggerEvent.m_otherBody == nullptr)
            return;

        // Ignore self
        AZ::EntityId entityId = triggerEvent.m_otherBody->GetEntityId();
        if (entityId == GetEntityId())
        {
            return;
        }

        SetButtonColor(m_activatedColor);

        for (auto receiverEntityId : m_receivers)
        {
            DemoRoomTriggerButtonReceiverNotificationBus::Event(
                receiverEntityId,
                &DemoRoomTriggerButtonReceiverNotificationBus::Events::OnTriggerButtonEnter);
        }
    }

    void DemoRoomTriggerButtonComponent::OnTriggerExit(const AzPhysics::TriggerEvent& triggerEvent) const
    {
        if (triggerEvent.m_otherBody == nullptr)
            return;

        SetButtonColor(m_deactivatedColor);

        for (auto receiverEntityId : m_receivers)
        {
            DemoRoomTriggerButtonReceiverNotificationBus::Event(
                receiverEntityId,
                &DemoRoomTriggerButtonReceiverNotificationBus::Events::OnTriggerButtonExit);
        }
    }

    void DemoRoomTriggerButtonComponent::SetButtonColor(const AZ::Color& color) const
    {
        AZ::Render::MaterialAssignmentId slot;
        AZ::Render::MaterialComponentRequestBus::EventResult(slot, GetEntityId(),
            &AZ::Render::MaterialComponentRequestBus::Events::FindMaterialAssignmentId, 0, m_slotLabel);

        AZ::Render::MaterialComponentRequestBus::Event(GetEntityId(),
#if PK_O3DE_MAJOR_VERSION > 2205
            &AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValueT<AZ::Color>,
#else
            &AZ::Render::MaterialComponentRequestBus::Events::SetPropertyOverrideT<AZ::Color>,
#endif
            slot, "baseColor.color", color);
    }
}
