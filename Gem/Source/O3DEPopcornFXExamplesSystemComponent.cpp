//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Serialization/SerializeContext.h>

#include "O3DEPopcornFXExamplesSystemComponent.h"

#include <O3DEPopcornFXExamples/O3DEPopcornFXExamplesTypeIds.h>

namespace O3DEPopcornFXExamples
{
    AZ_COMPONENT_IMPL(O3DEPopcornFXExamplesSystemComponent, "O3DEPopcornFXExamplesSystemComponent",
        O3DEPopcornFXExamplesSystemComponentTypeId);

    void O3DEPopcornFXExamplesSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<O3DEPopcornFXExamplesSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void O3DEPopcornFXExamplesSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("O3DEPopcornFXExamplesService"));
    }

    void O3DEPopcornFXExamplesSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("O3DEPopcornFXExamplesService"));
    }

    void O3DEPopcornFXExamplesSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void O3DEPopcornFXExamplesSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    O3DEPopcornFXExamplesSystemComponent::O3DEPopcornFXExamplesSystemComponent()
    {
        if (O3DEPopcornFXExamplesInterface::Get() == nullptr)
        {
            O3DEPopcornFXExamplesInterface::Register(this);
        }
    }

    O3DEPopcornFXExamplesSystemComponent::~O3DEPopcornFXExamplesSystemComponent()
    {
        if (O3DEPopcornFXExamplesInterface::Get() == this)
        {
            O3DEPopcornFXExamplesInterface::Unregister(this);
        }
    }

    void O3DEPopcornFXExamplesSystemComponent::Init()
    {
    }

    void O3DEPopcornFXExamplesSystemComponent::Activate()
    {
        O3DEPopcornFXExamplesRequestBus::Handler::BusConnect();
    }

    void O3DEPopcornFXExamplesSystemComponent::Deactivate()
    {
        O3DEPopcornFXExamplesRequestBus::Handler::BusDisconnect();
    }
}
