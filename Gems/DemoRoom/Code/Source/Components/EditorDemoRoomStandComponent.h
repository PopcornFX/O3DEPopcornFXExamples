//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>
#include "EditorDemoRoomStandBus.h"

namespace DemoRoom
{
    static constexpr const char* const EditorDemoRoomStandComponentTypeId = "{49931260-D3D4-4EF5-BC9B-EC3967B17042}";

    class EditorDemoRoomStandComponent
        : public AzToolsFramework::Components::EditorComponentBase
        , public EditorDemoRoomStandRequestBus::Handler
    {
    public:
        AZ_EDITOR_COMPONENT(EditorDemoRoomStandComponent, EditorDemoRoomStandComponentTypeId, AzToolsFramework::Components::EditorComponentBase);
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
        AZ::EntityId GetStandEntityId() const override { return m_standEntityId; }
        void SetStandEntityId(AZ::EntityId entityId) override { m_standEntityId = entityId; OnStandEntityUpdate(); }
        AZ::EntityId GetNamePlateEntityId() const override { return m_namePlateEntityId; }
        void SetNamePlateEntityId(AZ::EntityId entityId) override { m_namePlateEntityId = entityId; OnNamePlateEntityUpdate(); }
        AZ::u32 GetRoom() const override { return m_room; }
        void SetRoom(AZ::u32 room) override { m_room = room; }
        AZ::u32 GetSection() const override { return m_section; }
        void SetSection(AZ::u32 section) override { m_section = section; }

        enum class StandType
        {
            Type1,
            Type2,
            Type3,
            //Type4,
            _MaxValue
        };

        StandType m_type = StandType::Type1;

        AZ::EntityId m_standEntityId;
        AZ::EntityId m_namePlateEntityId;

        AZ::u32 m_room = 0;
        AZ::u32 m_section = 0;

        static const char* s_standMeshes[];
        static AZ::Vector3 s_standOffsets[];
        static AZ::Vector3 s_namePlateOffsets[];

    private:
        void OnTypeUpdate();
        void OnStandEntityUpdate();
        void OnNamePlateEntityUpdate();
    };
}
