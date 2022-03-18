//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Color.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBodyEvents.h>

namespace DemoRoom
{
    class DemoRoomTriggerButtonComponent
        : public AZ::Component
    {
    public:
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void Reflect(AZ::ReflectContext* reflection);

        AZ_COMPONENT(DemoRoomTriggerButtonComponent, "{BE5570D9-4AE9-4E0E-960A-DC9D8AAFC333}");
        virtual ~DemoRoomTriggerButtonComponent();

        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

private:
        void OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent) const;
        void OnTriggerExit(const AzPhysics::TriggerEvent& triggerEvent) const;
        void SetButtonColor(const AZ::Color& color) const;

        AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler m_onTriggerEnterHandler;
        AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler m_onTriggerExitHandler;

        AZStd::vector<AZ::EntityId> m_receivers;
        AZStd::string m_slotLabel = "SM_Button001_Red";
        AZ::Color m_activatedColor = AZ::Color(0.0f, 1.0f, 0.0f, 1.0f);
        AZ::Color m_deactivatedColor = AZ::Color(1.0f, 0.0f, 0.0f, 1.0f);
    };
}
