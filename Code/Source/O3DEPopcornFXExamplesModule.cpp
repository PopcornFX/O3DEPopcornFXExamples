//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "O3DEPopcornFXExamplesSystemComponent.h"
#include "DrawPopcornFXVersionTextComponent.h"

namespace O3DEPopcornFXExamples
{
    class O3DEPopcornFXExamplesModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(O3DEPopcornFXExamplesModule, "{17025b09-f45e-42f2-890f-2663d0663fb8}", AZ::Module);
        AZ_CLASS_ALLOCATOR(O3DEPopcornFXExamplesModule, AZ::SystemAllocator, 0);

        O3DEPopcornFXExamplesModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                O3DEPopcornFXExamplesSystemComponent::CreateDescriptor(),
                DrawPopcornFXVersionTextComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<O3DEPopcornFXExamplesSystemComponent>(),
            };
        }
    };
}// namespace O3DEPopcornFXExamples

AZ_DECLARE_MODULE_CLASS(Gem_O3DEPopcornFXExamples, O3DEPopcornFXExamples::O3DEPopcornFXExamplesModule)
