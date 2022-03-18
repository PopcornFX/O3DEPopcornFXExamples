//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>

namespace DemoRoom
{
    class DemoRoomDrawTextComponent
        : public AZ::Component
        , public AZ::TransformNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(DemoRoomDrawTextComponent, "{49931260-D3D4-4EF5-BC9B-EC3967B17243}");
        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        DemoRoomDrawTextComponent() = default;
        explicit DemoRoomDrawTextComponent(const AZStd::string& text, const AZ::Color &color);
        ~DemoRoomDrawTextComponent() override = default;

        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        //////////////////////////////////////////////////////////////////////////
        // TransformNotificationBus
        //! Called when the local transform of the entity has changed. Local transform update always implies world transform change too.
        virtual void	OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;
        //////////////////////////////////////////////////////////////////////////

    private:
        AZStd::string m_text;
        AZ::Color m_color;
    };
}
