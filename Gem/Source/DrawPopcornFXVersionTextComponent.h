//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Color.h>

#include <O3DEPopcornFXExamples/O3DEPopcornFXExamplesBus.h>

class DrawPopcornFXVersionTextComponent
    : public AZ::Component
    , public AZ::TransformNotificationBus::Handler
{
public:
    static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
    static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
    static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
    static void Reflect(AZ::ReflectContext* reflection);

    AZ_COMPONENT(DrawPopcornFXVersionTextComponent, "{B55F7122-70D8-4FF0-B105-CE2F2BB7DBAA}");
    virtual ~DrawPopcornFXVersionTextComponent();

    // AZ::Component
    void Init() override;
    void Activate() override;
    void Deactivate() override;

    //////////////////////////////////////////////////////////////////////////
    // TransformNotificationBus
    //! Called when the local transform of the entity has changed. Local transform update always implies world transform change too.
    virtual void	OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;
    //////////////////////////////////////////////////////////////////////////
    private:
        AZStd::string   GetEngineVersion() const;

        AZ::Color m_color = AZ::Color(1.0f);
        bool m_displayO3DEVersion = false;
        AZStd::string m_versionToDisplay;
};
