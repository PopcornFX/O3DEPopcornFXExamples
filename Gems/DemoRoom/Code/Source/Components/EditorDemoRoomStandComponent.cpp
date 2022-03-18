//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/TransformBus.h>

#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentBus.h>

#include "EditorDemoRoomStandComponent.h"

using namespace AzToolsFramework;

namespace DemoRoom
{
    const char* EditorDemoRoomStandComponent::s_standMeshes[] = {
        "Objects/DemoDisplay_Base01.azmodel",
        "Objects/DemoDisplay_Base02.azmodel",
        "Objects/SM_DemoDisplay03_Combined.azmodel",
        //"Objects/DemoDisplay_Base04.azmodel",
    };
    static_assert(AZ_ARRAY_SIZE(EditorDemoRoomStandComponent::s_standMeshes) == (int)EditorDemoRoomStandComponent::StandType::_MaxValue);

    AZ::Vector3 EditorDemoRoomStandComponent::s_standOffsets[] = {
        AZ::Vector3(0.0f, 0.12f, -0.18f),
        AZ::Vector3(0.0f, 0.0f, -0.05f),
        AZ::Vector3(0.0f, 0.12f, -0.18f),
        //AZ::Vector3(0.0f, 0.0f, 0.0f),
    };
    static_assert(AZ_ARRAY_SIZE(EditorDemoRoomStandComponent::s_standOffsets) == (int)EditorDemoRoomStandComponent::StandType::_MaxValue);

    AZ::Vector3 EditorDemoRoomStandComponent::s_namePlateOffsets[] = {
        AZ::Vector3(0.0f, -1.34f, 0.27f),
        AZ::Vector3(0.0f, -1.7f, 0.29f),
        AZ::Vector3(0.0f, -1.34f, 0.27f),
        //AZ::Vector3(0.0f, -1.0f, 0.0f),
    };
    static_assert(AZ_ARRAY_SIZE(EditorDemoRoomStandComponent::s_namePlateOffsets) == (int)EditorDemoRoomStandComponent::StandType::_MaxValue);

    void EditorDemoRoomStandComponent::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (serializeContext)
        {
            serializeContext->Class<EditorDemoRoomStandComponent, EditorComponentBase>()
                ->Version(0)
                ->Field("Type", &EditorDemoRoomStandComponent::m_type)
                ->Field("StandEntityId", &EditorDemoRoomStandComponent::m_standEntityId)
                ->Field("NameplateEntityId", &EditorDemoRoomStandComponent::m_namePlateEntityId)
                ->Field("Room", &EditorDemoRoomStandComponent::m_room)
                ->Field("Section", &EditorDemoRoomStandComponent::m_section)
                ;

            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<EditorDemoRoomStandComponent>(
                    "DemoRoom Stand", "Setup a showcase Stand")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "DemoRoom")
                    //->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/DemoRoomStand.svg")
                    //->Attribute(AZ::Edit::Attributes::ViewportIcon, "Icons/Components/Viewport/DemoRoomStand.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
                    ->DataElement(AZ::Edit::UIHandlers::ComboBox, &EditorDemoRoomStandComponent::m_type, "Type", "Stand Type.")
                    ->EnumAttribute(StandType::Type1, "Type 1")
                    ->EnumAttribute(StandType::Type2, "Type 2")
                    ->EnumAttribute(StandType::Type3, "Type 3")
                    //->EnumAttribute(StandType::Type4, "Type 4")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorDemoRoomStandComponent::OnTypeUpdate)
                    ->DataElement(0, &EditorDemoRoomStandComponent::m_standEntityId, "Stand Entity", "")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorDemoRoomStandComponent::OnStandEntityUpdate)
                    ->DataElement(0, &EditorDemoRoomStandComponent::m_namePlateEntityId, "NamePlate Entity", "")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorDemoRoomStandComponent::OnNamePlateEntityUpdate)
                    ->ClassElement(AZ::Edit::ClassElements::Group, "DemoRoom properties")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, false)
                    ->DataElement(0, &EditorDemoRoomStandComponent::m_room, "Room", "")
                    ->DataElement(0, &EditorDemoRoomStandComponent::m_section, "Section", "")
                    ;
            }
        }
    }

    void EditorDemoRoomStandComponent::BuildGameEntity([[maybe_unused]] AZ::Entity* gameEntity)
    {
    }

    void EditorDemoRoomStandComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("DemoRoomStandService"));
    }

    void EditorDemoRoomStandComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("DemoRoomStandService"));
    }

    void EditorDemoRoomStandComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void EditorDemoRoomStandComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void EditorDemoRoomStandComponent::Init()
    {
    }

    void EditorDemoRoomStandComponent::Activate()
    {
        EditorDemoRoomStandRequestBus::Handler::BusConnect(GetEntityId());
    }

    void EditorDemoRoomStandComponent::Deactivate()
    {
        EditorDemoRoomStandRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void EditorDemoRoomStandComponent::OnTypeUpdate()
    {
        OnStandEntityUpdate();
        OnNamePlateEntityUpdate();
    }

    void EditorDemoRoomStandComponent::OnStandEntityUpdate()
    {
        if (m_standEntityId.IsValid())
        {
            AZ::Data::AssetId assetId;

            const char* path = s_standMeshes[(int)m_type];
            AZ::Data::AssetCatalogRequestBus::BroadcastResult(  assetId, &AZ::Data::AssetCatalogRequestBus::Events::GetAssetIdByPath,
                                                                path, AZ::Data::s_invalidAssetType, false);

            AZ::Render::MeshComponentRequestBus::Event(m_standEntityId, &AZ::Render::MeshComponentRequests::SetModelAssetId, assetId);
            AZ::TransformBus::Event(m_standEntityId, &AZ::TransformBus::Events::SetLocalTranslation, s_standOffsets[(int)m_type]);
        }
    }

    void EditorDemoRoomStandComponent::OnNamePlateEntityUpdate()
    {
        if (m_namePlateEntityId.IsValid())
        {
            AZ::Data::AssetId assetId;
            const AZStd::string path = "Objects/SM_NamePlate.azmodel";
            AZ::Data::AssetCatalogRequestBus::BroadcastResult(  assetId, &AZ::Data::AssetCatalogRequestBus::Events::GetAssetIdByPath,
                                                                path.c_str(), AZ::Data::s_invalidAssetType, false);

            AZ::Render::MeshComponentRequestBus::Event(m_namePlateEntityId, &AZ::Render::MeshComponentRequests::SetModelAssetId, assetId);
            AZ::TransformBus::Event(m_namePlateEntityId, &AZ::TransformBus::Events::SetLocalTranslation, s_namePlateOffsets[(int)m_type]);
        }
    }
} // namespace DemoRoom
