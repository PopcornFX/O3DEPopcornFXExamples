//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Component/TransformBus.h>

#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentBus.h>

#include "DemoRoomDrawTextComponent.h"
#include "TextRendering/Render3DTextRenderFeature.h"

namespace DemoRoom
{
    void DemoRoomDrawTextComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<DemoRoomDrawTextComponent, AZ::Component>()
                ->Version(0)
                ->Field("Text", &DemoRoomDrawTextComponent::m_text)
                ->Field("Color", &DemoRoomDrawTextComponent::m_color)
                ;
        }
    }

    DemoRoomDrawTextComponent::DemoRoomDrawTextComponent(const AZStd::string& text, const AZ::Color& color)
        : m_text(text)
        , m_color(color)
    {
    }

    void DemoRoomDrawTextComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("DemoRoomDrawTextService"));
    }

    void DemoRoomDrawTextComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("DemoRoomDrawTextService"));
    }

    void DemoRoomDrawTextComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void DemoRoomDrawTextComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void DemoRoomDrawTextComponent::Init()
    {
    }

    void DemoRoomDrawTextComponent::Activate()
    {
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        SRenderText3D   textToRender;
        textToRender.m_Text = m_text;
        textToRender.m_Color = m_color.GetAsVector3();
        textToRender.m_EntityId = GetEntityId();
        AZ::TransformBus::EventResult(textToRender.m_Transform, textToRender.m_EntityId, &AZ::TransformBus::Events::GetWorldTM);
        DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::UpdateText, textToRender);
    }

    void DemoRoomDrawTextComponent::Deactivate()
    {
        AZ::TransformNotificationBus::Handler::BusDisconnect(GetEntityId());
        DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::RemoveText, GetEntityId());
    }

    void DemoRoomDrawTextComponent::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, const AZ::Transform& world)
    {
        SRenderText3D   textToRender;
        textToRender.m_Text = m_text;
        textToRender.m_Color = m_color.GetAsVector3();
        textToRender.m_Transform = world;
        textToRender.m_EntityId = GetEntityId();
        DemoRoomDrawTextEventsBus::Broadcast(&DemoRoomDrawTextEventsBus::Handler::UpdateText, textToRender);
    }
} // namespace DemoRoom
