//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include "DemoRoomTriggerButtonReceiverComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace DemoRoom
{
    //////////////////////////////////////////////////////////////////////////
    class DemoRoomTriggerButtonReceiverComponentNotificationBehaviorHandler
        : public DemoRoomTriggerButtonReceiverNotificationBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(DemoRoomTriggerButtonReceiverComponentNotificationBehaviorHandler, "{61073993-8177-4E27-9C77-BA6C8CE5C73E}", AZ::SystemAllocator,
            OnTriggerButtonEnter, OnTriggerButtonExit);

        void OnTriggerButtonEnter() override
        {
            Call(FN_OnTriggerButtonEnter);
        }

        void OnTriggerButtonExit() override
        {
            Call(FN_OnTriggerButtonExit);
        }
    };

    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonReceiverComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonReceiverComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DemoRoomTriggerButtonReceiverService"));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonReceiverComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DemoRoomTriggerButtonService"));
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonReceiverComponent::Reflect(AZ::ReflectContext* reflection)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
        if (serializeContext)
        {
            serializeContext->Class<DemoRoomTriggerButtonReceiverComponent, AZ::Component>()
                ->Version(0)
                ;
    
            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<DemoRoomTriggerButtonReceiverComponent>("DemoRoom Trigger Button Receiver", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute("Category", "Gameplay")
                    //->Attribute("Icon", "Icons/Components/DemoRoomTriggerButton.svg")
                    //->Attribute("ViewportIcon", "Icons/Components/Viewport/DemoRoomTriggerButton.svg")
                    ->Attribute("AutoExpand", true)
                    ->Attribute("AppearsInAddComponentMenu", AZ_CRC_CE("Game"))
                    ;
            }
        }

        auto* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
        if (behaviorContext)
        {
            behaviorContext->EBus<DemoRoomTriggerButtonReceiverNotificationBus>("DemoRoomTriggerButtonReceiverNotificationBus")
                ->Handler<DemoRoomTriggerButtonReceiverComponentNotificationBehaviorHandler>()
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::List)
                ;
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    DemoRoomTriggerButtonReceiverComponent::~DemoRoomTriggerButtonReceiverComponent()
    {
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonReceiverComponent::Init()
    {
       
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonReceiverComponent::Activate()
    {
        DemoRoomTriggerButtonReceiverNotificationBus::Handler::BusConnect(GetEntityId());
    }
    
    //////////////////////////////////////////////////////////////////////////////
    void DemoRoomTriggerButtonReceiverComponent::Deactivate()
    {
        DemoRoomTriggerButtonReceiverNotificationBus::Handler::BusDisconnect();
    }
}
