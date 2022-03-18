//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <DemoRoomSystemComponent.h>

#include <AzToolsFramework/Entity/EditorEntityContextBus.h>

namespace DemoRoom
{
    /// System component for DemoRoom editor
    class DemoRoomEditorSystemComponent
        : public DemoRoomSystemComponent
        , private AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = DemoRoomSystemComponent;
    public:
        AZ_COMPONENT(DemoRoomEditorSystemComponent, "{93B49918-30C2-4BA4-AACF-CAAC5C02D900}", BaseSystemComponent);
        static void Reflect(AZ::ReflectContext* context);

        DemoRoomEditorSystemComponent();
        ~DemoRoomEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        // AzToolsFramework::EditorEventsBus overrides ...
        void NotifyRegisterViews() override;
    };
} // namespace DemoRoom
