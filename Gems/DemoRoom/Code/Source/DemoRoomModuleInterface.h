//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>
#include <DemoRoomSystemComponent.h>
#include <Components/DemoRoomCameraInputComponent.h>
#include <Components/DemoRoomDrawTextComponent.h>
#include <Components/DemoRoomSwitchLevelComponent.h>
#include <Components/DemoRoomTriggerButtonComponent.h>
#include <Components/DemoRoomTriggerButtonReceiverComponent.h>

namespace DemoRoom
{
    class DemoRoomModuleInterface
        : public AZ::Module
    {
    public:
        AZ_RTTI(DemoRoomModuleInterface, "{9063BBF2-2030-450D-8E65-1F3F56376659}", AZ::Module);
        AZ_CLASS_ALLOCATOR(DemoRoomModuleInterface, AZ::SystemAllocator, 0);

        DemoRoomModuleInterface()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                DemoRoomSystemComponent::CreateDescriptor(),
                DemoRoomCameraInputComponent::CreateDescriptor(),
                DemoRoomDrawTextComponent::CreateDescriptor(),
                DemoRoomSwitchLevelComponent::CreateDescriptor(),
                DemoRoomTriggerButtonComponent::CreateDescriptor(),
                DemoRoomTriggerButtonReceiverComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<DemoRoomSystemComponent>(),
            };
        }
    };
}// namespace DemoRoom
