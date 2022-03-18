//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "EditorDemoRoomComponent.h"

using namespace AzToolsFramework;

namespace DemoRoom
{
    void EditorDemoRoomComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<EditorDemoRoomComponent, EditorComponentBase>()
                ->Version(0)
                ->Field("StandsEntityId", &EditorDemoRoomComponent::m_standsEntityId)
                ->Field("EnvironmentEntityId", &EditorDemoRoomComponent::m_environmentEntityId)
                ->Field("RoomSizes", &EditorDemoRoomComponent::m_roomSizes)
                ->Field("Lights", &EditorDemoRoomComponent::m_lights)
                ->Field("Double Height", &EditorDemoRoomComponent::m_doubleHeight);

            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<EditorDemoRoomComponent>(
                    "DemoRoom", "DemoRoom Properties")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "DemoRoom")
                    //->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/DemoRoom.svg")
                    //->Attribute(AZ::Edit::Attributes::ViewportIcon, "Icons/Components/Viewport/DemoRoom.svg")
                    ;
            }
        }
    }

    void EditorDemoRoomComponent::BuildGameEntity([[maybe_unused]] AZ::Entity* gameEntity)
    {
    }

    void EditorDemoRoomComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("DemoRoomService"));
    }

    void EditorDemoRoomComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("DemoRoomService"));
    }

    void EditorDemoRoomComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void EditorDemoRoomComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void EditorDemoRoomComponent::Init()
    {
    }

    void EditorDemoRoomComponent::Activate()
    {
        EditorDemoRoomRequestBus::Handler::BusConnect(GetEntityId());
    }

    void EditorDemoRoomComponent::Deactivate()
    {
        EditorDemoRoomRequestBus::Handler::BusDisconnect(GetEntityId());
    }

} // namespace DemoRoom
