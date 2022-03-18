//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace DemoRoom
{
    class EditorDemoRoomDrawTextRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(EditorDemoRoomDrawTextRequests, "{99A565D6-B9EB-430A-B26F-8E3AC36F7EAC}");

        //! Overrides the default AZ::EBusTraits handler policy to allow one listener only.
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        virtual ~EditorDemoRoomDrawTextRequests() {}

        virtual const AZStd::string& GetText() const = 0;
        virtual void SetText(const AZStd::string& text) = 0;
        virtual void SetColor(const AZ::Color& color) = 0;
    };

    /// The EBus for requests to for setting and getting DemoRoom component properties.
    typedef AZ::EBus<EditorDemoRoomDrawTextRequests> EditorDemoRoomDrawTextRequestBus;
}
