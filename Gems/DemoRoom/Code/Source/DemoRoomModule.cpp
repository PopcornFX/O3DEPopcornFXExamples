//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include <DemoRoomModuleInterface.h>
#include <DemoRoomSystemComponent.h>

namespace DemoRoom
{
    class DemoRoomModule
        : public DemoRoomModuleInterface
    {
    public:
        AZ_RTTI(DemoRoomModule, "{CBB53843-0411-4A8A-BB05-9E5C99E3831E}", DemoRoomModuleInterface);
        AZ_CLASS_ALLOCATOR(DemoRoomModule, AZ::SystemAllocator, 0);
    };
}// namespace DemoRoom

AZ_DECLARE_MODULE_CLASS(Gem_DemoRoom, DemoRoom::DemoRoomModule)
