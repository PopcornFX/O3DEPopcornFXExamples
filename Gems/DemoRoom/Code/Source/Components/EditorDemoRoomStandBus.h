//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace DemoRoom
{
    class EditorDemoRoomStandRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(EditorDemoRoomStandRequests, "{8579F9BC-D512-4C28-A53E-30D4E6548A2A}");

        //! Overrides the default AZ::EBusTraits handler policy to allow one listener only.
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        virtual ~EditorDemoRoomStandRequests() {}

        virtual AZ::EntityId GetStandEntityId() const = 0;
        virtual void SetStandEntityId(AZ::EntityId entityId) = 0;
        virtual AZ::EntityId GetNamePlateEntityId() const = 0;
        virtual void SetNamePlateEntityId(AZ::EntityId entityId) = 0;

        virtual AZ::u32 GetRoom() const = 0;
        virtual void SetRoom(AZ::u32 room) = 0;
        virtual AZ::u32 GetSection() const = 0;
        virtual void SetSection(AZ::u32 section) = 0;
    };

    /// The EBus for requests to for setting and getting DemoRoom Stand component properties.
    typedef AZ::EBus<EditorDemoRoomStandRequests> EditorDemoRoomStandRequestBus;
}
