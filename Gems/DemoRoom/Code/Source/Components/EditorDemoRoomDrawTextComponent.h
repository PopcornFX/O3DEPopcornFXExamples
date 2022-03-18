//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>
#include <AzCore/Component/TransformBus.h>
#include "EditorDemoRoomDrawTextBus.h"

namespace DemoRoom
{
    static constexpr const char* const EditorDemoRoomDrawTextComponentTypeId = "{E49F32E7-ECFB-4B29-9B1F-08ABEF9BEE85}";

    class EditorDemoRoomDrawTextComponent
        : public AzToolsFramework::Components::EditorComponentBase
        , public AZ::TransformNotificationBus::Handler
        , public EditorDemoRoomDrawTextRequestBus::Handler
    {
    public:
        AZ_COMPONENT(EditorDemoRoomDrawTextComponent, EditorDemoRoomDrawTextComponentTypeId, AzToolsFramework::Components::EditorComponentBase);
        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // EditorComponentBase
        void BuildGameEntity(AZ::Entity* gameEntity) override;

        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // EditorDemoRoomRequestBus::Handler overrides ...
        const AZStd::string& GetText() const override { return m_text; }
        void SetText(const AZStd::string& text) override { m_text = text; OnPropertyUpdate(); }
        void SetColor(const AZ::Color& color) override { m_color = color; OnPropertyUpdate(); }

        //////////////////////////////////////////////////////////////////////////
        // TransformNotificationBus
        //! Called when the local transform of the entity has changed. Local transform update always implies world transform change too.
        virtual void	OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;
        //////////////////////////////////////////////////////////////////////////

    private:
        void OnPropertyUpdate();
        AZStd::string m_text;
        AZ::Color m_color = AZ::Color(1.0f);
    };
}
