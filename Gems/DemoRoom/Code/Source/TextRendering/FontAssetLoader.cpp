//----------------------------------------------------------------------------
// Copyright Persistant Studios, SARL. All Rights Reserved.
// https://www.popcornfx.com/terms-and-conditions/
//----------------------------------------------------------------------------

#include "FontAssetLoader.h"
#include <AzFramework/Asset/AssetSystemBus.h>

//----------------------------------------------------------------------------

CFontAssetLoader::CFontAssetLoader()
:   m_FontTexture(NULL)
,   m_FontShader(NULL)
{
}

//----------------------------------------------------------------------------

CFontAssetLoader::~CFontAssetLoader()
{

}

//----------------------------------------------------------------------------

void	CFontAssetLoader::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
{
    _OnAssetReady(asset);
}

//----------------------------------------------------------------------------

void	CFontAssetLoader::OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset)
{
    _OnAssetReady(asset);
}

//----------------------------------------------------------------------------

void	CFontAssetLoader::OnAssetUnloaded(const AZ::Data::AssetId, const AZ::Data::AssetType)
{
}

//----------------------------------------------------------------------------

bool    CFontAssetLoader::AddTextureToLoad(const char* texturePath)
{
    if (texturePath == NULL)
        return false;
    AzFramework::AssetSystem::AssetStatus	status = AzFramework::AssetSystem::AssetStatus_Unknown;
    EBUS_EVENT_RESULT(status, AzFramework::AssetSystemRequestBus, CompileAssetSync, texturePath);
    if (status != AzFramework::AssetSystem::AssetStatus_Compiled)
    {
        AZ_Error("CFontAssetLoader", false, "Could not compile image at '%s'", texturePath);
    }
    else
    {
        AZ::Data::AssetId	streamingImageAssetId;
        EBUS_EVENT_RESULT(streamingImageAssetId, AZ::Data::AssetCatalogRequestBus, GetAssetIdByPath, texturePath, azrtti_typeid<AZ::RPI::StreamingImageAsset>(), false);
        if (!streamingImageAssetId.IsValid())
        {
            AZ_Error("CFontAssetLoader", false, "Failed to get streaming image asset id with path '%s'", texturePath);
        }
        else
        {
            m_FontTextureData = AZ::Data::AssetManager::Instance().GetAsset<AZ::RPI::StreamingImageAsset>(streamingImageAssetId, AZ::Data::AssetLoadBehavior::QueueLoad);
            if (m_FontTextureData.IsReady())
                _OnAssetReady(m_FontTextureData);
            AZ::Data::AssetBus::MultiHandler::BusConnect(streamingImageAssetId);
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------------

bool    CFontAssetLoader::AddShaderToLoad(const char* shaderPath)
{
    if (shaderPath == NULL)
        return false;
    AZ::Data::AssetId	shaderId;
    EBUS_EVENT_RESULT(shaderId, AZ::Data::AssetCatalogRequestBus, GetAssetIdByPath, shaderPath, azrtti_typeid<AZ::RPI::ShaderAsset>(), false);
    if (!shaderId.IsValid())
    {
        AZ_Error("CFontAssetLoader", false, "Could not load shader '%s'", shaderPath);
        return false;
    }
    else
    {
        m_FontShaderData = AZ::Data::AssetManager::Instance().GetAsset<AZ::RPI::ShaderAsset>(shaderId, AZ::Data::AssetLoadBehavior::QueueLoad);
        if (m_FontShaderData.IsReady())
            _OnAssetReady(m_FontShaderData);
        AZ::Data::AssetBus::MultiHandler::BusConnect(shaderId);
        return true;
    }
}

//----------------------------------------------------------------------------

void    CFontAssetLoader::_OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
{
    if (asset.GetType() == azrtti_typeid<AZ::RPI::StreamingImageAsset>())
    {
        // Material texture loaded:
        AZ::Data::Asset<AZ::RPI::StreamingImageAsset>   streamingImgAsset = AZ::Data::Asset<AZ::RPI::StreamingImageAsset>(asset);
        m_FontTexture = AZ::RPI::StreamingImage::FindOrCreate(streamingImgAsset);
    }
    else if (asset.GetType() == azrtti_typeid<AZ::RPI::ShaderAsset>())
    {
        // Material texture loaded:
        AZ::Data::Asset<AZ::RPI::ShaderAsset>   shaderAsset = AZ::Data::Asset<AZ::RPI::ShaderAsset>(asset);
        m_FontShader = AZ::RPI::Shader::FindOrCreate(shaderAsset);
    }
}

//----------------------------------------------------------------------------
