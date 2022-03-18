//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>
#include "EditorDemoRoomBus.h"

namespace DemoRoom
{
    static constexpr const char* const EditorDemoRoomComponentTypeId = "{BC352E4B-17F8-433B-AA60-E1459F166C9B}";

    class EditorDemoRoomComponent
        : public AzToolsFramework::Components::EditorComponentBase
        , public EditorDemoRoomRequestBus::Handler
    {
    public:
        AZ_EDITOR_COMPONENT(EditorDemoRoomComponent, EditorDemoRoomComponentTypeId, AzToolsFramework::Components::EditorComponentBase);
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
        AZ::EntityId GetStandsEntityId() const override { return m_standsEntityId; }
        void SetStandsEntityId(const AZ::EntityId& standsEntityId) override { m_standsEntityId = standsEntityId; }

        AZ::EntityId GetEnvironmentEntityId() const override { return m_environmentEntityId; }
        void SetEnvironmentEntityId(const AZ::EntityId& environmentEntityId) override { m_environmentEntityId = environmentEntityId; }

        AZStd::vector<AZ::u32> GetRoomSizes() const override { return m_roomSizes; }
        void SetRoomSizes(const AZStd::vector<AZ::u32>& rooms) override { m_roomSizes = rooms; }

        bool GetLights() const override { return m_lights; }
        void SetLights(bool lights) override { m_lights = lights; }

        bool GetDoubleHeight() const override { return m_doubleHeight; }
        void SetDoubleHeight(bool doubleHeight) override { m_doubleHeight = doubleHeight; }

    private:
        AZ::EntityId m_standsEntityId;
        AZ::EntityId m_environmentEntityId;
        AZStd::vector<AZ::u32> m_roomSizes;
        bool    m_lights;
        bool    m_doubleHeight;
    };
}
