//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace O3DEPopcornFXExamples
{
    class O3DEPopcornFXExamplesRequests
    {
    public:
        AZ_RTTI(O3DEPopcornFXExamplesRequests, "{d48856e6-7c7b-493d-bde4-9b624bdf444b}");
        virtual ~O3DEPopcornFXExamplesRequests() = default;
        // Put your public methods here
    };
    
    class O3DEPopcornFXExamplesBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using O3DEPopcornFXExamplesRequestBus = AZ::EBus<O3DEPopcornFXExamplesRequests, O3DEPopcornFXExamplesBusTraits>;
    using O3DEPopcornFXExamplesInterface = AZ::Interface<O3DEPopcornFXExamplesRequests>;

} // namespace O3DEPopcornFXExamples
