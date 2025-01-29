//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/Component.h>

#include <O3DEPopcornFXExamples/O3DEPopcornFXExamplesBus.h>

namespace O3DEPopcornFXExamples
{
    class O3DEPopcornFXExamplesSystemComponent
        : public AZ::Component
        , protected O3DEPopcornFXExamplesRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(O3DEPopcornFXExamplesSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        O3DEPopcornFXExamplesSystemComponent();
        ~O3DEPopcornFXExamplesSystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // O3DEPopcornFXExamplesRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
