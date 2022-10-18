//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/TransformBus.h>

#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentBus.h>

#include "EditorDemoRoomDrawTextComponent.h"
#include "DemoRoomDrawTextComponent.h"
#include "TextRendering/Render3DTextRenderFeature.h"

namespace DemoRoom
{
    void EditorDemoRoomDrawTextComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<EditorDemoRoomDrawTextComponent, AZ::Component>()
                ->Version(0)
                ->Field("Text", &EditorDemoRoomDrawTextComponent::m_text)
                ->Field("Color", &EditorDemoRoomDrawTextComponent::m_color)
                ;

            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<EditorDemoRoomDrawTextComponent>(
                    "DemoRoom DrawText", "Setup a 3D text")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "DemoRoom")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::MultiLineEdit, &EditorDemoRoomDrawTextComponent::m_text, "Text", "Text to display.")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorDemoRoomDrawTextComponent::OnPropertyUpdate)
                    ->DataElement(AZ::Edit::UIHandlers::Color, &EditorDemoRoomDrawTextComponent::m_color, "Color", "Color of the text to display.")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorDemoRoomDrawTextComponent::OnPropertyUpdate)
                    ;
            }
        }
    }

    void EditorDemoRoomDrawTextComponent::BuildGameEntity(AZ::Entity* gameEntity)
    {
        gameEntity->CreateComponent<DemoRoomDrawTextComponent>(m_text, m_color);
    }

    void EditorDemoRoomDrawTextComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        DemoRoomDrawTextComponent::GetProvidedServices(provided);
    }

    void EditorDemoRoomDrawTextComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        DemoRoomDrawTextComponent::GetIncompatibleServices(incompatible);
    }

    void EditorDemoRoomDrawTextComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        DemoRoomDrawTextComponent::GetRequiredServices(required);
    }

    void EditorDemoRoomDrawTextComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        DemoRoomDrawTextComponent::GetDependentServices(dependent);
    }

    void EditorDemoRoomDrawTextComponent::Init()
    {
    }

    void EditorDemoRoomDrawTextComponent::Activate()
    {
        EditorDemoRoomDrawTextRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        SRenderText3D   textToRender;
        textToRender.m_Text = m_text;
        textToRender.m_Color = m_color.GetAsVector3();
        textToRender.m_EntityId = GetEntityId();
        AZ::TransformBus::EventResult(textToRender.m_Transform, textToRender.m_EntityId, &AZ::TransformBus::Events::GetWorldTM);
        DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::UpdateText, textToRender);
    }

    void EditorDemoRoomDrawTextComponent::Deactivate()
    {
        AZ::TransformNotificationBus::Handler::BusDisconnect(GetEntityId());
        DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::RemoveText, GetEntityId());
        EditorDemoRoomDrawTextRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void EditorDemoRoomDrawTextComponent::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, const AZ::Transform& world)
    {
        SRenderText3D   textToRender;
        textToRender.m_Text = m_text;
        textToRender.m_Color = m_color.GetAsVector3();
        textToRender.m_Transform = world;
        textToRender.m_EntityId = GetEntityId();
        DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::UpdateText, textToRender);
    }

    void EditorDemoRoomDrawTextComponent::OnPropertyUpdate()
    {
        SRenderText3D   textToRender;
        textToRender.m_Text = m_text;
        textToRender.m_Color = m_color.GetAsVector3();
        textToRender.m_EntityId = GetEntityId();
        AZ::TransformBus::EventResult(textToRender.m_Transform, textToRender.m_EntityId, &AZ::TransformBus::Events::GetWorldTM);
        DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::UpdateText, textToRender);
    }
} // namespace DemoRoom
