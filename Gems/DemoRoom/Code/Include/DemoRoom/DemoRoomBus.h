//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace DemoRoom
{
    class DemoRoomRequests
    {
    public:
        AZ_RTTI(DemoRoomRequests, "{04EB601E-C098-47F3-BD62-3335CD8721F3}");
        virtual ~DemoRoomRequests() = default;
        // Put your public methods here
    };
    
    class DemoRoomBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using DemoRoomRequestBus = AZ::EBus<DemoRoomRequests, DemoRoomBusTraits>;
    using DemoRoomInterface = AZ::Interface<DemoRoomRequests>;

} // namespace DemoRoom
