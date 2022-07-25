//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

#include "O3DEPopcornFXExamplesSystemComponent.h"

namespace O3DEPopcornFXExamples
{
    void O3DEPopcornFXExamplesSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<O3DEPopcornFXExamplesSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<O3DEPopcornFXExamplesSystemComponent>("O3DEPopcornFXExamples", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void O3DEPopcornFXExamplesSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("O3DEPopcornFXExamplesService"));
    }

    void O3DEPopcornFXExamplesSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("O3DEPopcornFXExamplesService"));
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
