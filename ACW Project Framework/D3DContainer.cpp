#include "D3DContainer.h"

D3DContainer::D3DContainer(int const screenWidth, int const screenHeight, HWND hwnd, bool const fullScreen, bool const vSyncEnabled, float const screenDepth, float const screenNear) : m_initializationFailed(false), m_vSyncEnabled(vSyncEnabled) {

	unsigned int numerator = 0;
	unsigned int denominator = 0;

	InitializeFactoryAdapter(screenWidth, screenHeight, numerator, denominator);

	if (m_initializationFailed)
	{
		return;
	}

	InitializeDeviceAndSwapChain(screenWidth, screenHeight, numerator, denominator, fullScreen, hwnd);

	if (m_initializationFailed)
	{
		return;
	}

	InitializeBuffers(screenWidth, screenHeight);

	if (m_initializationFailed)
	{
		return;
	}

	D3D11_RASTERIZER_DESC rasterDescription;

	//Initialize raster description
	ZeroMemory(&rasterDescription, sizeof(rasterDescription));

	//Initialize and create our rasterizer so we can control how the polygons can be drawn and then bind it to our context
	rasterDescription = {
		D3D11_FILL_SOLID, //FillMode
		D3D11_CULL_BACK, //CullMode
		false, //FrontCounterClockwise
		0, //DepthBias
		0.0f, //DepthBiasClamp
		0.0f, //SlopeScaledDepthBias
		true, //DepthClipEnable
		false, //ScissorEnable
		false, //MultisampleEnable
		false //AntialiasedLineEnable
	};

	auto const result = m_device->CreateRasterizerState(&rasterDescription, &m_rasterState);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	m_deviceContext->RSSetState(m_rasterState);

	//Initialize viewport
	D3D11_VIEWPORT viewport;

	ZeroMemory(&viewport, sizeof(viewport));

	//Initialize and create our viewport
	viewport = {
		0.0f, //TopLeftX
		0.0f, //TopLeftY
		static_cast<float>(screenWidth), //Width
		static_cast<float>(screenHeight), //Height
		0.0f, //MinDepth
		1.0f //MaxDepth
	};

	m_deviceContext->RSSetViewports(1, &viewport);

	//Initialize and create projection matrix
	auto const fieldOfView = static_cast<float>(XM_PI / 4.0f);
	auto const screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//Initialize world matrix by binding the identity matrix to it
	m_worldMatrix = XMMatrixIdentity();

	//Initialize and create orthographic matrix
	m_orthographicMatrix = XMMatrixOrthographicLH(static_cast<float>(screenWidth), static_cast<float>(screenHeight), screenNear, screenDepth);
};

D3DContainer::D3DContainer(const D3DContainer& other) = default;

D3DContainer::D3DContainer(D3DContainer&& other) noexcept = default;

D3DContainer::~D3DContainer()
{
	//Before shutdown we need to set our swap chain to windowed mode or an exception is thrown
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, nullptr);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}
}

D3DContainer& D3DContainer::operator=(const D3DContainer& other) = default;

//D3DContainer& D3DContainer::operator=(D3DContainer&& other) noexcept = default;

void D3DContainer::InitializeFactoryAdapter(unsigned int const screenWidth, unsigned int const screenHeight, unsigned int &numerator, unsigned int &denominator)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	DXGI_ADAPTER_DESC adapterDescription;

	size_t stringLength;

	unsigned int numOfModes;

	//Create DirectX factory
	auto result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Create adapter from factory
	result = factory->EnumAdapters(0, (&adapter));

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Enumerate primary adapter output (monitor)
	result = adapter->EnumOutputs(0, &adapterOutput);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Get the number of modes that fit the display format for our adapter output (monitor)
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numOfModes, nullptr);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Create a list to hold all possible display modes for this monitor/video card combinations
	auto* displayModeList = new DXGI_MODE_DESC[numOfModes];

	if (!displayModeList)
	{
		m_initializationFailed = true;
		return;
	}

	//Now we fill the list
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numOfModes, displayModeList);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Now we go through each possible and find the one that matches our monitor and store the numerator/denominator
	for (unsigned int i = 0; i < numOfModes; i++)
	{
		if (displayModeList[i].Width == static_cast<unsigned int>(screenWidth))
		{
			if (displayModeList[i].Height == static_cast<unsigned int>(screenHeight))
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//Get the adapter description
	result = adapter->GetDesc(&adapterDescription);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Store video card memory in megabytes
	m_videoCardMemory = static_cast<int>(adapterDescription.DedicatedVideoMemory / 1024 / 1024);

	//Convert video card name to a character array and store it
	auto const error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDescription.Description, 128);

	if (error != 0)
	{
		m_initializationFailed = true;
		return;
	}

	//Release resources
	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;
}

void D3DContainer::InitializeDeviceAndSwapChain(unsigned int const screenWidth, unsigned int const screenHeight, unsigned int const numerator, unsigned int const denominator, bool const fullScreen, HWND hwnd) {
	
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
	auto featureLevel = D3D_FEATURE_LEVEL_11_0;
	
	//DirectX Initialization

	//Initialize swap chain description
	ZeroMemory(&swapChainDescription, sizeof(swapChainDescription));

	swapChainDescription = 
	{
		//Set Buffer Description
		DXGI_MODE_DESC {
			screenWidth, //Width
			screenHeight, //Height
			DXGI_RATIONAL { //Refresh rate of swapchain
				m_vSyncEnabled ? numerator : 0,
				m_vSyncEnabled ? denominator : 1
			},
			DXGI_FORMAT_R8G8B8A8_UNORM, //Format
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, //ScanlineOrdering
			DXGI_MODE_SCALING_UNSPECIFIED //Scaling
		},

		//Set Sample Description
		DXGI_SAMPLE_DESC { // Multi-sampling (off)
			1,
			0
		},

		DXGI_USAGE_RENDER_TARGET_OUTPUT, //Backbuffer BufferUsage
		1, //BufferCount
		hwnd, //OutputWindow
		!fullScreen, //Windowed
		DXGI_SWAP_EFFECT_DISCARD, //SwapEffect
		0 //Flags
	};

	//Create swap chain, device and device context
	auto const result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDescription, &m_swapChain, &m_device, nullptr, &m_deviceContext);

	if (FAILED(result))
	{
		m_initializationFailed = true;
	}
}

void D3DContainer::InitializeBuffers(unsigned int const screenWidth, unsigned int const screenHeight) {

	ID3D11Texture2D* backBuffer;
	D3D11_TEXTURE2D_DESC depthBufferDescription;
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;

	//Attach back buffer to swap chain
	auto result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer));

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Create render target view
	result = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Release pointer
	backBuffer->Release();
	backBuffer = nullptr;

	//Initialize and create depth buffer and stencil buffer
	ZeroMemory(&depthBufferDescription, sizeof(depthBufferDescription));

	depthBufferDescription = {
		screenWidth, //Width
		screenHeight, //Height
		1, //MipLevels
		1, //ArraySize
		DXGI_FORMAT_D24_UNORM_S8_UINT, //Format
		DXGI_SAMPLE_DESC {
			1, //Count
			0 //Quality
		},
		D3D11_USAGE_DEFAULT, //Usage
		D3D11_BIND_DEPTH_STENCIL, //BindFlags
		0, //CPUAccessFlags
		0 //MiscFlags
	};

	//Create 2D texture for our buffer
	result = m_device->CreateTexture2D(&depthBufferDescription, nullptr, &m_depthStencilBuffer);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	ZeroMemory(&depthStencilDescription, sizeof(depthStencilDescription));

	depthStencilDescription = {
		true, //DepthEnable
		D3D11_DEPTH_WRITE_MASK_ALL, //DepthWriteMask
		D3D11_COMPARISON_LESS, //DepthFunc
		true, //StencilEnable
		0xFF, //StencilReadMask (Hex for 255)
		0xFF, //StencilWriteMAsk (Hex for 255)
		D3D11_DEPTH_STENCILOP_DESC { // FrontFace
			D3D11_STENCIL_OP_KEEP, //StencilFailOp
			D3D11_STENCIL_OP_INCR, //StencilDepthFailOp
			D3D11_STENCIL_OP_KEEP, //StencilPassOp
			D3D11_COMPARISON_ALWAYS //StencilFunc
		},
		D3D11_DEPTH_STENCILOP_DESC { // BackFace
			D3D11_STENCIL_OP_KEEP, //StencilFailOp
			D3D11_STENCIL_OP_DECR, //StencilDepthFailOp
			D3D11_STENCIL_OP_KEEP, //StencilPassOp
			D3D11_COMPARISON_ALWAYS //StencilFunc
		}
	};

	result = m_device->CreateDepthStencilState(&depthStencilDescription, &m_depthStencilState);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Set the depth stencil state
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//Initialize and create the view for our depth stencil
	ZeroMemory(&depthStencilViewDescription, sizeof(depthStencilViewDescription));

	depthStencilViewDescription = {
		DXGI_FORMAT_D24_UNORM_S8_UINT, //Format
		D3D11_DSV_DIMENSION_TEXTURE2D, //ViewDimension
		0 //Flags
	};

	depthStencilViewDescription.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDescription, &m_depthStencilView);

	if (FAILED(result))
	{
		m_initializationFailed = true;
		return;
	}

	//Bind our depth stencil to the render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3DContainer::BeginScene(float const red, float const green, float const blue, float const alpha)
{
	float colour[4];

	colour[0] = red;
	colour[1] = green;
	colour[2] = blue;
	colour[3] = alpha;

	//Clear back buffer
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colour);

	//Clear depth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


void D3DContainer::EndScene()
{
	//Present back buffer to the front
	if (m_vSyncEnabled)
	{
		//Locks to screen refresh rate
		m_swapChain->Present(1, 0);
	}
	else
	{
		//Presents straight away
		m_swapChain->Present(0, 0);
	}
}

ID3D11Device* D3DContainer::GetDevice() const
{
	return m_device;
}

ID3D11DeviceContext* D3DContainer::GetDeviceContext() const
{
	return m_deviceContext;
}

void D3DContainer::GetProjectionMatrix(XMMATRIX& projectionMatrix) const
{
	projectionMatrix = m_projectionMatrix;
}


void D3DContainer::GetWorldMatrix(XMMATRIX& worldMatrix) const
{
	worldMatrix = m_worldMatrix;
}

void D3DContainer::GetOrthogonalMatrix(XMMATRIX& orthographicMatrix) const
{
	orthographicMatrix = m_orthographicMatrix;
}

void D3DContainer::GetVideoCardInfo(char* cardName, int& memory) const
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}

bool D3DContainer::GetInitializationState() const
{
	return m_initializationFailed;
}
