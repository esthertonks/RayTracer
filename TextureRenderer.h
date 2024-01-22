#pragma once

#include <D3D11_1.h>
#include "glm/glm.hpp"

// Sets up the device context and a ID3D11RenderTargetView that the texture image will be drawn onto.
class TextureRenderer
{
public:
	TextureRenderer();
	~TextureRenderer();

	bool Initialise(HWND hwnd, glm::vec2 &textureRect);
	bool Resize(glm::vec2 renderRect);

	inline ID3D11Device* GetDevice() const
	{
		return m_device;
	}


	inline ID3D11DeviceContext* GetDeviceContext() const
	{
		return m_deviceContext;
	}

	bool BeginRender();
	void Render(uint32_t* pixels);
	void EndRender();

	inline ID3D11ShaderResourceView* GetTextureView() const
	{
		return m_textureView;
	}

	inline glm::vec2 GetTextureDimensions() const
	{
		return m_textureDimensions;
	}

private:

	void DestroyTexture();

	bool CreateRenderTargetView();
	bool RecreateRenderTargetView();
	bool CreateDXTexture();

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	IDXGISwapChain* m_swapchain;

	ID3D11Texture2D* m_texture;
	glm::vec2 m_textureDimensions;
	ID3D11ShaderResourceView* m_textureView;

	D3D11_MAPPED_SUBRESOURCE m_mappedResource;
};

