//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <DemoRoomModuleInterface.h>
#include <DemoRoomEditorSystemComponent.h>
#include <Components/EditorDemoRoomStandComponent.h>
#include <Components/EditorDemoRoomDrawTextComponent.h>
#include <Components/DemoRoomSwitchLevelComponent.h>
#include <Components/EditorDemoRoomComponent.h>

void InitDemoRoomResources()
{
    // We must register our Qt resources (.qrc file) since this is being loaded from a separate module (gem)
    Q_INIT_RESOURCE(DemoRoom);
}

namespace DemoRoom
{
    class DemoRoomEditorModule
        : public DemoRoomModuleInterface
    {
    public:
        AZ_RTTI(DemoRoomEditorModule, "{CBB53843-0411-4A8A-BB05-9E5C99E3831E}", DemoRoomModuleInterface);
        AZ_CLASS_ALLOCATOR(DemoRoomEditorModule, AZ::SystemAllocator, 0);

        DemoRoomEditorModule()
        {
            InitDemoRoomResources();

            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                DemoRoomEditorSystemComponent::CreateDescriptor(),
                EditorDemoRoomComponent::CreateDescriptor(),
                EditorDemoRoomStandComponent::CreateDescriptor(),
                EditorDemoRoomDrawTextComponent::CreateDescriptor(),
                DemoRoomSwitchLevelComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<DemoRoomEditorSystemComponent>(),
            };
        }
    };
}// namespace DemoRoom

AZ_DECLARE_MODULE_CLASS(Gem_DemoRoom, DemoRoom::DemoRoomEditorModule)
