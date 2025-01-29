
#pragma once

#include <O3DEPopcornFXExamples/O3DEPopcornFXExamplesTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace O3DEPopcornFXExamples
{
    class O3DEPopcornFXExamplesRequests
    {
    public:
        AZ_RTTI(O3DEPopcornFXExamplesRequests, O3DEPopcornFXExamplesRequestsTypeId);
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
