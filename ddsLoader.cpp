#include "ddsLoader.h"

wchar_t* multiByteToWideChar(const std::string& pKey)
{
	const  char* pCStrKey = pKey.c_str();
	//��һ�ε��÷���ת������ַ������ȣ�����ȷ��Ϊwchar_t*���ٶ����ڴ�ռ�
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t* pWCStrKey = new wchar_t[pSize];
	//�ڶ��ε��ý����ֽ��ַ���ת����˫�ֽ��ַ���
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}

void ddsLoader::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
	//*m_Device.GetAddressOf() = pd3dDevice;
    m_Device = pd3dDevice;
    m_pd3dImmediateContext = pd3dImmediateContext;

	td32.Width = 32;
	td32.Height = 32;
	td32.MipLevels = 0;
	td32.ArraySize = 1;
	td32.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td32.SampleDesc.Count = 1;
	td32.SampleDesc.Quality = 0;
	td32.Usage = D3D11_USAGE_STAGING;
	td32.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	td32.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	td32.MiscFlags = 0;

	td16.Width = 32;
	td16.Height = 32;
	td16.MipLevels = 0;
	td16.ArraySize = 1;
	td16.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td16.SampleDesc.Count = 1;
	td16.SampleDesc.Quality = 0;
	td16.Usage = D3D11_USAGE_STAGING;
	td16.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	td16.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	td16.MiscFlags = 0;

	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 0;
	srvDesc.Texture2D.MostDetailedMip = 0;

	//m_Device->CreateTexture2D(&td32, nullptr, m_Tex32.GetAddressOf());
	//m_Device->CreateTexture2D(&td16, nullptr, m_Tex16.GetAddressOf());


}

void ddsLoader::InitTex32(std::string& Texname,ComPtr<ID3D11ShaderResourceView>& ShaderResource)
{
	m_Device->CreateShaderResourceView(m_Tex32.Get(), &srvDesc, m_ShaderResource.GetAddressOf());
	std::string name = ".\\Texture\\";
	name += Texname;
	wchar_t* wname = multiByteToWideChar(name);
	DirectX::CreateDDSTextureFromFile(m_Device.Get(), wname,
		nullptr, ShaderResource.GetAddressOf(), (size_t)32, nullptr);

}

void ddsLoader::InitTex16(std::string& Texname, ComPtr<ID3D11ShaderResourceView>& ShaderResource)
{
	m_Device->CreateShaderResourceView(m_Tex16.Get(), &srvDesc, m_ShaderResource.GetAddressOf());
	std::string name = ".\\Texture\\";
	name += Texname;
	wchar_t* wname = multiByteToWideChar(name);
	DirectX::CreateDDSTextureFromFile(m_Device.Get(), wname,
		nullptr, ShaderResource.GetAddressOf(), (size_t)16, nullptr);
}


void ddsLoader::InitTex32ArrayFromFiles(
    const std::vector<std::string>& textureFileNames,
    ComPtr<ID3D11ShaderResourceView>& shaderResourceView)
{
    std::vector<ComPtr<ID3D11Texture2D>> loadedTextures(textureFileNames.size());

    // ������������Ĵ�С�͸�ʽ������ͬ��
    DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    UINT textureWidth = 0;
    UINT textureHeight = 0;

    // ���ȼ������е�����
    for (size_t i = 0; i < textureFileNames.size(); ++i)
    {
        std::wstring wTexName = L".\\Texture\\" + std::wstring(textureFileNames[i].begin(), textureFileNames[i].end());
        ComPtr<ID3D11Resource> texResource = nullptr;
        HRESULT hr = DirectX::CreateDDSTextureFromFile(
            m_Device.Get(),
            wTexName.c_str(),
            texResource.GetAddressOf(),
            nullptr, (size_t)32, nullptr);

        if (SUCCEEDED(hr))
        {
            // �����ص�������Դת��ΪID3D11Texture2D
            texResource.As(&loadedTextures[i]);

            if (i == 0) {
                // ��ȡ��һ������ĳߴ�͸�ʽ������������Ĳ���
                D3D11_TEXTURE2D_DESC desc;
                loadedTextures[i]->GetDesc(&desc);
                textureWidth = desc.Width;
                textureHeight = desc.Height;
                textureFormat = desc.Format;
            }
        }
        else
        {
            // �����������ʧ�ܵ����
            // ע�⣺����ֻ�Ǽ�ʾ����ʵ��ʹ��ʱӦ�ø���ϸ�ش������
            return;
        }
    }

    // �����յ���������
    D3D11_TEXTURE2D_DESC texArrayDesc;
    ZeroMemory(&texArrayDesc, sizeof(texArrayDesc));
    texArrayDesc.Width = textureWidth;
    texArrayDesc.Height = textureHeight;
    texArrayDesc.MipLevels = 1;  // ����ֻ��һ��Mip����
    texArrayDesc.ArraySize = static_cast<UINT>(textureFileNames.size());
    texArrayDesc.Format = textureFormat;
    texArrayDesc.SampleDesc.Count = 1;
    texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
    texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texArrayDesc.CPUAccessFlags = 0;
    texArrayDesc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> textureArray = nullptr;
    HRESULT hr = m_Device->CreateTexture2D(&texArrayDesc, nullptr, textureArray.GetAddressOf());
    if (FAILED(hr))
    {
        // ��������������ʧ�ܵ����
        return;
    }

    // ��ÿ�����ص������Ƶ�����������
    for (UINT i = 0; i < textureFileNames.size(); ++i)
    {
        if (loadedTextures[i] != nullptr)
        {
            for (UINT mipLevel = 0; mipLevel < 1; ++mipLevel)
            {
                D3D11_BOX srcBox;
                srcBox.left = 0;
                srcBox.right = textureWidth;
                srcBox.top = 0;
                srcBox.bottom = textureHeight;
                srcBox.front = 0;
                srcBox.back = 1;

                m_pd3dImmediateContext->CopySubresourceRegion(
                    textureArray.Get(), D3D11CalcSubresource(mipLevel, i, 1),
                    0, 0, 0,
                    loadedTextures[i].Get(), mipLevel,
                    &srcBox);
            }
        }
    }

    // ���������������ɫ����Դ��ͼ
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = textureFormat;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(textureFileNames.size());

    hr = m_Device->CreateShaderResourceView(textureArray.Get(), &srvDesc, shaderResourceView.GetAddressOf());
    if (FAILED(hr))
    {
        // ��������ɫ����Դ��ͼʧ�ܵ����
        return;
    }
}