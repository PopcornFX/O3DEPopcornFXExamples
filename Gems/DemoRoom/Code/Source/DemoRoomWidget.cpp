//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Component/TransformBus.h>

#include <AzToolsFramework/Component/EditorComponentAPIBus.h>
#include <AzToolsFramework/Entity/EditorEntityAPIBus.h>
#include <AzToolsFramework/Entity/EditorEntityInfoBus.h>
#include <AzToolsFramework/Entity/EditorEntityHelpers.h>
#include <AzToolsFramework/ToolsComponents/EditorNonUniformScaleComponent.h>
#include <AzToolsFramework/Commands/EntityStateCommand.h>

#include <AzFramework/API/ApplicationAPI.h>

#include <LmbrCentral/Shape/SphereShapeComponentBus.h>

#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Mesh/MeshComponentConstants.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentConstants.h>
#include <AtomLyIntegration/CommonFeatures/CoreLights/CoreLightsConstants.h>
#include <AtomLyIntegration/CommonFeatures/CoreLights/AreaLightComponentConfig.h>

#include <Source/EditorColliderComponent.h>
#include <Source/EditorMeshColliderComponent.h>
#include <Source/EditorStaticRigidBodyComponent.h>

#include <QCheckBox>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>
#include <QMenu>

#include "Components/EditorDemoRoomComponent.h"
#include "Components/EditorDemoRoomStandComponent.h"
#include "Components/EditorDemoRoomDrawTextComponent.h"

#include <DemoRoomWidget.h>

using namespace AzToolsFramework;

namespace DemoRoom
{
    //////////////////////////////////////////////////////////////////////////
    // RoomListWidget
    //////////////////////////////////////////////////////////////////////////

    RoomListWidget::RoomListWidget(QWidget* parent)
        : QGroupBox(QObject::tr("Room List"), parent)
    {
        QVBoxLayout* roomsLayout = new QVBoxLayout();
        
        m_roomListWidget = new QListWidget(this);
        
        roomsLayout->addWidget(m_roomListWidget);
        
        QPushButton* addRoomButton = new QPushButton("Add Room", this);
        QObject::connect(addRoomButton, &QPushButton::clicked, this, [this]() {
            AddRoom(3);
        });
        roomsLayout->addWidget(addRoomButton);
        
        setLayout(roomsLayout);
        setMinimumHeight(200);
    }

    void RoomListWidget::DeleteRoom(QListWidgetItem* item)
    {
        if (m_roomListWidget->count() > 1)
            delete m_roomListWidget->takeItem(m_roomListWidget->row(item));
        for (int i = 0; i < m_roomListWidget->count(); ++i)
        {
            RoomWidget* roomWidget = qobject_cast<RoomWidget*>(m_roomListWidget->itemWidget(m_roomListWidget->item(i)));

            if (roomWidget != nullptr)
                roomWidget->RenameRoom(QString("Room %1").arg(i + 1));
        }
    }

    int RoomListWidget::RoomCount()
    {
        return m_roomListWidget->count();
    }

    void RoomListWidget::GetRoomSizes(AZStd::vector<AZ::u32> &rooms)
    {
        for (int i = 0; i < m_roomListWidget->count(); ++i)
        {
            RoomWidget* roomWidget = qobject_cast<RoomWidget*>(m_roomListWidget->itemWidget(m_roomListWidget->item(i)));

            rooms.push_back(roomWidget->RoomSize());
        }
    }

    void RoomListWidget::SetRoomSizes(const AZStd::vector<AZ::u32> &rooms)
    {
        if (rooms.size() == 0)
            return;
        m_roomListWidget->clear();
        for (auto size : rooms)
            AddRoom(size);
    }

    void RoomListWidget::AddRoom(int size)
    {
        QListWidgetItem* listWidgetItem = new QListWidgetItem(m_roomListWidget);
        RoomWidget* roomWidgetItem = new RoomWidget(size, this, listWidgetItem);
        listWidgetItem->setSizeHint(roomWidgetItem->sizeHint());
        m_roomListWidget->setItemWidget(listWidgetItem, roomWidgetItem);
    }

    //////////////////////////////////////////////////////////////////////////
    // RoomWidget
    //////////////////////////////////////////////////////////////////////////

    RoomWidget::RoomWidget(int size, RoomListWidget* roomListWidget, QListWidgetItem* listWidgetItem, QWidget* parent)
        : QWidget(parent)
    {
        QFormLayout* mainLayout = new QFormLayout(this);

        m_roomNameLabel = new QLabel(QString("Room %1").arg(roomListWidget->RoomCount()), this);
        mainLayout->addRow(m_roomNameLabel);

        m_roomSizeInput = new QLineEdit(this);
        m_roomSizeInput->setValidator(new QIntValidator(1, 100, this));
        m_roomSizeInput->setText(QString::number(size));
        mainLayout->addRow(QObject::tr("Size"), m_roomSizeInput);

        m_listWidgetItem = listWidgetItem;
        m_roomListWidget = roomListWidget;
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ShowContextMenu(const QPoint &)));
    }

    void RoomWidget::ShowContextMenu(const QPoint& pos)
    {
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Remove Room", this);
        connect(&action1, &QAction::triggered, this, [this]() {
            m_roomListWidget->DeleteRoom(m_listWidgetItem);
        });
        contextMenu.addAction(&action1);

        contextMenu.exec(mapToGlobal(pos));
    }

    void RoomWidget::RenameRoom(const QString& name)
    {
        m_roomNameLabel->setText(name);
    }

    AZ::u32 RoomWidget::RoomSize()
    {
        return m_roomSizeInput->text().toUInt();
    }

    //////////////////////////////////////////////////////////////////////////
    // DemoRoomWidget
    //////////////////////////////////////////////////////////////////////////

    DemoRoomWidget::DemoRoomWidget(QWidget* parent)
        : QWidget(parent)
    {
        setWindowTitle(QObject::tr("DemoRoom"));

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(20);

        // === Entity Group
        QGroupBox* entityGroup = new QGroupBox("Entity", this);
        QVBoxLayout* entityGroupLayout = new QVBoxLayout();
        m_entityInput = new QLineEdit(this);
        m_entityInput->setReadOnly(true);
        entityGroupLayout->addWidget(m_entityInput);

        QLabel* selectionLabel = new QLabel(QObject::tr("<i>(Select a DemoRoom entity to update it)</i>"), this);
        selectionLabel->setTextFormat(Qt::RichText);
        entityGroupLayout->addWidget(selectionLabel);

        entityGroup->setLayout(entityGroupLayout);
        mainLayout->addWidget(entityGroup);

        // === Rooms Group
        m_roomList = new RoomListWidget(this);
        mainLayout->addWidget(m_roomList);

        // === Properties Group
        QGroupBox* roomPropertiesWidget = new QGroupBox(QObject::tr("Room Properties"), this);
        QFormLayout* roomFormLayout = new QFormLayout();

        // Lights
        m_lights = new QCheckBox(this);
        m_lights->setCheckState(Qt::Checked);
        roomFormLayout->addRow(QObject::tr("Lights"), m_lights);

        // Double height
        m_doubleHeight = new QCheckBox(this);
        roomFormLayout->addRow(QObject::tr("Double Height"), m_doubleHeight);

        roomPropertiesWidget->setLayout(roomFormLayout);
        mainLayout->addWidget(roomPropertiesWidget);

        // === Update Properties
        m_updateProperties = new QGroupBox(QObject::tr("Update Properties"), this);
        QFormLayout* updateFormLayout = new QFormLayout();

        // Rename stand entities
        m_renameStandEntities = new QCheckBox(this);
        m_renameStandEntities->setCheckState(Qt::Checked);
        updateFormLayout->addRow(QObject::tr("Rename stand entities"), m_renameStandEntities);

        // Delete exess stand entities
        m_deleteExessStandEntities = new QCheckBox(this);
        m_deleteExessStandEntities->setCheckState(Qt::Checked);
        updateFormLayout->addRow(QObject::tr("Delete exess stand entities"), m_deleteExessStandEntities);

        QPushButton* addMaterialsButtons = new QPushButton("Add Colliders", this);
        QObject::connect(addMaterialsButtons, &QPushButton::clicked, this, [this]() {
            AddColliders();
        });
        updateFormLayout->addRow(addMaterialsButtons);

        QPushButton* addCollidersButtons = new QPushButton("Set Materials", this);
        QObject::connect(addCollidersButtons, &QPushButton::clicked, this, [this]() {
            SetMaterials();
        });
        updateFormLayout->addRow(addCollidersButtons);

        m_updateProperties->setLayout(updateFormLayout);
        mainLayout->addWidget(m_updateProperties);
        m_updateProperties->hide();

        // === Warning
        QLabel* warningLabel = new QLabel(QObject::tr("<i>(Make sure a level is loaded before creating a DemoRoom)</i>"), this);
        warningLabel->setTextFormat(Qt::RichText);
        mainLayout->addWidget(warningLabel);

        // === Create/Update DemoRoom button
        m_createButton = new QPushButton("Create DemoRoom", this);
        QObject::connect(m_createButton, &QPushButton::clicked, this, [this]() {
            CreateRooms();
        });
        mainLayout->addWidget(m_createButton);

        // Add stretch at bottom of the layout to fill any expanded space larger than what is needed,
        // so that if our tool is resized large our content will stay together
        mainLayout->addStretch();

        setLayout(mainLayout);

        UpdateEntitySelection();
        AzToolsFramework::ToolsApplicationEvents::Bus::Handler::BusConnect();
    }

    DemoRoomWidget::~DemoRoomWidget()
    {
        AzToolsFramework::ToolsApplicationEvents::Bus::Handler::BusDisconnect();
    }

    void DemoRoomWidget::AfterEntitySelectionChanged(const EntityIdList&, const EntityIdList&)
    {
        UpdateEntitySelection();
    }

    void DemoRoomWidget::UpdateEntitySelection()
    {
        m_selectedEntity = AZ::EntityId();

        EntityIdList selectedEntityIds;
        ToolsApplicationRequests::Bus::BroadcastResult(selectedEntityIds, &ToolsApplicationRequests::GetSelectedEntities);
        if (selectedEntityIds.size() == 1)
        {
            bool hasDemoRoomComponent = false;
            EditorComponentAPIBus::BroadcastResult( hasDemoRoomComponent, &EditorComponentAPIRequests::HasComponentOfType,
                selectedEntityIds[0], AZ::Uuid(EditorDemoRoomComponentTypeId));

            if (hasDemoRoomComponent)
                m_selectedEntity = selectedEntityIds[0];
        }

        if (m_selectedEntity.IsValid())
        {
            AZStd::string demoRoomName;
            EditorEntityInfoRequestBus::EventResult(demoRoomName, m_selectedEntity, &AzToolsFramework::EditorEntityInfoRequestBus::Events::GetName);

            m_entityInput->setText(demoRoomName.c_str());
            m_createButton->setText("Update entity");

            AZStd::vector<AZ::u32> rooms;
            bool lights;
            bool doubleHeight;
            EditorDemoRoomRequestBus::EventResult(rooms, m_selectedEntity, &EditorDemoRoomRequests::GetRoomSizes);
            EditorDemoRoomRequestBus::EventResult(lights, m_selectedEntity, &EditorDemoRoomRequests::GetLights);
            EditorDemoRoomRequestBus::EventResult(doubleHeight, m_selectedEntity, &EditorDemoRoomRequests::GetDoubleHeight);
            m_roomList->SetRoomSizes(rooms);
            m_lights->setCheckState(lights ? Qt::Checked : Qt::Unchecked);
            m_doubleHeight->setCheckState(doubleHeight ? Qt::Checked : Qt::Unchecked);
            m_updateProperties->show();
        }
        else
        {
            m_entityInput->setText("Create new entity");
            m_createButton->setText("Create entity");
            m_roomList->SetRoomSizes(AZStd::vector<AZ::u32>{ 3 });
            m_lights->setCheckState(Qt::Checked);
            m_doubleHeight->setCheckState(Qt::Unchecked);
            m_updateProperties->hide();
        }
    }

    void DemoRoomWidget::EnumerateDemoRoomMeshEntities(const AZStd::function<void(AZ::EntityId, bool)>& callback)
    {
        //Environment
        AZ::EntityId environmentEntityId;
        EditorDemoRoomRequestBus::EventResult(environmentEntityId, m_selectedEntity, &EditorDemoRoomRequests::GetEnvironmentEntityId);
        if (environmentEntityId.IsValid())
        {
            EntityIdSet allDescendents;
            ToolsApplicationRequests::Bus::BroadcastResult(allDescendents, &ToolsApplicationRequests::GatherEntitiesAndAllDescendents, EntityIdList{ environmentEntityId });
            for (auto childId : allDescendents)
            {
                bool hasMeshComponent = false;
                EditorComponentAPIBus::BroadcastResult(hasMeshComponent, &EditorComponentAPIRequests::HasComponentOfType, childId, AZ::Render::EditorMeshComponentTypeId);
                if (hasMeshComponent)
                {
                    callback(childId, false);
                }
            }
        }
        else
            AZ_Warning("DemoRoom", false, "Failed to find generated Environment child entity. Can't add colliders.");
        

        //Stands
        AZ::EntityId standsEntityId;
        EditorDemoRoomRequestBus::EventResult(standsEntityId, m_selectedEntity, &EditorDemoRoomRequests::GetStandsEntityId);
        if (standsEntityId.IsValid())
        {
            EntityIdSet allDescendents;
            ToolsApplicationRequests::Bus::BroadcastResult(allDescendents, &ToolsApplicationRequests::GatherEntitiesAndAllDescendents, EntityIdList{ standsEntityId });
            for (auto childId : allDescendents)
            {
                bool hasStandComponent = false;
                EditorComponentAPIBus::BroadcastResult(hasStandComponent, &EditorComponentAPIRequests::HasComponentOfType, childId, AZ::Uuid(EditorDemoRoomStandComponentTypeId));
                if (hasStandComponent)
                {
                    AZ::EntityId standMeshEntityId;
                    EditorDemoRoomStandRequestBus::EventResult(standMeshEntityId, childId, &EditorDemoRoomStandRequests::GetStandEntityId);
                    if (standMeshEntityId.IsValid())
                    {
                        bool hasMeshComponent = false;
                        EditorComponentAPIBus::BroadcastResult(hasMeshComponent, &EditorComponentAPIRequests::HasComponentOfType, standMeshEntityId, AZ::Render::EditorMeshComponentTypeId);
                        if (hasMeshComponent)
                        {
                            callback(standMeshEntityId, true);
                        }
                    }

                    AZ::EntityId nameplateMeshEntityId;
                    EditorDemoRoomStandRequestBus::EventResult(nameplateMeshEntityId, childId, &EditorDemoRoomStandRequests::GetNamePlateEntityId);
                    if (nameplateMeshEntityId.IsValid())
                    {
                        bool hasMeshComponent = false;
                        EditorComponentAPIBus::BroadcastResult(hasMeshComponent, &EditorComponentAPIRequests::HasComponentOfType, nameplateMeshEntityId, AZ::Render::EditorMeshComponentTypeId);
                        if (hasMeshComponent)
                        {
                            callback(nameplateMeshEntityId, false);
                        }
                    }
                }
            }
        }
        else
            AZ_Warning("DemoRoom", false, "Failed to find Stands child entity. Can't add colliders.");
    }

    void DemoRoomWidget::AddColliders()
    {
        auto actionCallback = [](AZ::EntityId entityId, bool isStandMesh)
        {
            //Update the collider IFN
            bool hasOldColliderComponent = false;
            EditorComponentAPIBus::BroadcastResult(hasOldColliderComponent, &EditorComponentAPIRequests::HasComponentOfType, entityId, PhysX::EditorColliderComponent::RTTI_Type());

            if (hasOldColliderComponent)
            {
                //In O3DE 23.05, Physics asset shape is now handled by EditorMeshColliderComponent.
                EditorComponentAPIRequests::GetComponentOutcome componentOutcome;
                EditorComponentAPIBus::BroadcastResult(componentOutcome, &EditorComponentAPIRequests::GetComponentOfType, entityId, PhysX::EditorColliderComponent::RTTI_Type());
                if (componentOutcome)
                {
                    AZStd::vector<AZ::EntityComponentIdPair> componentsToRemove;
                    componentsToRemove.push_back(componentOutcome.GetValue());
                    EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::RemoveComponents, componentsToRemove);
                }
                EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, entityId, PhysX::EditorMeshColliderComponent::RTTI_Type());
                EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, entityId, PhysX::EditorStaticRigidBodyComponent::RTTI_Type());
            }
            else
            {
                bool hasColliderComponent = false;
                EditorComponentAPIBus::BroadcastResult(hasColliderComponent, &EditorComponentAPIRequests::HasComponentOfType, entityId, PhysX::EditorMeshColliderComponent::RTTI_Type());
                if (isStandMesh && hasColliderComponent)
                {
                    EditorComponentAPIRequests::GetComponentOutcome componentOutcome;
                    EditorComponentAPIBus::BroadcastResult(componentOutcome, &EditorComponentAPIRequests::GetComponentOfType, entityId, PhysX::EditorMeshColliderComponent::RTTI_Type());
                    if (componentOutcome)
                    {
                        AZStd::vector<AZ::EntityComponentIdPair> componentsToRemove;
                        componentsToRemove.push_back(componentOutcome.GetValue());
                        EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::RemoveComponents, componentsToRemove);
                    }
                    EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, entityId, PhysX::EditorStaticRigidBodyComponent::RTTI_Type());
                    EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, entityId, PhysX::EditorMeshColliderComponent::RTTI_Type());
                }
                else if (!hasColliderComponent)
                {
                    EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, entityId, PhysX::EditorMeshColliderComponent::RTTI_Type());
                    EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, entityId, PhysX::EditorStaticRigidBodyComponent::RTTI_Type());
                }
            }
        };

        EnumerateDemoRoomMeshEntities(actionCallback);
    }

    void DemoRoomWidget::SetMaterials()
    {
        MaterialAssignmentMap materialAssignmentMap;
        // Materials
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "Material #207", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoDisplay_Outline_Inst", "Material #208", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoDisplay_Outline_Inst", "Material #245", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst3", "Material #247", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "Material #248", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "Material #246 Slot #4", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst3", "Material #260", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "Material #261", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "Material #289", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst3", "Material #394", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoRoomTiles_Inst", "Material #396", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoRoomTiles_Inst", "Material #398", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "Material #399", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst3", "Material #402", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "Material #403", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst2", "Material #451", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst_NamePlate", "SM_Base_Black", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "SM_Base_Gray", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst_NamePlate", "SM_NamePlate", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst_BaseFloor_Inst", "SM_Base_FlatWall", materialAssignmentMap))
            return;
        if (!FindMaterialAssetId("Materials/M_DemoWall_Inst", "SM_URoom_Wall_Gray", materialAssignmentMap))
            return;

        auto actionCallback = [&materialAssignmentMap](AZ::EntityId entityId, [[maybe_unused]] bool isStandMesh)
        {
            bool hasMaterialComponent = false;
            EditorComponentAPIBus::BroadcastResult(hasMaterialComponent, &EditorComponentAPIRequests::HasComponentOfType, entityId, AZ::Render::EditorMaterialComponentTypeId);
            if (hasMaterialComponent)
            {
                AZ::Render::MaterialAssignmentMap namePlateMaterials;
                AZ::Render::MaterialComponentRequestBus::EventResult(   namePlateMaterials, entityId,
#if PK_O3DE_MAJOR_VERSION > 2205
                                                                        &AZ::Render::MaterialComponentRequestBus::Events::GetDefautMaterialMap);
#else
                                                                        &AZ::Render::MaterialComponentRequestBus::Events::GetOriginalMaterialAssignments);
#endif
                for (const auto& materialPair : namePlateMaterials)
                {
                    if (materialPair.first.IsDefault())
                        continue;

                    AZStd::string label;
                    AZ::Render::MaterialComponentRequestBus::EventResult(   label, entityId,
#if PK_O3DE_MAJOR_VERSION > 2205
                                                                            &AZ::Render::MaterialComponentRequestBus::Events::GetMaterialLabel,
#else
                                                                            &AZ::Render::MaterialComponentRequestBus::Events::GetMaterialSlotLabel,
#endif
                                                                            materialPair.first);

                    auto materialIt = materialAssignmentMap.find(label);

                    if (materialIt != materialAssignmentMap.end())
                    {
                        AZ::Render::MaterialComponentRequestBus::Event( entityId,
#if PK_O3DE_MAJOR_VERSION > 2205
                                                                        &AZ::Render::MaterialComponentRequestBus::Events::SetMaterialAssetId,
#else
                                                                        &AZ::Render::MaterialComponentRequestBus::Events::SetMaterialOverride,
#endif
                                                                        materialPair.first, materialIt->second);
#if PK_O3DE_MAJOR_VERSION > 2205
                        AZ::Render::MaterialConsumerNotificationBus::Event( entityId,
                            &AZ::Render::MaterialConsumerNotificationBus::Events::OnMaterialAssignmentSlotsChanged);
#else
                        AZ::Render::MaterialReceiverNotificationBus::Event( entityId,
                            &AZ::Render::MaterialReceiverNotificationBus::Events::OnMaterialAssignmentsChanged);
#endif
                    }
                    else
                        AZ_Error("DemoRoom", false, "No material defined for slot \"%s\"", label.c_str());
                }
            }
        };

        EnumerateDemoRoomMeshEntities(actionCallback);
    }

    void DemoRoomWidget::CreateRooms()
    {
        AzToolsFramework::ToolsApplicationEvents::Bus::Handler::BusDisconnect();

        // UI Room Parameters
        AZStd::vector<AZ::u32> rooms;
        m_roomList->GetRoomSizes(rooms);
        const bool doubleHeight = m_doubleHeight->checkState() == Qt::Checked;
        const bool lights = m_lights->checkState() == Qt::Checked;

        // UI Update Parameters
        const bool renameStandEntities = m_renameStandEntities->checkState() == Qt::Checked;
        const bool deleteExessStandEntities = m_deleteExessStandEntities->checkState() == Qt::Checked;

        // == Assets
        const AZ::Vector3 trimOffset(3.0f, 0.0f, 0.0f);
        AZ::Data::AssetId trimAssetId = GetMeshAssetId("Objects/SM_DemoRoomTrim", doubleHeight);
        if (!trimAssetId.IsValid())
            return;

        const AZ::Vector3 divierOffset(1.5f, 0.0f, 0.0f);
        AZ::Data::AssetId dividerAssetId = GetMeshAssetId("Objects/SM_DemoDivider_1");
        if (!trimAssetId.IsValid())
            return;

        AZ::Data::AssetId clampAssetId = GetMeshAssetId("Objects/SM_DemoRoomClamp", doubleHeight);
        if (!clampAssetId.IsValid())
            return;

        const AZ::Transform firstWallOffset = AZ::Transform(AZ::Vector3(0.0f, 19.0f, 0.0f),
                                                            AZ::Quaternion::CreateFromEulerAnglesDegrees(AZ::Vector3(0.0f, 0.0f, 180.0f)),
                                                            1.0f);
        AZ::Data::AssetId wallAssetId = GetMeshAssetId("Objects/SM_URoom_Wall", doubleHeight);
        if (!wallAssetId.IsValid())
            return;

        const float backWallScaleFactor = 1.75f; // sectionCount / backWallScaleFactor
        AZ::Data::AssetId backWallAssetId = GetMeshAssetId("Objects/SM_DemoRoomBackWall", doubleHeight);
        if (!backWallAssetId.IsValid())
            return;

        const AZ::Vector3 sectionOffset(10.0f, 0.0f, 0.0f);
        AZ::Data::AssetId sectionAssetId = GetMeshAssetId("Objects/SM_DemoRoomU", doubleHeight);
        if (!sectionAssetId.IsValid())
            return;

        const AZ::Vector3 lightOffset = sectionOffset + AZ::Vector3(0.0f, 15.0f, 5.0f);
        const AZ::Vector3 standOffset = AZ::Vector3(sectionOffset.GetX() / 2.0f, 15.0f, 0.0f);

        // Create or get existing DemoRoom entity
        ScopedUndoBatch undo("Create DemoRoom entity");

        AZ::EntityId demoRoomEntityId;
        AZ::EntityId standsEntityId;

        if (m_selectedEntity.IsValid())
        {
            demoRoomEntityId = m_selectedEntity;

            AZ::EntityId oldEnvironmentEntityId;
            EditorDemoRoomRequestBus::EventResult(standsEntityId, m_selectedEntity, &EditorDemoRoomRequests::GetStandsEntityId);
            EditorDemoRoomRequestBus::EventResult(oldEnvironmentEntityId, m_selectedEntity, &EditorDemoRoomRequests::GetEnvironmentEntityId);
            if (!standsEntityId.IsValid())
            {
                AZ_Error("DemoRoom", false, "Failed to find stands child entity. Can't update the DemoRoom");
                return;
            }
            if (!oldEnvironmentEntityId.IsValid())
            {
                AZ_Error("DemoRoom", false, "Failed to find generated environment child entity. Can't update the DemoRoom");
                return;
            }
            ToolsApplicationRequests::Bus::Broadcast(&ToolsApplicationRequests::DeleteEntityAndAllDescendants, oldEnvironmentEntityId);
        }
        else
        {
            demoRoomEntityId = CreateDemoRoomEntity();
            standsEntityId = CreateEntityAtLocation(AZ::Vector3::CreateZero(), demoRoomEntityId, "Stands");
        }

        AZ::EntityId environmentEntityId = CreateEntityAtLocation(AZ::Vector3::CreateZero(), demoRoomEntityId, "Generated");

        AZ::Vector3 roomPos = AZ::Vector3::CreateZero();

        // First Wall
        AZ::EntityId firstDividerEntityId = CreateEntityAtLocation(roomPos, environmentEntityId, "Divider");
        AZ::EntityId firstWallEntityId = CreateMeshEntityAtLocation(wallAssetId, AZ::Vector3::CreateZero(), firstDividerEntityId, "Wall");
        AZ::TransformBus::Event(firstWallEntityId, &AZ::TransformInterface::SetLocalTM, firstWallOffset);
        CreateMeshEntityAtLocation(trimAssetId, AZ::Vector3::CreateZero(), firstDividerEntityId, "Trim");
        CreateMeshEntityAtLocation(clampAssetId, AZ::Vector3::CreateZero(), firstDividerEntityId, "Clamp");
        roomPos += trimOffset;

        AzToolsFramework::EntityOrderArray standEntitiesOrderArray;

        for (AZ::u32 iRoom = 0; iRoom < rooms.size(); ++iRoom)
        {
            const AZ::u32 roomSize = rooms[iRoom];

            if (iRoom != 0)
            {
                // Divider between rooms
                AZ::EntityId dividerEntityId = CreateEntityAtLocation(roomPos, environmentEntityId, "Divider");
                CreateMeshEntityAtLocation(dividerAssetId, divierOffset, dividerEntityId, "Divider");
                CreateMeshEntityAtLocation(trimAssetId, AZ::Vector3::CreateZero(), dividerEntityId, "Trim");
                CreateMeshEntityAtLocation(clampAssetId, AZ::Vector3::CreateZero(), dividerEntityId, "Clamp");
                roomPos += trimOffset;
            }

            const QString roomName = QString("Room_%1").arg(iRoom + 1);
            AZ::EntityId roomEntityId = CreateEntityAtLocation(roomPos, environmentEntityId, roomName.toUtf8().constData());

            // Sections
            AZ::Vector3 sectionPos = AZ::Vector3::CreateZero();

            // if there is only one section, place one light in the center
            if (lights && roomSize == 1)
                CreateLightEntityAtLocation(sectionPos + lightOffset / AZ::Vector3(2.0f, 1.0f, 1.0f), roomEntityId, "Light");

            for (AZ::u32 iSection = 0; iSection < roomSize; ++iSection)
            {
                const QString sectionName = QString("Section_%1").arg(iSection + 1);
                const QString standName = QString("Stand_%1").arg(standEntitiesOrderArray.size() + 1);

                CreateMeshEntityAtLocation(sectionAssetId, sectionPos, roomEntityId, sectionName.toUtf8().constData());

                // Stand
                AZ::EntityId standEntityId = FindExistingStand(standsEntityId, iRoom, iSection);
                if (standEntityId.IsValid())
                {
                    if (renameStandEntities)
                        EditorEntityAPIBus::Event(standEntityId, &EditorEntityAPIRequests::SetName, standName.toUtf8().constData());
                    AZ::TransformBus::Event(standEntityId, &AZ::TransformInterface::SetLocalTranslation, roomPos + sectionPos + standOffset);
                }
                else
                    standEntityId = CreateStandEntityAtLocation(roomPos + sectionPos + standOffset, standsEntityId, standName.toUtf8().constData(), iRoom, iSection);
                standEntitiesOrderArray.push_back(standEntityId);

                // Light
                if (lights && iSection != roomSize - 1)
                    CreateLightEntityAtLocation(sectionPos + lightOffset, roomEntityId, "Light");

                sectionPos += sectionOffset;
            }

            // Back wall
            AZ::EntityId backWallEntityId = CreateMeshEntityAtLocation(backWallAssetId, AZ::Vector3::CreateZero(), roomEntityId, "BackWall", true);
            AZ::NonUniformScaleRequestBus::Event(backWallEntityId, &AZ::NonUniformScaleRequests::SetScale, AZ::Vector3(roomSize / backWallScaleFactor, 1.0f, 1.0f));

            roomPos += sectionOffset * (float)roomSize;
        }

        // Last Wall
        AZ::EntityId lastDividerEntityId = CreateEntityAtLocation(roomPos, environmentEntityId, "Divider");
        CreateMeshEntityAtLocation(wallAssetId, trimOffset, lastDividerEntityId, "Wall");
        CreateMeshEntityAtLocation(trimAssetId, AZ::Vector3::CreateZero(), lastDividerEntityId, "Trim");
        CreateMeshEntityAtLocation(clampAssetId, AZ::Vector3::CreateZero(), lastDividerEntityId, "Clamp");
        roomPos += trimOffset;

        // Delete Exess stands
        if (deleteExessStandEntities)
            DeleteExessStands(standsEntityId, rooms);

        // Set stand entities order
        SetEntityChildOrder(standsEntityId, standEntitiesOrderArray);

        // Lock Generated environment entity
        AzToolsFramework::EditorLockComponentRequestBus::Event(environmentEntityId, &AzToolsFramework::EditorLockComponentRequests::SetLocked, true);

        // DemoRoom Component Config
        EditorDemoRoomRequestBus::Event(demoRoomEntityId, &EditorDemoRoomRequests::SetStandsEntityId, standsEntityId);
        EditorDemoRoomRequestBus::Event(demoRoomEntityId, &EditorDemoRoomRequests::SetEnvironmentEntityId, environmentEntityId);
        EditorDemoRoomRequestBus::Event(demoRoomEntityId, &EditorDemoRoomRequests::SetRoomSizes, rooms);
        EditorDemoRoomRequestBus::Event(demoRoomEntityId, &EditorDemoRoomRequests::SetLights, lights);
        EditorDemoRoomRequestBus::Event(demoRoomEntityId, &EditorDemoRoomRequests::SetDoubleHeight, doubleHeight);

        bool isPrefabSystemEnabled = false;
        AzFramework::ApplicationRequests::Bus::BroadcastResult(isPrefabSystemEnabled, &AzFramework::ApplicationRequests::IsPrefabSystemEnabled);

        if (!isPrefabSystemEnabled)
        {
            AZ::Entity* demoRoomEntity = nullptr;
            AZ::ComponentApplicationBus::BroadcastResult(demoRoomEntity, &AZ::ComponentApplicationRequests::FindEntity, demoRoomEntityId);
            if (demoRoomEntity == nullptr)
            {
                AZ_Error("DemoRoom", false, "Failed to find entity \"DemoRoom\"");
                return;
            }
            // Prepare undo command last so it captures the final state of the entity.
            EntityCreateCommand* command = aznew EntityCreateCommand(static_cast<AZ::u64>(demoRoomEntity->GetId()));
            command->Capture(demoRoomEntity);
            command->SetParent(undo.GetUndoBatch());
        }

        AzToolsFramework::ToolsApplicationEvents::Bus::Handler::BusConnect();
        // Select DemoRoom entity
        ToolsApplicationRequests::Bus::Broadcast(&ToolsApplicationRequests::SetSelectedEntities, EntityIdList{ demoRoomEntityId });
    }

    AZ::Data::AssetId DemoRoomWidget::GetMeshAssetId(const AZStd::string& assetPath, bool doubleHeight /*= false*/)
    {
        const AZStd::string fullPath = assetPath + (doubleHeight ? "2" : "") + ".azmodel";
        AZ::Data::AssetId assetId;
        AZ::Data::AssetCatalogRequestBus::BroadcastResult(  assetId, &AZ::Data::AssetCatalogRequestBus::Events::GetAssetIdByPath,
                                                            fullPath.c_str(), AZ::Data::s_invalidAssetType, false);
        if (!assetId.IsValid())
        {
            AZ_Error("DemoRoom", false, "Failed to find mesh \"%s\"", fullPath.c_str());
        }
        return assetId;
    }

    bool DemoRoomWidget::FindMaterialAssetId(const AZStd::string& assetPath, const AZStd::string& slotLabel, MaterialAssignmentMap &map)
    {
        const AZStd::string fullPath = assetPath + ".azmaterial";
        AZ::Data::AssetId assetId;
        AZ::Data::AssetCatalogRequestBus::BroadcastResult(  assetId, &AZ::Data::AssetCatalogRequestBus::Events::GetAssetIdByPath,
                                                            fullPath.c_str(), AZ::Data::s_invalidAssetType, false);
        if (!assetId.IsValid())
        {
            AZ_Error("DemoRoom", false, "Failed to find material \"%s\"", fullPath.c_str());
            return false;
        }
        map[slotLabel] = assetId;
        return true;
    }

    AZ::EntityId DemoRoomWidget::CreateEntityAtLocation(const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name)
    {
        AZ::EntityId entityId;
        EditorRequests::Bus::BroadcastResult(entityId, &EditorRequests::CreateNewEntityAtPosition, pos, parentId);
        EditorEntityAPIBus::Event(entityId, &EditorEntityAPIRequests::SetName, name.c_str());
        return entityId;
    }

    AZ::EntityId DemoRoomWidget::CreateDemoRoomEntity()
    {
        AZ::EntityId entityId = CreateEntityAtLocation(AZ::Vector3::CreateZero(), AZ::EntityId(), "DemoRoom");

        EditorComponentAPIBus::Broadcast(   &EditorComponentAPIRequests::AddComponentOfType, entityId,
                                            AZ::Uuid(EditorDemoRoomComponentTypeId));
        return entityId;
    }

    AZ::EntityId DemoRoomWidget::CreateStandEntityAtLocation(const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name, AZ::u32 room, AZ::u32 section)
    {
        AZ::EntityId entityId = CreateEntityAtLocation(pos, parentId, name);

        EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, entityId, AZ::Uuid(EditorDemoRoomStandComponentTypeId));

        AZ::EntityId standEntityId = CreateEntityAtLocation(AZ::Vector3::CreateZero(), entityId, "Stand");
        EditorComponentAPIBus::Broadcast(   &EditorComponentAPIRequests::AddComponentsOfType, standEntityId,
                                            AZ::ComponentTypeList{ AZ::Render::EditorMeshComponentTypeId, AZ::Render::EditorMaterialComponentTypeId });

        AZ::EntityId nameplateEntityId = CreateEntityAtLocation(AZ::Vector3::CreateZero(), standEntityId, "Nameplate");
        EditorComponentAPIBus::Broadcast(   &EditorComponentAPIRequests::AddComponentsOfType, nameplateEntityId,
                                            AZ::ComponentTypeList{ AZ::Render::EditorMeshComponentTypeId, AZ::Render::EditorMaterialComponentTypeId });

        AZ::EntityId nameplateTextEntityId = CreateEntityAtLocation(AZ::Vector3::CreateZero(), nameplateEntityId, "Text");
        const AZ::Transform textTransform = AZ::Transform(  AZ::Vector3(-0.225f, 0.0f, 0.225f),
                                                            AZ::Quaternion::CreateFromEulerAnglesDegrees(AZ::Vector3(-30.0f, 0.0f, 0.0f)),
                                                            0.5f);
        AZ::TransformBus::Event(nameplateTextEntityId, &AZ::TransformInterface::SetLocalTM, textTransform);
        EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentOfType, nameplateTextEntityId, AZ::Uuid(EditorDemoRoomDrawTextComponentTypeId));
        EditorDemoRoomDrawTextRequestBus::Event(nameplateTextEntityId, &EditorDemoRoomDrawTextRequests::SetText, "Name");
        EditorDemoRoomDrawTextRequestBus::Event(nameplateTextEntityId, &EditorDemoRoomDrawTextRequests::SetColor, AZ::Color(1.0f));

        EditorDemoRoomStandRequestBus::Event(entityId, &EditorDemoRoomStandRequests::SetStandEntityId, standEntityId);
        EditorDemoRoomStandRequestBus::Event(entityId, &EditorDemoRoomStandRequests::SetNamePlateEntityId, nameplateEntityId);
        EditorDemoRoomStandRequestBus::Event(entityId, &EditorDemoRoomStandRequests::SetRoom, room);
        EditorDemoRoomStandRequestBus::Event(entityId, &EditorDemoRoomStandRequests::SetSection, section);

        return entityId;
    }

    AZ::EntityId DemoRoomWidget::CreateLightEntityAtLocation(const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name)
    {
        AZ::EntityId entityId = CreateEntityAtLocation(pos, parentId, name);

        EditorComponentAPIBus::Broadcast(   &EditorComponentAPIRequests::AddComponentsOfType, entityId,
                                            AZ::ComponentTypeList{ LmbrCentral::EditorSphereShapeComponentTypeId, AZ::Render::EditorAreaLightComponentTypeId });

        return entityId;
    }

    AZ::EntityId DemoRoomWidget::CreateMeshEntityAtLocation(AZ::Data::AssetId assetId, const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name, bool addNonUniformScale /*= false*/)
    {
        AZ::EntityId entityId = CreateEntityAtLocation(pos, parentId, name);

        AZ::ComponentTypeList	newComponents;
        newComponents.push_back(AZ::Render::EditorMeshComponentTypeId);
        newComponents.push_back(AZ::Render::EditorMaterialComponentTypeId);
        if (addNonUniformScale)
            newComponents.push_back(Components::EditorNonUniformScaleComponent::RTTI_Type());

        EditorComponentAPIBus::Broadcast(&EditorComponentAPIRequests::AddComponentsOfType, entityId, newComponents);

        AZ::Render::MeshComponentRequestBus::Event(entityId, &AZ::Render::MeshComponentRequests::SetModelAssetId, assetId);

        return entityId;
    }

    AZ::EntityId DemoRoomWidget::FindExistingStand(AZ::EntityId parentStandsId, AZ::u32 room, AZ::u32 section)
    {
        EntityIdList children;
        EditorEntityInfoRequestBus::EventResult(children, parentStandsId, &AzToolsFramework::EditorEntityInfoRequestBus::Events::GetChildren);
        for (auto childId : children)
        {
            AZ::u32 childRoom;
            AZ::u32 childSection;
            EditorDemoRoomStandRequestBus::EventResult(childRoom, childId, &EditorDemoRoomStandRequests::GetRoom);
            EditorDemoRoomStandRequestBus::EventResult(childSection, childId, &EditorDemoRoomStandRequests::GetSection);
            if (childRoom == room && childSection == section)
                return childId;
        }
        return AZ::EntityId();
    }

    void DemoRoomWidget::DeleteExessStands(AZ::EntityId parentStandsId, const AZStd::vector<AZ::u32>& rooms)
    {
        EntityIdList children;
        EditorEntityInfoRequestBus::EventResult(children, parentStandsId, &AzToolsFramework::EditorEntityInfoRequestBus::Events::GetChildren);
        for (auto childId : children)
        {
            AZ::u32 childRoom;
            AZ::u32 childSection;
            EditorDemoRoomStandRequestBus::EventResult(childRoom, childId, &EditorDemoRoomStandRequests::GetRoom);
            EditorDemoRoomStandRequestBus::EventResult(childSection, childId, &EditorDemoRoomStandRequests::GetSection);
            if (childRoom >= rooms.size() || childSection >= rooms[childRoom])
                ToolsApplicationRequests::Bus::Broadcast(&ToolsApplicationRequests::DeleteEntityById, childId);
        }
    }
}

#include <moc_DemoRoomWidget.cpp>
