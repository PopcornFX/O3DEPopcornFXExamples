//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Component/ComponentBus.h>

namespace DemoRoom
{
    class DemoRoomTriggerButtonReceiverNotifications
        : public AZ::ComponentBus
    {
    public:
        virtual void OnTriggerButtonEnter() {};
        virtual void OnTriggerButtonExit() {};
    };

    using DemoRoomTriggerButtonReceiverNotificationBus = AZ::EBus<DemoRoomTriggerButtonReceiverNotifications>;
}
