#include "TextureRenderer.h"

#include <DirectXColors.h>
#include <iostream>

TextureRenderer::TextureRenderer() :
	m_device(nullptr),
	m_deviceContext(nullptr),
	m_renderTargetView(nullptr),
	m_swapchain(nullptr),
	m_texture(nullptr),
	m_textureDimensions(0.0f, 0.0f),
	m_textureView(nullptr)
{
}

TextureRenderer::~TextureRenderer()
{

	DestroyTexture();

	if (m_deviceContext)
	{
		m_deviceContext->ClearState();
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}
	if (m_swapchain)
	{
		m_swapchain->Release();
		m_swapchain = nullptr;
	}
	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}
}

void TextureRenderer::DestroyTexture()
{
	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}
}

bool TextureRenderer::Initialise(HWND hWnd, glm::vec2 &textureRect)
{
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = 0;
	swapDesc.BufferDesc.Height = 0;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hWnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
		featureLevelArray, 2, D3D11_SDK_VERSION, &swapDesc, &m_swapchain, &m_device, &featureLevel, &m_deviceContext);
	if (FAILED(result))
	{
		std::cout << "D3D11CreateDeviceAndSwapChain failed" << std::endl;
		return false;
	}

	assert(m_device);
	assert(m_swapchain);

	if (!CreateRenderTargetView())
		return false;

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	m_textureDimensions = textureRect;

	CreateDXTexture();

	return true;
}

bool TextureRenderer::CreateDXTexture()
{
	assert(m_textureDimensions != glm::vec2(0.0f, 0.0f));

	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	textureDesc.Height = (UINT)m_textureDimensions.y;
	textureDesc.Width = (UINT)m_textureDimensions.x;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0; D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	result = m_device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(result))
	{
		std::cout << "CreateTexture2D failed" << std::endl;
		return false;
	}

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	result = m_device->CreateShaderResourceView(m_texture, &shaderResourceViewDesc, &m_textureView);
	if (FAILED(result))
	{
		std::cout << "CreateShaderResourceView failed" << std::endl;
		return false;
	}

	return true;
}

bool TextureRenderer::Resize(glm::vec2 renderRect)
{
	m_textureDimensions = renderRect;

	DestroyTexture();

	//https://learn.microsoft.com/en-us/windows/uwp/gaming/setting-up-directx-resources
	//https://learn.microsoft.com/en-gb/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi?redirectedfrom=MSDN#handling-window-resizing

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	if (m_swapchain)
	{
		// Clear render target view and depth buffer.
		m_deviceContext->OMSetRenderTargets(0, 0, 0);

		if (m_renderTargetView)
		{
			m_renderTargetView->Release();
			m_renderTargetView = nullptr;
		}

		m_swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		if(!RecreateRenderTargetView())
			return false;

		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
	}

	CreateDXTexture();

	return true;
}

bool TextureRenderer::CreateRenderTargetView()
{
	ID3D11Resource* backbuffer = nullptr;
	HRESULT result = m_swapchain->GetBuffer(0, __uuidof(backbuffer), reinterpret_cast<void**>(&backbuffer));
	if (FAILED(result))
	{
		std::cout << "swapchain get buffer failed" << std::endl;
		return false;
	}

	assert(backbuffer != nullptr);

	result = m_device->CreateRenderTargetView(backbuffer, NULL, &m_renderTargetView);
	// Release the resource to decrement the counter by one
	// This is necessary to keep it from leaking memory
	backbuffer->Release();

	if (FAILED(result))
	{
		std::cout << "CreateRenderTargetView failed" << std::endl;
		return false;
	}

	return true;
}

bool TextureRenderer::RecreateRenderTargetView()
{
	ID3D11Resource* backbuffer = nullptr;
	HRESULT result = m_swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
	if (FAILED(result))
	{
		std::cout << "swapchain get buffer failed" << std::endl;
		return false;
	}

	assert(backbuffer != nullptr);

	result = m_device->CreateRenderTargetView(backbuffer, NULL, &m_renderTargetView);
	// Release the resource to decrement the counter by one
	// This is necessary to keep it from leaking memory
	backbuffer->Release();
	if (FAILED(result))
	{
		std::cout << "CreateRenderTargetView failed" << std::endl;
		return false;
	}

	return true;
}

bool TextureRenderer::BeginRender()
{
	assert(m_renderTargetView);

	// Clear the back buffer 
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, DirectX::Colors::MidnightBlue);

	HRESULT hr = m_deviceContext->Map(m_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);
	if (FAILED(hr)) {
		std::cout << "Failed to map pixel buffer" << std::endl;
		return false;
	}

	return true;
}

void TextureRenderer::Render(uint32_t* pixels)
{
	uint32_t* destPixels = static_cast<uint32_t*>(m_mappedResource.pData);

	size_t srcRowSizeInBytes = m_textureDimensions.x * sizeof(uint32_t);
	size_t dstRowPitch = m_mappedResource.RowPitch / sizeof(uint32_t);

	// Copy row by row as the destination row size will be different in the mapped resource due to padding added 
	// by the graphics driver. See MemcpySubresource in DirectX 12 and DirectXTex sample project.
	for (UINT y = 0; y < m_textureDimensions.y; ++y) {
		std::memcpy(destPixels + y * dstRowPitch, pixels + y * (uint32_t)m_textureDimensions.x, srcRowSizeInBytes);
	}

	m_deviceContext->Unmap(m_texture, 0);

	return;
}

void TextureRenderer::EndRender()
{
	m_swapchain->Present(0, 0);

	return;
}

