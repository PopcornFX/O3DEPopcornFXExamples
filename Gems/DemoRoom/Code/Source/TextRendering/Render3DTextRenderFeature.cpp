//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include "Render3DTextRenderFeature.h"
#include <Atom/RHI/DrawPacketBuilder.h>
#include <Atom/RHI/RHISystemInterface.h>
#include <Atom/RHI/Factory.h>
#include <Atom/RHI.Reflect/InputStreamLayoutBuilder.h>
#include <Atom/RPI.Public/Image/StreamingImagePool.h>
#include <Atom/RPI.Public/RPISystemInterface.h>
#include <Atom/RPI.Public/Image/ImageSystemInterface.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/View.h>
#include <AzFramework/Scene/Scene.h>
#include <CryCommon/Cry_Color.h>
#include <CryCommon/VertexFormats.h>
#include <CryCommon/ISystem.h>
#include <CryCommon/IFont.h>

#include <AtomLyIntegration/AtomFont/FFont.h>

//----------------------------------------------------------------------------

void	CDemoRoomTextFeatureProcessor::Reflect(AZ::ReflectContext* context)
{
    if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
    {
        serializeContext
            ->Class<CDemoRoomTextFeatureProcessor, AZ::RPI::FeatureProcessor>()
            ->Version(0);
    }
}

//----------------------------------------------------------------------------

CDemoRoomTextFeatureProcessor::CDemoRoomTextFeatureProcessor()
:   m_TextWasModified(false)
{
}

//----------------------------------------------------------------------------

void	CDemoRoomTextFeatureProcessor::Activate()
{
    // Create a buffer pool:
    AZ::RHI::BufferPoolDescriptor   dynamicPoolDescriptor;
    dynamicPoolDescriptor.m_heapMemoryLevel = AZ::RHI::HeapMemoryLevel::Device;
    dynamicPoolDescriptor.m_hostMemoryAccess = AZ::RHI::HostMemoryAccess::Write;
    dynamicPoolDescriptor.m_bindFlags = AZ::RHI::BufferBindFlags::InputAssembly | AZ::RHI::BufferBindFlags::ShaderRead;
    dynamicPoolDescriptor.m_largestPooledAllocationSizeInBytes = 0x100000;
    dynamicPoolDescriptor.m_deviceMask = AZ::RHI::MultiDevice::DefaultDevice;

    m_BufferPool = aznew AZ::RHI::BufferPool;
    m_BufferPool->SetName(AZ::Name("DemoRoom DrawText Pool"));

    AZ::RHI::ResultCode resultCode = m_BufferPool->Init(dynamicPoolDescriptor);
    if (resultCode != AZ::RHI::ResultCode::Success)
    {
        AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to create GPU buffer pool");
        return;
    }

    // Load the necessary assets:
    // m_FontAssetLoader.AddTextureToLoad("Images/Atlas.png.streamingimage")
    if (!m_FontAssetLoader.AddShaderToLoad("Shaders/Basic3DFont.azshader"))
    {
        AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to create assets");
        return;
    }

    DemoRoomDrawTextEventsBus::Handler::BusConnect();
}

//----------------------------------------------------------------------------

void	CDemoRoomTextFeatureProcessor::Deactivate()
{
    if (m_QuadsVtx != NULL)
        free(m_QuadsVtx);
    if (m_QuadsIdx != NULL)
        free(m_QuadsIdx);
    DemoRoomDrawTextEventsBus::Handler::BusDisconnect();
}

//----------------------------------------------------------------------------

void	CDemoRoomTextFeatureProcessor::Simulate(const SimulatePacket& packet)
{
    AZ_UNUSED(packet);

    IFFont  *pFont = NULL;
    const AZStd::string     pkfxFontName = "default-pkfx";
    if (gEnv != NULL && gEnv->pCryFont != NULL)
    {
        ICryFont& cryFont = *gEnv->pCryFont;
        pFont = cryFont.GetFont(pkfxFontName.c_str());
        if (pFont == NULL)
        {
            IFFont* font = cryFont.NewFont(pkfxFontName.c_str());
            AZ_Assert(font, "Could not instantiate font: %s", pkfxFontName.c_str());

            const AZStd::string fontPath = "Fonts/" + pkfxFontName + ".font";
            if (!font->Load(fontPath.c_str()))
            {
                AZ_Error("CDemoRoomTextFeatureProcessor", false, "Could not load font: %s", fontPath.c_str());
            }
        }
    }
    if (m_TextWasModified && pFont != NULL)
    {
        AZ::u32                 characCount = 0;
        ColorF                  txtColor(1.0f, 1.0f, 1.0f, 1.0f);
        STextDrawContext        txtCtx;

        txtCtx.SetSizeIn800x600(false);
        txtCtx.SetSize(Vec2(12.0f));
        txtCtx.SetCharWidthScale(1.0f);
        txtCtx.SetProportional(true);
        txtCtx.SetFlags(0);
        txtCtx.SetColor(txtColor);

        for (size_t i = 0; i < m_TextsToDraw.size(); ++i)
        {
            const char* str = m_TextsToDraw[i].m_Text.data();
            characCount += pFont->GetNumQuadsForText(str, true, txtCtx);
        }

        // Allocate the buffers to the right size:
        if (m_IndicesCapacity < characCount * 6)
        {
            if (!_AllocateBuffer(m_IndicesData, m_IndicesCapacity, characCount * 6, sizeof(AZ::u32)))
            {
                AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to allocate buffer");
                return;
            }
            if (m_QuadsIdx != NULL)
                free(m_QuadsIdx);
            m_QuadsIdx = (AZ::u16*)malloc(m_IndicesCapacity * sizeof(AZ::u16));
        }
        if (m_VtxCapacity < characCount * 4)
        {
            if (!_AllocateBuffer(m_PositionsData, m_VtxCapacity, characCount * 4, sizeof(AZ::Vector3)))
            {
                AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to allocate buffer");
                return;
            }
            if (!_AllocateBuffer(m_UVsData, m_VtxCapacity, characCount * 4, sizeof(AZ::Vector2)))
            {
                AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to allocate buffer");
                return;
            }
            if (!_AllocateBuffer(m_ColorsData, m_VtxCapacity, characCount * 4, sizeof(AZ::Vector3)))
            {
                AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to allocate buffer");
                return;
            }
            if (m_QuadsVtx != NULL)
                free(m_QuadsVtx);
            m_QuadsVtx = (SVF_P2F_C4B_T2F_F4B*)malloc(m_VtxCapacity * sizeof(SVF_P2F_C4B_T2F_F4B));
        }

        if (characCount == 0)
        {
            m_IndexCount = 0;
            m_TextWasModified = false;
            return;
        }

        // Update buffer views and count:
        uint32_t    idxByteCount = static_cast<uint32_t>(characCount * 6 * sizeof(AZ::u16));
        uint32_t    posByteCount = static_cast<uint32_t>(characCount * 4 * sizeof(AZ::Vector3));
        uint32_t    uvByteCount = static_cast<uint32_t>(characCount * 4 * sizeof(AZ::Vector2));
        m_Indices = AZ::RHI::IndexBufferView(*m_IndicesData, 0U, idxByteCount, AZ::RHI::IndexFormat::Uint16);
        m_Positions = AZ::RHI::StreamBufferView(*m_PositionsData, 0U, posByteCount, sizeof(AZ::Vector3));
        m_UVs = AZ::RHI::StreamBufferView(*m_UVsData, 0U, uvByteCount, sizeof(AZ::Vector2));
        m_Colors = AZ::RHI::StreamBufferView(*m_ColorsData, 0U, uvByteCount, sizeof(AZ::Vector3));

        SVF_P2F_C4B_T2F_F4B *srcQuads = m_QuadsVtx;
        AZ::u16             *srcIdx = m_QuadsIdx;
        // Map the buffers:
        AZ::u16             *indices = static_cast<AZ::u16*>(_MapBuffer(m_IndicesData, m_Indices.GetByteCount()));
        AZ::Vector3         *positions = static_cast<AZ::Vector3*>(_MapBuffer(m_PositionsData, m_Positions.GetByteCount()));
        AZ::Vector2         *uvs = static_cast<AZ::Vector2*>(_MapBuffer(m_UVsData, m_UVs.GetByteCount()));
        AZ::Vector3         *colors = static_cast<AZ::Vector3*>(_MapBuffer(m_ColorsData, m_Colors.GetByteCount()));
        // Generate the letters geometry:
        uint32_t            quadCount = 0;
        const float         pixelToWorld = 1.0f / 30.0f;

        for (size_t i = 0; i < m_TextsToDraw.size(); ++i)
        {
            AZ::Transform   strTransform = m_TextsToDraw[i].m_Transform;
            AZStd::string   strText = m_TextsToDraw[i].m_Text;
            AZ::Vector3     color = m_TextsToDraw[i].m_Color;
            uint32_t        strQuadCount = pFont->WriteTextQuadsToBuffers(srcQuads, srcIdx, ((uint32_t)m_VtxCapacity / 4) - quadCount, 0, 0, 0, strText.data(), true, txtCtx);

            for (uint32_t vtxId = 0; vtxId < strQuadCount * 4; ++vtxId)
            {
                AZ::Vector3    pos = AZ::Vector3(srcQuads[vtxId].xy.x, 0, -srcQuads[vtxId].xy.y);
                pos = strTransform.TransformPoint(pos * pixelToWorld);
                positions[vtxId] = pos;
                uvs[vtxId] = AZ::Vector2(srcQuads[vtxId].st.x, srcQuads[vtxId].st.y);
                colors[vtxId] = color;
            }
            for (uint32_t idxId = 0; idxId < strQuadCount * 6; ++idxId)
            {
                indices[idxId] = srcIdx[idxId] + (AZ::u16)(quadCount * 4);
            }

            srcQuads += strQuadCount * 4;
            srcIdx += strQuadCount * 6;

            indices += strQuadCount * 6;
            positions += strQuadCount * 4;
            uvs += strQuadCount * 4;
            colors += strQuadCount * 4;
            quadCount += strQuadCount;
        }
        // Unmap the buffers:
        m_BufferPool->UnmapBuffer(*m_IndicesData);
        m_BufferPool->UnmapBuffer(*m_PositionsData);
        m_BufferPool->UnmapBuffer(*m_UVsData);
        m_BufferPool->UnmapBuffer(*m_ColorsData);

        // Update views to actual count:
        uint32_t    actualIdxByteCount = static_cast<uint32_t>(quadCount * 6 * sizeof(AZ::u16));
        uint32_t    actualPosByteCount = static_cast<uint32_t>(quadCount * 4 * sizeof(AZ::Vector3));
        uint32_t    actualUvByteCount = static_cast<uint32_t>(quadCount * 4 * sizeof(AZ::Vector2));
        uint32_t    actualColorsByteCount = static_cast<uint32_t>(quadCount * 4 * sizeof(AZ::Vector3));
        m_Indices = AZ::RHI::IndexBufferView(*m_IndicesData, 0U, actualIdxByteCount, AZ::RHI::IndexFormat::Uint16);
        m_Positions = AZ::RHI::StreamBufferView(*m_PositionsData, 0U, actualPosByteCount, sizeof(AZ::Vector3));
        m_UVs = AZ::RHI::StreamBufferView(*m_UVsData, 0U, actualUvByteCount, sizeof(AZ::Vector2));
        m_Colors = AZ::RHI::StreamBufferView(*m_ColorsData, 0U, actualColorsByteCount, sizeof(AZ::Vector3));

        m_IndexCount = static_cast<uint32_t>(quadCount * 6);

        m_TextWasModified = false;
    }

    if (pFont != NULL)
    {
        AZ::FFont* font = static_cast<AZ::FFont*>(pFont); // LYSHINE_ATOM_TODO - find a different solution from downcasting - GHI #3570
        m_FontImage = font->GetFontImage();
    }

    AZ::Data::Instance<AZ::RPI::Shader>         shader = m_FontAssetLoader.GetShader();
    if (m_MaterialSrg == NULL && shader != NULL && m_FontImage != NULL)
    {
        m_MaterialSrg = _CreateShaderResourceGroup(m_FontAssetLoader.GetShader(), "MaterialSrg");

        if (m_MaterialSrg == NULL)
        {
            AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to find material SRG in shader");
            return;
        }
        AZ::RHI::ShaderInputImageIndex  fontMapIdx = m_MaterialSrg->FindShaderInputImageIndex(AZ::Name("m_fontMap"));
        if (!fontMapIdx.IsValid())
        {
            AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to find m_fontMap in MaterialSrg");
            return;
        }
        if (!m_MaterialSrg->SetImageView(fontMapIdx, m_FontImage->GetImageView()))
        {
            AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to set m_fontMap in MaterialSrg");
            return;
        }
        if (!m_MaterialSrg->IsQueuedForCompile())
            m_MaterialSrg->Compile();
        AZ::RHI::PipelineStateDescriptorForDraw	pipelineStateDesc = AZ::RHI::PipelineStateDescriptorForDraw();
        shader->GetRootVariant().ConfigurePipelineState(pipelineStateDesc);
        AZ::RHI::InputStreamLayoutBuilder layoutBuilder;
        layoutBuilder.AddBuffer()->Channel("POSITION", AZ::RHI::Format::R32G32B32_FLOAT);
        layoutBuilder.AddBuffer()->Channel("UV0", AZ::RHI::Format::R32G32_FLOAT);
        layoutBuilder.AddBuffer()->Channel("UV1", AZ::RHI::Format::R32G32B32_FLOAT);
        pipelineStateDesc.m_inputStreamLayout = layoutBuilder.End();
        const AZ::RPI::Scene    *scene = AZ::RPI::RPISystemInterface::Get()->GetSceneByName(AZ::Name(AzFramework::Scene::MainSceneName));
        if (scene == NULL)
        {
            AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to get current default scene");
            return;
        }
        scene->ConfigurePipelineState(shader->GetDrawListTag(), pipelineStateDesc);
        m_PipelineState = shader->AcquirePipelineState(pipelineStateDesc);
    }
}

//----------------------------------------------------------------------------

void	CDemoRoomTextFeatureProcessor::Render(const RenderPacket& packet)
{
    if (m_Indices.GetByteCount() == 0 || m_MaterialSrg == NULL)
        return;

    AZ::Data::Instance<AZ::RPI::Shader>             shader = m_FontAssetLoader.GetShader();
    AZ::Data::Instance<AZ::RPI::StreamingImage>     texture = m_FontAssetLoader.GetTexture();

    m_GeometryView.ClearStreamBufferViews();
    m_GeometryView.SetIndexBufferView(m_Indices);
    m_GeometryView.AddStreamBufferView(m_Positions);
    m_GeometryView.AddStreamBufferView(m_UVs);
    m_GeometryView.AddStreamBufferView(m_Colors);
    m_GeometryView.SetDrawArguments(AZ::RHI::DrawIndexed(0, m_IndexCount, 0));

    AZ::RHI::DrawPacketBuilder	dpBuilder(AZ::RHI::MultiDevice::DefaultDevice);
    dpBuilder.Begin(NULL);
    dpBuilder.SetGeometryView(&m_GeometryView);
    dpBuilder.SetDrawInstanceArguments(AZ::RHI::DrawInstanceArguments(1, 0));
    dpBuilder.AddShaderResourceGroup(m_MaterialSrg->GetRHIShaderResourceGroup());
    AZ::RHI::DrawPacketBuilder::DrawRequest	materialDr;
    materialDr.m_listTag = shader->GetDrawListTag();
    materialDr.m_pipelineState = m_PipelineState.get();
    materialDr.m_streamIndices = m_GeometryView.GetFullStreamBufferIndices();
    dpBuilder.AddDrawItem(materialDr);

    m_DrawPacket = dpBuilder.End();

    for (const AZ::RPI::ViewPtr& view : packet.m_views)
    {
        view->AddDrawPacket(m_DrawPacket.get(), 10000.0f);
    }
}

//----------------------------------------------------------------------------

void        CDemoRoomTextFeatureProcessor::UpdateText(const SRenderText3D& renderText)
{
    SRenderText3D   *found = NULL;
    for (SRenderText3D& cur : m_TextsToDraw)
    {
        if (cur.m_EntityId == renderText.m_EntityId)
        {
            found = &cur;
            break;
        }
    }
    if (found != NULL)
        *found = renderText;
    else
        m_TextsToDraw.push_back(renderText);
    m_TextWasModified = true;
}

//----------------------------------------------------------------------------

void        CDemoRoomTextFeatureProcessor::RemoveText(AZ::EntityId entityId)
{
    const SRenderText3D* found = NULL;
    for (const SRenderText3D& cur : m_TextsToDraw)
    {
        if (cur.m_EntityId == entityId)
        {
            found = &cur;
            break;
        }
    }
    if (found != NULL)
    {
        m_TextsToDraw.erase(found);
        m_TextWasModified = true;
    }
}

//----------------------------------------------------------------------------

bool        CDemoRoomTextFeatureProcessor::_AllocateBuffer( AZ::RHI::Ptr<AZ::RHI::Buffer> &outBuffer,
                                                            size_t &outCapacity,
                                                            size_t elemCount,
                                                            size_t elemSize)
{
    const size_t     alignOn = 0x1000;
    const size_t     alignedBufferSize = _Align(elemCount * elemSize, alignOn);
    AZ::RHI::Ptr<AZ::RHI::Buffer> tmpBuffer = aznew AZ::RHI::Buffer;
    AZ::RHI::BufferInitRequest bufferRequest;
    bufferRequest.m_descriptor = AZ::RHI::BufferDescriptor
    {
        AZ::RHI::BufferBindFlags::InputAssembly | AZ::RHI::BufferBindFlags::ShaderRead,
        alignedBufferSize
    };
    bufferRequest.m_buffer = tmpBuffer.get();
    AZ::RHI::ResultCode result = m_BufferPool->InitBuffer(bufferRequest);

    if (result != AZ::RHI::ResultCode::Success)
    {
        AZ_Error("PopcornFXRenderManager", false, "Failed to create GPU buffers for PopcornFX");
        return false;
    }
    outBuffer = tmpBuffer;
    outCapacity = alignedBufferSize / elemSize;
    return true;
}

//----------------------------------------------------------------------------

void    *CDemoRoomTextFeatureProcessor::_MapBuffer(AZ::RHI::Ptr<AZ::RHI::Buffer> buffer, AZ::u64 sizeToMap)
{
    if (buffer == NULL)
        return NULL;
    AZ::RHI::BufferMapRequest	mapRequest;
    mapRequest.m_buffer = buffer.get();
    mapRequest.m_byteOffset = 0;
    mapRequest.m_byteCount = sizeToMap;
    AZ::RHI::BufferMapResponse	mapResponse;
    m_BufferPool->MapBuffer(mapRequest, mapResponse);
    return mapResponse.m_data.begin()->second;;
}

//----------------------------------------------------------------------------

AZ::Data::Instance<AZ::RPI::ShaderResourceGroup>    CDemoRoomTextFeatureProcessor::_CreateShaderResourceGroup(  AZ::Data::Instance<AZ::RPI::Shader> shader,
                                                                                                                const char* srgName)
{
    AZ::Data::Asset<AZ::RPI::ShaderAsset> shaderAsset = shader->GetAsset();
    AZ::RPI::SupervariantIndex supervariantIndex = shader->GetSupervariantIndex();
    AZ::RHI::Ptr<AZ::RHI::ShaderResourceGroupLayout> srgLayout = shader->FindShaderResourceGroupLayout(AZ::Name{ srgName });
    AZ::Data::Instance<AZ::RPI::ShaderResourceGroup> srg = AZ::RPI::ShaderResourceGroup::Create(shaderAsset, supervariantIndex, srgLayout->GetName());
    if (!srg)
    {
        AZ_Error("CDemoRoomTextFeatureProcessor", false, "Failed to create shader resource group");
        return NULL;
    }
    return srg;
}

//----------------------------------------------------------------------------
