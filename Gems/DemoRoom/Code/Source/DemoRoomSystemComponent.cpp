//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <DemoRoomSystemComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <Atom/RPI.Public/FeatureProcessorFactory.h>
#include <CryCommon/ISystem.h>

#include "TextRendering/Render3DTextRenderFeature.h"

namespace DemoRoom
{
    void DemoRoomSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        CDemoRoomTextFeatureProcessor::Reflect(context);

        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<DemoRoomSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<DemoRoomSystemComponent>("DemoRoom", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void DemoRoomSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("DemoRoomService"));
    }

    void DemoRoomSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("DemoRoomService"));
    }

    void DemoRoomSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void DemoRoomSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    DemoRoomSystemComponent::DemoRoomSystemComponent()
    {
        if (DemoRoomInterface::Get() == nullptr)
        {
            DemoRoomInterface::Register(this);
        }
    }

    DemoRoomSystemComponent::~DemoRoomSystemComponent()
    {
        if (DemoRoomInterface::Get() == this)
        {
            DemoRoomInterface::Unregister(this);
        }
    }

    void DemoRoomSystemComponent::OnCrySystemInitialized(ISystem &system, const SSystemInitParams&)
    {
#if !defined(AZ_MONOLITHIC_BUILD)
        // When module is linked dynamically, we must set our gEnv pointer.
        // When module is linked statically, we'll share the application's gEnv pointer.
        gEnv = system.GetGlobalEnvironment();
#endif
    }

    void DemoRoomSystemComponent::OnCrySystemShutdown(ISystem&)
    {
    }

    void DemoRoomSystemComponent::Init()
    {
    }

    void DemoRoomSystemComponent::Activate()
    {
        DemoRoomRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
        CrySystemEventBus::Handler::BusConnect();

        AZ::RPI::FeatureProcessorFactory* factory = AZ::RPI::FeatureProcessorFactory::Get();
        if (factory != NULL)
            factory->RegisterFeatureProcessorWithInterface<CDemoRoomTextFeatureProcessor, CDemoRoomTextFeatureProcessorInterface>();
    }

    void DemoRoomSystemComponent::Deactivate()
    {
        AZ::RPI::FeatureProcessorFactory* factory = AZ::RPI::FeatureProcessorFactory::Get();
        if (factory != NULL)
            factory->UnregisterFeatureProcessor<CDemoRoomTextFeatureProcessor>();

        CrySystemEventBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        DemoRoomRequestBus::Handler::BusDisconnect();
    }

    void DemoRoomSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace DemoRoom
