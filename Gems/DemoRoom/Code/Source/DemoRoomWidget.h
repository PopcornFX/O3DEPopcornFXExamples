//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#if !defined(Q_MOC_RUN)
#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <QWidget>
#include <QGroupBox>
#endif

class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QListWidget;
class QListWidgetItem;

namespace DemoRoom
{
    class RoomListWidget
        : public QGroupBox
    {
        Q_OBJECT
    public:
        explicit RoomListWidget(QWidget* parent = nullptr);

        void DeleteRoom(QListWidgetItem* item);
        int RoomCount();
        void GetRoomSizes(AZStd::vector<AZ::u32> &rooms);
        void SetRoomSizes(const AZStd::vector<AZ::u32> &rooms);

     private:
         void AddRoom(int size);

    private:
        QListWidget* m_roomListWidget = nullptr;
    };

    class RoomWidget
        : public QWidget
    {
        Q_OBJECT
    public:
        explicit RoomWidget(int size, RoomListWidget* roomListWidget, QListWidgetItem* listWidgetItem, QWidget* parent = nullptr);

        AZ::u32 RoomSize();
        void RenameRoom(const QString& name);

    private Q_SLOTS:
        void ShowContextMenu(const QPoint& pos);

    private:
        QLabel* m_roomNameLabel = nullptr;
        QLineEdit* m_roomSizeInput = nullptr;

        QListWidgetItem* m_listWidgetItem;
        RoomListWidget* m_roomListWidget = nullptr;
    };

    class DemoRoomWidget
        : public QWidget
        , public AzToolsFramework::ToolsApplicationEvents::Bus::Handler
    {
        Q_OBJECT
    public:
        AZ_CLASS_ALLOCATOR(DemoRoomWidget, AZ::SystemAllocator, 0);

        explicit DemoRoomWidget(QWidget* parent = nullptr);
        ~DemoRoomWidget() override;

        ////////////////////////////////////////////////////////////////////////
        void AfterEntitySelectionChanged(const AzToolsFramework::EntityIdList&, const AzToolsFramework::EntityIdList&) override;

    private:
        // Entity
        QLineEdit* m_entityInput = nullptr;
        // Room List
        RoomListWidget* m_roomList;
        // Room Properties
        QCheckBox* m_lights = nullptr;
        QCheckBox* m_doubleHeight = nullptr;
        // Update Properties
        QGroupBox* m_updateProperties = nullptr;
        QCheckBox* m_renameStandEntities = nullptr;
        QCheckBox* m_deleteExessStandEntities = nullptr;
        // Button
        QPushButton* m_createButton = nullptr;

        AZ::EntityId m_selectedEntity;

        // Materials map
        using MaterialAssignmentMap = AZStd::unordered_map<AZStd::string, AZ::Data::AssetId>;

        void UpdateEntitySelection();

        void EnumerateDemoRoomMeshEntities(const AZStd::function<void(AZ::EntityId, bool)>& callback);
        void AddColliders();
        void SetMaterials();
        void CreateRooms();

        AZ::Data::AssetId GetMeshAssetId(const AZStd::string& assetPath, bool doubleHeight = false);
        bool FindMaterialAssetId(const AZStd::string& assetPath, const AZStd::string& slotLabel, MaterialAssignmentMap &map);
        AZ::EntityId CreateDemoRoomEntity();
        AZ::EntityId CreateEntityAtLocation(const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name);
        AZ::EntityId CreateStandEntityAtLocation(const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name, AZ::u32 room, AZ::u32 section);
        AZ::EntityId CreateLightEntityAtLocation(const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name);
        AZ::EntityId CreateMeshEntityAtLocation(AZ::Data::AssetId assetId, const AZ::Vector3& pos, AZ::EntityId parentId, const AZStd::string& name, bool addNonUniformScale = false);
        AZ::EntityId FindExistingStand(AZ::EntityId parentStandsId, AZ::u32 room, AZ::u32 section);
        void DeleteExessStands(AZ::EntityId parentStandsId, const AZStd::vector<AZ::u32>& rooms);

    };
}
