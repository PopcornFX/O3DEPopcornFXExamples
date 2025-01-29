//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <AzCore/Serialization/SerializeContext.h>

#include <AzToolsFramework/API/ViewPaneOptions.h>

#include <DemoRoomWidget.h>
#include <DemoRoomEditorSystemComponent.h>

namespace DemoRoom
{
    void DemoRoomEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<DemoRoomEditorSystemComponent, DemoRoomSystemComponent>()
                ->Version(0);
        }
    }

    DemoRoomEditorSystemComponent::DemoRoomEditorSystemComponent() = default;

    DemoRoomEditorSystemComponent::~DemoRoomEditorSystemComponent() = default;

    void DemoRoomEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("DemoRoomEditorService"));
    }

    void DemoRoomEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("DemoRoomEditorService"));
    }

    void DemoRoomEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void DemoRoomEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
        DemoRoomSystemComponent::GetDependentServices(dependent);
    }

    void DemoRoomEditorSystemComponent::Activate()
    {
        DemoRoomSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void DemoRoomEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        DemoRoomSystemComponent::Deactivate();
    }

    void DemoRoomEditorSystemComponent::NotifyRegisterViews()
    {
        AzToolsFramework::ViewPaneOptions options;
        options.paneRect = QRect(100, 100, 500, 500);
        options.showOnToolsToolbar = true;
        options.toolbarIcon = ":/DemoRoom/toolbar_icon.svg";

        // Register our custom widget as a dockable tool with the Editor
        AzToolsFramework::RegisterViewPane<DemoRoomWidget>("Demo Room", "Tools", options);
    }

} // namespace DemoRoom
