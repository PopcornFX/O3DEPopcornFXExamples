//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once
#include <AzCore/Component/Component.h>

#include <DemoRoom/DemoRoomTriggerButtonReceiverComponentBus.h>

namespace DemoRoom
{
    class DemoRoomTriggerButtonReceiverComponent
        : public AZ::Component
        , private DemoRoomTriggerButtonReceiverNotificationBus::Handler
    {
    public:
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void Reflect(AZ::ReflectContext* reflection);

        AZ_COMPONENT(DemoRoomTriggerButtonReceiverComponent, "{2A4C42F4-3675-4684-A8AA-F39A148B8B10}");
        virtual ~DemoRoomTriggerButtonReceiverComponent();

        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;
    };
}
