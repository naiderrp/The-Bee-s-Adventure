#include "pch.h"
#include "ScreenText.h"

#include "Utilities/Utilities.h"

#include <filesystem>

namespace GraphicsEngine {
	void ScreenText::InitializeGraphicsContext(D12Device& device, D12CommandList& commandList)
	{
		auto mDevice = device.Get();
		mTextPipeline.InitializeForDrawingText(mDevice, L"shaders/TextVertexShader.hlsl", L"shaders/TextPixelShader.hlsl");

		// create text vertex buffer committed resources
		{
			for (int i = 0; i < _countof(mTextVertexBuffer); ++i)
			{
				// create upload heap. We will fill this with data for our text
				ID3D12Resource* vBufferUploadHeap;
				mDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
					D3D12_HEAP_FLAG_NONE, // no flags
					&CD3DX12_RESOURCE_DESC::Buffer(mMaxNumTextCharacters * sizeof(Render::Text::Vertex)), // resource description for a buffer
					D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
					nullptr,
					IID_PPV_ARGS(mTextVertexBuffer[i].GetAddressOf()));

				mTextVertexBuffer[i]->SetName(L"Text Vertex Buffer Upload Resource Heap");

				CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

				// map the resource heap to get a gpu virtual address to the beginning of the heap
				mTextVertexBuffer[i]->Map(0, &readRange, reinterpret_cast<void**>(&mTextVBGPUAddress[i]));
			}
		}

		// set the text vertex buffer view for each frame
		{
			for (int i = 0; i < _countof(mTextVertexBuffer); ++i)
			{
				mTextVertexBufferView[i].BufferLocation = mTextVertexBuffer[i]->GetGPUVirtualAddress();
				mTextVertexBufferView[i].StrideInBytes = sizeof(Render::Text::Vertex);
				mTextVertexBufferView[i].SizeInBytes = mMaxNumTextCharacters * sizeof(Render::Text::Vertex);
			}
		}

		// Load the image from file
		D3D12_RESOURCE_DESC fontTextureDesc;
		int fontImageBytesPerRow;
		BYTE* fontImageData;

		int fontImageSize = Utils::LoadImageDataFromFile(&fontImageData, fontTextureDesc, mFont.fontImage.c_str(), fontImageBytesPerRow);
#ifdef _DEBUG
		if (!fontImageSize) {
			//PRINT_N("Can't load image")
				throw;
		}
#endif
		// create the font texture resource
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&fontTextureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&mFont.textureBuffer));

		mFont.textureBuffer->SetName(L"Font Texture Buffer Resource Heap");

		ID3D12Resource* fontTextureBufferUploadHeap;
		UINT64 fontTextureUploadBufferSize;
		mDevice->GetCopyableFootprints(&fontTextureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &fontTextureUploadBufferSize);

		// create an upload heap to copy the texture to the gpu
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(fontTextureUploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&fontTextureBufferUploadHeap));
		fontTextureBufferUploadHeap->SetName(L"Font Texture Buffer Upload Resource Heap");

		// store font image in upload heap
		D3D12_SUBRESOURCE_DATA fontTextureData = {};
		fontTextureData.pData = &fontImageData[0]; // pointer to our image data
		fontTextureData.RowPitch = fontImageBytesPerRow; // size of all our triangle vertex data
		fontTextureData.SlicePitch = fontImageBytesPerRow * fontTextureDesc.Height; // also the size of our triangle vertex data

		// Now we copy the upload buffer contents to the default heap
		UpdateSubresources(commandList.GFXCmd(), mFont.textureBuffer, fontTextureBufferUploadHeap, 0, 0, 1, &fontTextureData);

		// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
		commandList.GFXCmd()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mFont.textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		API_EVAL_HR(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mTextDescriptorHeap.GetAddressOf())), "Failed to create srv heap");

		// create an srv for the font
		D3D12_SHADER_RESOURCE_VIEW_DESC fontsrvDesc = {};
		fontsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		fontsrvDesc.Format = fontTextureDesc.Format;
		fontsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		fontsrvDesc.Texture2D.MipLevels = 1;

		// we need to get the next descriptor location in the descriptor heap to store this srv
		auto srvHandleSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		mFont.srvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mTextDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, srvHandleSize);

		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(mTextDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 0, srvHandleSize);
		mDevice->CreateShaderResourceView(mFont.textureBuffer, &fontsrvDesc, srvHandle);

		mTextPassData.Initialize(mDevice, Utils::CalculateConstantbufferAlignment(sizeof(DirectX::XMMATRIX)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	void ScreenText::InitializeGraphicsContext(D12Device& device, D12CommandList& commandList, D12CommandQueue& commandQueue)
	{
		auto mDevice = device.Get();
		mTextPipeline.InitializeForDrawingText(mDevice, L"shaders/TextVertexShader.hlsl", L"shaders/TextPixelShader.hlsl");

		// create text vertex buffer committed resources
		{
			for (int i = 0; i < _countof(mTextVertexBuffer); ++i)
			{
				// create upload heap. We will fill this with data for our text
				ID3D12Resource* vBufferUploadHeap;
				mDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
					D3D12_HEAP_FLAG_NONE, // no flags
					&CD3DX12_RESOURCE_DESC::Buffer(mMaxNumTextCharacters * sizeof(Render::Text::Vertex)), // resource description for a buffer
					D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
					nullptr,
					IID_PPV_ARGS(mTextVertexBuffer[i].GetAddressOf()));

				mTextVertexBuffer[i]->SetName(L"Text Vertex Buffer Upload Resource Heap");

				CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

				// map the resource heap to get a gpu virtual address to the beginning of the heap
				mTextVertexBuffer[i]->Map(0, &readRange, reinterpret_cast<void**>(&mTextVBGPUAddress[i]));
			}
		}

		// set the text vertex buffer view for each frame
		{
			for (int i = 0; i < _countof(mTextVertexBuffer); ++i)
			{
				mTextVertexBufferView[i].BufferLocation = mTextVertexBuffer[i]->GetGPUVirtualAddress();
				mTextVertexBufferView[i].StrideInBytes = sizeof(Render::Text::Vertex);
				mTextVertexBufferView[i].SizeInBytes = mMaxNumTextCharacters * sizeof(Render::Text::Vertex);
			}
		}

		// Load the image from file
		D3D12_RESOURCE_DESC fontTextureDesc;
		int fontImageBytesPerRow;
		BYTE* fontImageData;

		int fontImageSize = Utils::LoadImageDataFromFile(&fontImageData, fontTextureDesc, mFont.fontImage.c_str(), fontImageBytesPerRow);
#ifdef _DEBUG
		if (!fontImageSize) {
			//PRINT_N("Can't load image")
				throw;
		}
#endif
		// create the font texture resource
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&fontTextureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&mFont.textureBuffer));

		mFont.textureBuffer->SetName(L"Font Texture Buffer Resource Heap");

		ID3D12Resource* fontTextureBufferUploadHeap;
		UINT64 fontTextureUploadBufferSize;
		mDevice->GetCopyableFootprints(&fontTextureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &fontTextureUploadBufferSize);

		// create an upload heap to copy the texture to the gpu
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(fontTextureUploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&fontTextureBufferUploadHeap));
		fontTextureBufferUploadHeap->SetName(L"Font Texture Buffer Upload Resource Heap");

		// store font image in upload heap
		D3D12_SUBRESOURCE_DATA fontTextureData = {};
		fontTextureData.pData = &fontImageData[0]; // pointer to our image data
		fontTextureData.RowPitch = fontImageBytesPerRow; // size of all our triangle vertex data
		fontTextureData.SlicePitch = fontImageBytesPerRow * fontTextureDesc.Height; // also the size of our triangle vertex data

		// Now we copy the upload buffer contents to the default heap
		UpdateSubresources(commandList.GFXCmd(), mFont.textureBuffer, fontTextureBufferUploadHeap, 0, 0, 1, &fontTextureData);

		// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
		commandList.GFXCmd()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mFont.textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		API_EVAL_HR(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(mTextDescriptorHeap.GetAddressOf())), "Failed to create srv heap");

		// create an srv for the font
		D3D12_SHADER_RESOURCE_VIEW_DESC fontsrvDesc = {};
		fontsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		fontsrvDesc.Format = fontTextureDesc.Format;
		fontsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		fontsrvDesc.Texture2D.MipLevels = 1;

		// we need to get the next descriptor location in the descriptor heap to store this srv
		auto srvHandleSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		mFont.srvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mTextDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, srvHandleSize);

		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(mTextDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 0, srvHandleSize);
		mDevice->CreateShaderResourceView(mFont.textureBuffer, &fontsrvDesc, srvHandle);

		D12Resource tmpPassDataBuffer;
		tmpPassDataBuffer.Initialize(mDevice, Utils::CalculateConstantbufferAlignment(sizeof(DirectX::XMMATRIX)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		auto matrix = DirectX::XMMatrixIdentity();
		std::memmove(tmpPassDataBuffer.GetCPUMemory(), &matrix, sizeof(matrix));

		mTextPassData.Initialize(mDevice, Utils::CalculateConstantbufferAlignment(sizeof(DirectX::XMMATRIX)), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ);

		commandList.GFXCmd()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mTextPassData.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		commandList.GFXCmd()->CopyBufferRegion(mTextPassData.Get(), 0, tmpPassDataBuffer.Get(), 0, sizeof(matrix));

		commandList.GFXCmd()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mTextPassData.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		commandList.GFXCmd()->Close();

		commandQueue.ExecuteCommandList(commandList.Get());

		while (commandQueue.GetFence()->GetCompletedValue() < commandQueue.GetCurrentFenceValue()) {
			WaitForSingleObject(commandQueue.GetFence(), INFINITE);
		}

		tmpPassDataBuffer.Release();
		commandList.ResetCommandList();
	}

	void ScreenText::InitializeFont(LPCWSTR fontFile, int windowWidth, int windowHeight)
	{
		mWidth = windowWidth;
		mHeight = windowHeight;
		mFont = LoadFontFromFile(fontFile, mWidth, mHeight);
		mFont.fontImage = L"fonts/" + mFont.fontImage;
	}

	Render::Text::GraphicsContext ScreenText::GetGraphicsContext()
	{
		Render::Text::GraphicsContext context = {
			mTextPipeline,
			mTextPassData,
			mTextVertexBuffer->Get(),
			mTextDescriptorHeap.Get(),
			mTextVertexBufferView,
			mTextVBGPUAddress
		};

		return context;
	}

	Render::Text::Font ScreenText::LoadFontFromFile(LPCWSTR filename, int windowWidth, int windowHeight)
	{
		std::wifstream fs;

#ifdef _DEBUG
	//	if (!std::filesystem::exists(filename)) {
			//PRINT_W_N("Can't open file: " << filename)
			//	throw;
		//}
#endif
		fs.open(filename);

		Render::Text::Font font;
		std::wstring tmp;
		int startpos;

		// extract font name
		fs >> tmp >> tmp; // info face="Arial"
		startpos = tmp.find(L"\"") + 1;
		font.name = tmp.substr(startpos, tmp.size() - startpos - 1);

		// get font size
		fs >> tmp; // size=73
		startpos = tmp.find(L"=") + 1;
		font.size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
		fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

		// get padding
		fs >> tmp; // padding=5,5,5,5 
		startpos = tmp.find(L"=") + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

		// get up padding
		startpos = tmp.find(L",") + 1;
		font.toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

		// get right padding
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		startpos = tmp.find(L",") + 1;
		font.rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

		// get down padding
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		startpos = tmp.find(L",") + 1;
		font.bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

		// get left padding
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		font.leftpadding = std::stoi(tmp) / (float)windowWidth;

		fs >> tmp; // spacing=0,0

		// get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
		fs >> tmp >> tmp; // common lineHeight=95
		startpos = tmp.find(L"=") + 1;
		font.lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

		// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
		fs >> tmp; // base=68
		startpos = tmp.find(L"=") + 1;
		font.baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

		// get texture width
		fs >> tmp; // scaleW=512
		startpos = tmp.find(L"=") + 1;
		font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get texture height
		fs >> tmp; // scaleH=512
		startpos = tmp.find(L"=") + 1;
		font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get pages, packed, page id
		fs >> tmp >> tmp; // pages=1 packed=0
		fs >> tmp >> tmp; // page id=0

		// get texture filename
		std::wstring wtmp;
		fs >> wtmp; // file="Arial.png"
		startpos = wtmp.find(L"\"") + 1;
		font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

		// get number of characters
		fs >> tmp >> tmp; // chars count=97
		startpos = tmp.find(L"=") + 1;
		font.numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// initialize the character list
		font.CharList = new Render::Text::FontChar[font.numCharacters];

		for (int c = 0; c < font.numCharacters; ++c)
		{
			// get unicode id
			fs >> tmp >> tmp; // char id=0
			startpos = tmp.find(L"=") + 1;
			font.CharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

			// get x
			fs >> tmp; // x=392
			startpos = tmp.find(L"=") + 1;
			font.CharList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureWidth;

			// get y
			fs >> tmp; // y=340
			startpos = tmp.find(L"=") + 1;
			font.CharList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureHeight;

			// get width
			fs >> tmp; // width=47
			startpos = tmp.find(L"=") + 1;
			tmp = tmp.substr(startpos, tmp.size() - startpos);
			font.CharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
			font.CharList[c].twidth = (float)std::stoi(tmp) / (float)font.textureWidth;

			// get height
			fs >> tmp; // height=57
			startpos = tmp.find(L"=") + 1;
			tmp = tmp.substr(startpos, tmp.size() - startpos);
			font.CharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
			font.CharList[c].theight = (float)std::stoi(tmp) / (float)font.textureHeight;

			// get xoffset
			fs >> tmp; // xoffset=-6
			startpos = tmp.find(L"=") + 1;
			font.CharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

			// get yoffset
			fs >> tmp; // yoffset=16
			startpos = tmp.find(L"=") + 1;
			font.CharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

			// get xadvance
			fs >> tmp; // xadvance=65
			startpos = tmp.find(L"=") + 1;
			font.CharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

			// get page
			// get channel
			fs >> tmp >> tmp; // page=0    chnl=0
		}

		// get number of kernings
		fs >> tmp >> tmp; // kernings count=96
		startpos = tmp.find(L"=") + 1;
		font.numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// initialize the kernings list
		font.KerningsList = new Render::Text::FontKerning[font.numKernings];

		for (int k = 0; k < font.numKernings; ++k)
		{
			// get first character
			fs >> tmp >> tmp; // kerning first=87
			startpos = tmp.find(L"=") + 1;
			font.KerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

			// get second character
			fs >> tmp; // second=45
			startpos = tmp.find(L"=") + 1;
			font.KerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

			// get amount
			fs >> tmp; // amount=-1
			startpos = tmp.find(L"=") + 1;
			int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
			font.KerningsList[k].amount = (float)t / (float)windowWidth;
		}
		return font;
	}
}