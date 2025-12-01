//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <Atom/RHI/PipelineState.h>
#include <Atom/RPI.Public/Buffer/BufferPool.h>
#include <Atom/RPI.Public/FeatureProcessor.h>
#include <Atom/RPI.Public/Shader/ShaderResourceGroup.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Math/Matrix4x4.h>

#include "DemoRoom/DemoRoomDrawTextRequests.h"

#include "FontAssetLoader.h"
#include <assert.h>

//----------------------------------------------------------------------------

struct SVF_P2F_C4B_T2F_F4B;

//----------------------------------------------------------------------------

class CDemoRoomTextFeatureProcessorInterface : public AZ::RPI::FeatureProcessor
{
public:
    AZ_RTTI(CDemoRoomTextFeatureProcessorInterface, "{D86216E4-92A8-43BE-A5E4-8853489C7BA2}", AZ::RPI::FeatureProcessor);
};

//----------------------------------------------------------------------------

class CDemoRoomTextFeatureProcessor
    : public CDemoRoomTextFeatureProcessorInterface
    , protected DemoRoomDrawTextEventsBus::Handler
{
public:
    AZ_RTTI(CDemoRoomTextFeatureProcessor, "{D86216E4-92A8-43BE-123F-883489C75BA2}", CDemoRoomTextFeatureProcessorInterface);
    AZ_CLASS_ALLOCATOR(CDemoRoomTextFeatureProcessor, AZ::SystemAllocator, 0);

    CDemoRoomTextFeatureProcessor();
    virtual ~CDemoRoomTextFeatureProcessor() = default;

    static void	Reflect(AZ::ReflectContext* context);

    // FeatureProcessor overrides ...
    void		Activate() override;
    void		Deactivate() override;
    void		Simulate(const SimulatePacket& packet) override;
    void		Render(const RenderPacket& packet) override;

    virtual void        UpdateText(const SRenderText3D& renderText);
    virtual void        RemoveText(AZ::EntityId entityId);

private:
    bool        _AllocateBuffer(AZ::RHI::Ptr<AZ::RHI::Buffer> &outBuffer,
                                size_t &outCapacity,
                                size_t elemCount,
                                size_t elemSize);
    void        *_MapBuffer(AZ::RHI::Ptr<AZ::RHI::Buffer> buffer, size_t sizeToMap);
    AZ::Data::Instance<AZ::RPI::ShaderResourceGroup>    _CreateShaderResourceGroup(AZ::Data::Instance<AZ::RPI::Shader> shader,
                                                                                   const char* srgName);

    template<class T>
    T		_Align(T nData, size_t nAlign)
    {
        assert((nAlign & (nAlign - 1)) == 0);
        size_t size = ((size_t)nData + (nAlign - 1)) & ~(nAlign - 1);
        return T(size);
    }

    AZStd::vector<SRenderText3D>    m_TextsToDraw;
    bool                            m_TextWasModified = false;
    CFontAssetLoader                m_FontAssetLoader;

    AZ::RHI::Ptr<AZ::RHI::BufferPool>   m_BufferPool = NULL;

    AZ::RHI::Ptr<AZ::RHI::Buffer>       m_IndicesData = NULL;
    AZ::RHI::Ptr<AZ::RHI::Buffer>       m_PositionsData = NULL;
    AZ::RHI::Ptr<AZ::RHI::Buffer>       m_UVsData = NULL;
    AZ::RHI::Ptr<AZ::RHI::Buffer>       m_ColorsData = NULL;

    size_t                              m_IndicesCapacity = 0;
    size_t                              m_VtxCapacity = 0;

    AZ::RHI::IndexBufferView            m_Indices;
    AZ::RHI::StreamBufferView           m_Positions;
    AZ::RHI::StreamBufferView           m_UVs;
    AZ::RHI::StreamBufferView           m_Colors;

    AZ::RHI::ConstPtr<AZ::RHI::PipelineState>           m_MaterialPipelineState = NULL;
    AZ::Data::Instance<AZ::RPI::ShaderResourceGroup>    m_MaterialSrg = NULL;
    AZ::RHI::ConstPtr<AZ::RHI::PipelineState>           m_PipelineState = NULL;

    AZ::Data::Instance<AZ::RPI::Image>                  m_FontImage = NULL;

    SVF_P2F_C4B_T2F_F4B                                 *m_QuadsVtx = NULL;
    AZ::u16                                             *m_QuadsIdx = NULL;

    AZ::RHI::Ptr<AZ::RHI::DrawPacket> m_DrawPacket;
    AZ::RHI::GeometryView m_GeometryView{ AZ::RHI::MultiDevice::AllDevices };
    uint32_t m_IndexCount = 0;
};

//----------------------------------------------------------------------------
