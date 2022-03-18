//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Component/EntityId.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/EBus/EBus.h>

//----------------------------------------------------------------------------

struct  SRenderText3D
{
    AZ::EntityId    m_EntityId;
    AZ::Transform   m_Transform;
    AZStd::string   m_Text;
    AZ::Vector3     m_Color;
};

//----------------------------------------------------------------------------

class DemoRoomDrawTextEvents
    : public AZ::EBusTraits
{
public:
    static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
    static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
    // Public functions
    virtual ~DemoRoomDrawTextEvents() {}

    virtual void        UpdateText(const SRenderText3D& renderText) { (void)renderText; }
    virtual void        RemoveText(AZ::EntityId entityId) { (void)entityId; }
};
using DemoRoomDrawTextEventsBus = AZ::EBus<DemoRoomDrawTextEvents>;
