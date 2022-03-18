//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AtomCore/Instance/Instance.h>
#include <Atom/RPI.Public/Image/StreamingImage.h>
#include <Atom/RPI.Public/Shader/Shader.h>

class	CFontAssetLoader
:   public AZ::Data::AssetBus::MultiHandler
{
public:
    AZ_CLASS_ALLOCATOR(CFontAssetLoader, AZ::SystemAllocator, 0);

    CFontAssetLoader();
    ~CFontAssetLoader();

    void	OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
    void	OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
    void	OnAssetUnloaded(const AZ::Data::AssetId, const AZ::Data::AssetType) override;

    bool    AddTextureToLoad(const char* texturePath);
    bool    AddShaderToLoad(const char* shaderPath);

    AZ::Data::Instance<AZ::RPI::StreamingImage> GetTexture() const { return m_FontTexture; }
    AZ::Data::Instance<AZ::RPI::Shader>         GetShader() const { return m_FontShader; }

private:
    void    _OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset);

    AZ::Data::Asset<AZ::Data::AssetData>        m_FontTextureData;
    AZ::Data::Instance<AZ::RPI::StreamingImage> m_FontTexture;

    AZ::Data::Asset<AZ::Data::AssetData>        m_FontShaderData;
    AZ::Data::Instance<AZ::RPI::Shader>         m_FontShader;
};
