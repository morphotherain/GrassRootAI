#pragma once
#include "core.h"

class ddsLoader
{
public:
	ddsLoader() = default;

	void Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);

	void InitTex32(std::string& Texname, ComPtr<ID3D11ShaderResourceView>& ShaderResource);
	void InitTex16(std::string& Texname, ComPtr<ID3D11ShaderResourceView>& ShaderResource);
	void InitTex32ArrayFromFiles(
		const std::vector<std::string>& textureFileNames,
		ComPtr<ID3D11ShaderResourceView>& shaderResourceView);

	D3D11_TEXTURE2D_DESC td32;
	D3D11_TEXTURE2D_DESC td16;

	ComPtr<ID3D11Texture2D> m_Tex16;
	ComPtr<ID3D11Texture2D> m_Tex32;

	ComPtr<ID3D11ShaderResourceView> m_ShaderResource;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;   // D3D11设备上下文
};