//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <DemoRoom/DemoRoomBus.h>
#include <CrySystemBus.h>

namespace DemoRoom
{
    class DemoRoomSystemComponent
        : public AZ::Component
        , protected CrySystemEventBus::Handler
        , protected DemoRoomRequestBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(DemoRoomSystemComponent, "{F29372AD-EF70-4EC4-9909-183AFDD56E09}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        DemoRoomSystemComponent();
        ~DemoRoomSystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // DemoRoomRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////
        // CrySystemEventsBus::Handler interface implementation
        virtual void OnCrySystemInitialized(ISystem& system, const SSystemInitParams&) override;
        virtual void OnCrySystemShutdown(ISystem& system) override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZTickBus interface implementation
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        ////////////////////////////////////////////////////////////////////////
    };

} // namespace DemoRoom
