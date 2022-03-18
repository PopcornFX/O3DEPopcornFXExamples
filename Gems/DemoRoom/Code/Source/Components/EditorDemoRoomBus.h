//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace DemoRoom
{
    class EditorDemoRoomRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(EditorDemoRoomRequests, "{E9FC84EC-C63A-4241-B284-B8B72487F269}");

        //! Overrides the default AZ::EBusTraits handler policy to allow one listener only.
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        virtual ~EditorDemoRoomRequests() {}

        virtual AZ::EntityId GetStandsEntityId() const = 0;
        virtual void SetStandsEntityId(const AZ::EntityId& standsEntityId) = 0;

        virtual AZ::EntityId GetEnvironmentEntityId() const = 0;
        virtual void SetEnvironmentEntityId(const AZ::EntityId& environmentEntityId) = 0;

        virtual AZStd::vector<AZ::u32> GetRoomSizes() const = 0;
        virtual void SetRoomSizes(const AZStd::vector<AZ::u32>& rooms) = 0;

        virtual bool GetLights() const = 0;
        virtual void SetLights(bool lights) = 0;

        virtual bool GetDoubleHeight() const = 0;
        virtual void SetDoubleHeight(bool doubleHeight) = 0;
    };

    /// The EBus for requests to for setting and getting DemoRoom component properties.
    typedef AZ::EBus<EditorDemoRoomRequests> EditorDemoRoomRequestBus;
}
