#include "pch.h"
#include "Renderer.h"
#include "DXGI/DXGIFactory.h"
#include "DXGI/DXGIAdapter.h"
#include "Debug/D12Debug.h"
#include "RenderAPI/DirectX12/Debug/DXGIDebug.h"

#include "Utilities/d3dx12.h"
#include <vector>
#include <string>

namespace GraphicsEngine {
	using namespace Render;

	// get the dxgi format equivilent of a wic format
	DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

		else return DXGI_FORMAT_UNKNOWN;
	}

	// get a dxgi compatible wic format from another wic format
	WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
		else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

		else return GUID_WICPixelFormatDontCare;
	}

	// get the number of bits per pixel for a dxgi format
	int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
	{
		if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

		else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
		else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
	}

	int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow)
	{
		char buffer[500];
		wcstombs(buffer, filename, 500);

		FILE* imageFile;
		imageFile = fopen(buffer, "r");

		if (!imageFile) throw std::exception(buffer);
		else fclose(imageFile);

		HRESULT hr;

		// we only need one instance of the imaging factory to create decoders and frames
		static IWICImagingFactory* wicFactory;

		// reset decoder, frame and converter since these will be different for each image we load
		IWICBitmapDecoder* wicDecoder = NULL;
		IWICBitmapFrameDecode* wicFrame = NULL;
		IWICFormatConverter* wicConverter = NULL;

		bool imageConverted = false;

		if (wicFactory == NULL)
		{
			// Initialize the COM library
			CoInitialize(NULL);

			// create the WIC factory
			hr = CoCreateInstance(
				CLSID_WICImagingFactory,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&wicFactory)
			);
			if (FAILED(hr)) return 0;
		}

		// load a decoder for the image
		hr = wicFactory->CreateDecoderFromFilename(
			filename,                        // Image we want to load in
			NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
			GENERIC_READ,                    // We want to read from this file
			WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
			&wicDecoder                      // the wic decoder to be created
		);
		if (FAILED(hr)) return 0;

		// get image from decoder (this will decode the "frame")
		hr = wicDecoder->GetFrame(0, &wicFrame);
		if (FAILED(hr)) return 0;

		// get wic pixel format of image
		WICPixelFormatGUID pixelFormat;
		hr = wicFrame->GetPixelFormat(&pixelFormat);
		if (FAILED(hr)) return 0;

		// get size of image
		UINT textureWidth, textureHeight;
		hr = wicFrame->GetSize(&textureWidth, &textureHeight);
		if (FAILED(hr)) return 0;

		// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
		// out how to implement the support yourself

		// convert wic pixel format to dxgi pixel format
		DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

		// if the format of the image is not a supported dxgi format, try to convert it
		if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
		{
			// get a dxgi compatible wic format from the current image format
			WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);

			// return if no dxgi compatible format was found
			if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;

			// set the dxgi format
			dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

			// create the format converter
			hr = wicFactory->CreateFormatConverter(&wicConverter);
			if (FAILED(hr)) return 0;

			// make sure we can convert to the dxgi compatible format
			BOOL canConvert = FALSE;
			hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
			if (FAILED(hr) || !canConvert) return 0;

			// do the conversion (wicConverter will contain the converted image)
			hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
			if (FAILED(hr)) return 0;

			// this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
			imageConverted = true;
		}

		int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
		bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
		int imageSize = bytesPerRow * textureHeight; // total image size in bytes

		// allocate enough memory for the raw image data, and set imageData to point to that memory
		*imageData = (BYTE*)malloc(imageSize);

		// copy (decoded) raw image data into the newly allocated memory (imageData)
		if (imageConverted)
		{
			// if image format needed to be converted, the wic converter will contain the converted image
			hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
			if (FAILED(hr)) return 0;
		}
		else
		{
			// no need to convert, just copy data from the wic frame
			hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
			if (FAILED(hr)) return 0;
		}

		// now describe the texture with the information we have obtained from the image
		resourceDescription = {};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDescription.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
		resourceDescription.Width = textureWidth; // width of the texture
		resourceDescription.Height = textureHeight; // height of the texture
		resourceDescription.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
		resourceDescription.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
		resourceDescription.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
		resourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
		resourceDescription.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags

		// return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
		return imageSize;
	}


	Renderer::~Renderer()
	{
		Release();

//#ifdef _DEBUG
//		DXGIDebug::Get().GetLiveObjects();
//#endif
	}
	void Renderer::setScene(GraphicsScene& scene)
	{
		// Рендерер хранит device, поэтому можно провернуть что-то вроде отложенной инициализации
		// и создать все буфера моделей здесь, на моменте setScene. 
		mScene = &scene;

		for (auto& sceneObject : mScene->sceneObjects) {
			sceneObject.get().mCBPassData.Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(Render::PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		}

		for (auto& textObject : mScene->screenTexts) {
			textObject.get().InitializeFont(textObject.get().font, mWidth, mHeight);
			textObject.get().InitializeGraphicsContext(mDevice, mCommandList);
		}
		for (const auto& object : mScene->screenTextureObjects)
			object.get().mCBPassData.Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(Render::Textured::PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

		mScene->skyBox->mCBPassData.Initialize(mDevice.Get(), Utils::CalculateConstantbufferAlignment(sizeof(Render::SkyBox::PassData)), D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		LoadSkyBoxTexture();
	}

	void Renderer::Initialize(HWND hwnd, UINT width, UINT height)
	{
		mWidth = width;
		mHeight = height;
#ifdef _DEBUG
//		std::cout << "debug layer enabled";
//		D12Debug::Get().Enable();
#endif
		DXGIFactory factory;
		DXGIAdapter adapter = factory.GetAdapter();
#ifdef _DEBUG
		//DXGI_ADAPTER_DESC desc;
		//adapter->GetDesc(&desc);
		//PRINT_W_N("Selected device: " << desc.Description);
#endif
		mDevice.Init(adapter.Get());
		mDevice->SetName(L"Main virtual device");

		mCommandQueue.Initialize(mDevice.Get());
		mCommandQueue->SetName(L"Main Command Queue");

		mCommandList.Initialize(mDevice.Get());
		mCommandList->SetName(L"Main Command List");

		mSwapChain.Initialize(mDevice.Get(), factory.Get(), mCommandQueue.Get(), hwnd, mWidth, mHeight);

		mBasePipeline.InitializeAsTransparent(mDevice.Get(), L"shaders/VertexShader.hlsl", L"shaders/PixelShader.hlsl");
		mBasePipeline->SetName(L"Main graphics pipeline");

		mViewport.TopLeftX = 0;
		mViewport.TopLeftY = 0;
		mViewport.Width = mWidth;
		mViewport.Height = mHeight;
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;

		mSRRect.left = 0;
		mSRRect.right = mWidth;
		mSRRect.top = 0;
		mSRRect.bottom = mHeight;

		mDepthBuffer.InitializeAsDepthBuffer(mDevice.Get(), mWidth, mHeight);
		mDepthHeap.InitializeAsDepthHeap(mDevice.Get(), mWidth, mHeight);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsvDesc, mDepthHeap.Get()->GetCPUDescriptorHandleForHeapStart());

		mSkyBoxPipeline.InitializeAsSkybox(mDevice.Get(), L"shaders/SkyboxVertexShader.hlsl", L"shaders/SkyboxPixelShader.hlsl");
		
		mTexturedPipeline.InitializeForTextured(mDevice.Get(), L"shaders/TextureVertexShader.hlsl", L"shaders/TexturePixelShader.hlsl");
	
		mDrawing2DPipeline.InitializeForDrawing2D(mDevice.Get(), L"shaders/VertexShader2D.hlsl", L"shaders/PixelShader2D.hlsl");

		mDrawing2DTexturedPipeline.InitializeForDrawing2DTextured(mDevice.Get(), L"shaders/VertexShader2DTextured.hlsl", L"shaders/PixelShader2DTextured.hlsl");

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NumDescriptors = 3;

		mDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap));

		LoadTexture(L"textures/trampoline.jpg", mTrampolineTextureBuffer);
		LoadTexture(L"textures/start-finish-3.jpg", mStartFinishTextureBuffer);
		LoadTexture(L"textures/button.jpg", mButtonTextureBuffer);
	}

	void Renderer::Release() {
		mSkyBoxTexture.Release();
		mCommandQueue.FlushQueue();
		mDepthHeap.Release();
		mDepthBuffer.Release();
		mBasePipeline.Release();
		mSkyBoxPipeline.Release();
		mTexturedPipeline.Release();
		mDrawing2DPipeline.Release();
		if (mSwapChain.Get()) mSwapChain.Release();
		if (mCommandList.Get()) mCommandList.Release();
		if (mCommandQueue.Get()) mCommandQueue.Release();
		if (mDevice.Get()) mDevice.Reset();
	}

	void Renderer::UpdateDraw()
	{
		auto viewProjectionMatrix = mScene->camera->viewMatrix() * mScene->camera->projectionMatrix();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);

		const float clearColor[] = { 0.0f,0.0f,0.0f,0.0f };

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mSwapChain.GetCurrentRTVHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDepthHeap->GetCPUDescriptorHandleForHeapStart();

		mCommandList.GFXCmd()->ClearRenderTargetView(rtvHandle, clearColor, 0, 0);
		mCommandList.GFXCmd()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);
		mCommandList.GFXCmd()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
		mCommandList.GFXCmd()->RSSetViewports(1, &mViewport);
		mCommandList.GFXCmd()->RSSetScissorRects(1, &mSRRect);
		
		// render skybox
		{
			auto& skyBox = mScene->skyBox;

			auto cameraPosition = mScene->camera->getPosition();
			DirectX::XMFLOAT4 cameraPositionAndScaleFactor = { cameraPosition.x, cameraPosition.y, cameraPosition.z, skyBox->scaleFactor };
			std::memmove(skyBox->mCBPassData.GetCPUMemory(), &viewProjectionMatrix, sizeof(Render::SkyBox::PassData::viewProjectionMatrix));

			using byte_t = char;
			byte_t* address = reinterpret_cast<byte_t*>(skyBox->mCBPassData.GetCPUMemory()) + sizeof(viewProjectionMatrix);
			std::memmove(reinterpret_cast<void*>(address), &cameraPositionAndScaleFactor, sizeof(Render::SkyBox::PassData::cameraPositionAndScaleFactor));

			mCommandList.GFXCmd()->SetPipelineState(mSkyBoxPipeline.Get());
			mCommandList.GFXCmd()->SetGraphicsRootSignature(mSkyBoxPipeline.GetRootSignature());
			mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &skyBox->model.vertexBufferView());
			mCommandList.GFXCmd()->IASetIndexBuffer(&skyBox->model.indexBufferView());

			ID3D12DescriptorHeap* heaps[] = { mSkyBoxTexture.srvHeap().Get() };
			mCommandList.GFXCmd()->SetDescriptorHeaps(_countof(heaps), heaps);

			mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, skyBox->mCBPassData.Get()->GetGPUVirtualAddress());
			mCommandList.GFXCmd()->SetGraphicsRootDescriptorTable(1, mSkyBoxTexture.srvHeap()->GetGPUDescriptorHandleForHeapStart());

			mCommandList.GFXCmd()->DrawIndexedInstanced(skyBox->model.indicesCount(), 1, 0, 0, 0);
		}
		// render textured objects
		{
			mCommandList.GFXCmd()->SetGraphicsRootSignature(mTexturedPipeline.GetRootSignature());
			mCommandList.GFXCmd()->SetPipelineState(mTexturedPipeline.Get());
			mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			for (const auto& object : mScene->screenTextureObjects) {
				auto& sceneObject = object.get();

				Render::Textured::PassData passData{ sceneObject.transform.mat4() * viewProjectionMatrix };
				std::memmove(sceneObject.mCBPassData.GetCPUMemory(), &passData, sizeof(passData));

				ID3D12DescriptorHeap* descriptorHeaps[] = { mSrvDescriptorHeap.Get() };
				mCommandList.GFXCmd()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, sceneObject.mCBPassData.Get()->GetGPUVirtualAddress());

				if (sceneObject.texturePath.empty())
				{
					mCommandList.GFXCmd()->SetGraphicsRootDescriptorTable(1, mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				}
				else
				{
					UINT descriptorHandleIncrementSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					auto handle = mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
					handle.ptr += descriptorHandleIncrementSize;

					mCommandList.GFXCmd()->SetGraphicsRootDescriptorTable(1, handle);
				}

				mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &sceneObject.model.vertexBufferView());
				mCommandList.GFXCmd()->IASetIndexBuffer(&sceneObject.model.indexBufferView());
				mCommandList.GFXCmd()->DrawIndexedInstanced(sceneObject.model.indicesCount(), 1, 0, 0, 0);
			}
		}
		// render scene objects
		{
			mCommandList.GFXCmd()->SetGraphicsRootSignature(mBasePipeline.GetRootSignature());
			mCommandList.GFXCmd()->SetPipelineState(mBasePipeline.Get());
			mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			mScene->sceneObjects.back().get().transform.translation.y -= 0.75;

			// if (mScene)
			for (const auto& object : mScene->sceneObjects) {
				auto& sceneObject = object.get();

				Render::PassData passData{ sceneObject.transform.mat4() * viewProjectionMatrix, sceneObject.color };
				std::memmove(sceneObject.mCBPassData.GetCPUMemory(), &passData, sizeof(passData));

				mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, sceneObject.mCBPassData.Get()->GetGPUVirtualAddress());

				mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &sceneObject.model.vertexBufferView());

				if (sceneObject.model.indicesCount()) {
					mCommandList.GFXCmd()->IASetIndexBuffer(&sceneObject.model.indexBufferView());
					mCommandList.GFXCmd()->DrawIndexedInstanced(sceneObject.model.indicesCount(), 1, 0, 0, 0);
				}
				else
				{
					mCommandList.GFXCmd()->DrawInstanced(sceneObject.model.verticesCount(), 1, 0, 0);
				}
			}
			mScene->sceneObjects.back().get().transform.translation.y += 0.75;
		}
		// render 2d objects 
		{
			mCommandList.GFXCmd()->ClearDepthStencilView(mDepthHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			mCommandList.GFXCmd()->SetGraphicsRootSignature(mDrawing2DPipeline.GetRootSignature());
			mCommandList.GFXCmd()->SetPipelineState(mDrawing2DPipeline.Get());
			mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			for (int i = 0; i < mScene->sceneObjects2D->size(); ++i) {
				auto& sceneObject = mScene->sceneObjects2D->operator[](i);
				mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &sceneObject.model.vertexBufferView());
				mCommandList.GFXCmd()->IASetIndexBuffer(&sceneObject.model.indexBufferView());
				mCommandList.GFXCmd()->DrawIndexedInstanced(sceneObject.model.indicesCount(), 1, 0, 0, 0);
			}

			// button
			if (mScene->button) {
				mCommandList.GFXCmd()->ClearDepthStencilView(mDepthHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

				mCommandList.GFXCmd()->SetGraphicsRootSignature(mDrawing2DTexturedPipeline.GetRootSignature());
				mCommandList.GFXCmd()->SetPipelineState(mDrawing2DTexturedPipeline.Get());

				mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				UINT descriptorHandleIncrementSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				auto handle = mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				handle.ptr += (descriptorHandleIncrementSize * 2);

				mCommandList.GFXCmd()->SetGraphicsRootDescriptorTable(0, handle);

				mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &mScene->button->graphicsObject().model.vertexBufferView());
				mCommandList.GFXCmd()->IASetIndexBuffer(&mScene->button->graphicsObject().model.indexBufferView());
				mCommandList.GFXCmd()->DrawIndexedInstanced(mScene->button->graphicsObject().model.indicesCount(), 1, 0, 0, 0);
			}
		}
		// render text
		{
			for (const auto& text : mScene->screenTexts)
			{
				RenderText(text, text.get().pos);
			}
		}

		barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mSwapChain.GetCurrentRenderTarget();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);
		mCommandList.GFXCmd()->Close();
		mCommandQueue.ExecuteCommandList(mCommandList.Get());

		while (mCommandQueue.GetFence()->GetCompletedValue() < mCommandQueue.GetCurrentFenceValue()) {
			WaitForSingleObject(mCommandQueue.GetFence(), INFINITE);
		}
		mSwapChain.Present();
		mCommandList.ResetCommandList();
	}

	GraphicsScene* Renderer::scene()
	{
		return mScene;
	}
	
	void Renderer::LoadSkyBoxTexture()
	{
		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		HRESULT hr = DirectX::LoadDDSTextureFromFile(mDevice.Get(), mScene->skyBox->texturePath.c_str(), mSkyBoxTexture.GetAddressOf(), ddsData, subresources);

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mSkyBoxTexture.Get(), 0, static_cast<UINT>(subresources.size()));

		if (hr != S_OK) {
#ifdef _DEBUG
			//PRINT_N("Failed to load skybox texture")
#endif
				return;
		}

		// Create the GPU upload buffer.
		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

		auto desc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		ID3D12Resource* uploadRes;
		mDevice->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadRes)
		);

		UpdateSubresources(mCommandList.GFXCmd(), mSkyBoxTexture.Get(), uploadRes, 0, 0, static_cast<UINT>(subresources.size()), subresources.data());

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mSkyBoxTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		mCommandList.GFXCmd()->ResourceBarrier(1, &barrier);

		mCommandList.GFXCmd()->Close();
		mCommandQueue.ExecuteCommandList(mCommandList.Get());

		while (mCommandQueue.GetFence()->GetCompletedValue() < mCommandQueue.GetCurrentFenceValue()) {
			WaitForSingleObject(mCommandQueue.GetFence(), INFINITE);
		}

		mCommandList.ResetCommandList();

		mSkyBoxTexture->SetName(L"SkyBox texture");
		mSkyBoxTexture.CreateShaderResourceView(mDevice.Get(), D12Resource::ShaderResourceType::eTextureCube);
	}

	void Renderer::LoadTexture(const wchar_t* path, Microsoft::WRL::ComPtr<ID3D12Resource>& textureBuffer)
	{
		D3D12_RESOURCE_DESC textureDesc;
		int imageBytesPerRow;
		BYTE* imageData;
		int imageSize = LoadImageDataFromFile(&imageData, textureDesc, path, imageBytesPerRow);

#ifdef _DEBUG
		if (imageSize <= 0)
		{
			throw;
		}
#endif
		// create a default heap where the upload heap will copy its contents into (contents being the texture)
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&textureDesc, // the description of our texture
			D3D12_RESOURCE_STATE_COPY_DEST, // We will copy the texture from the upload heap to here, so we start it out in a copy dest state
			nullptr, // used for render targets and depth/stencil buffers
			IID_PPV_ARGS(&textureBuffer)
		);

		textureBuffer->SetName(L"Texture Buffer");

		UINT64 textureUploadBufferSize;
		// this function gets the size an upload buffer needs to be to upload a texture to the gpu.
		// each row must be 256 byte aligned except for the last row, which can just be the size in bytes of the row
		// eg. textureUploadBufferSize = ((((width * numBytesPerPixel) + 255) & ~255) * (height - 1)) + (width * numBytesPerPixel);
		//textureUploadBufferSize = (((imageBytesPerRow + 255) & ~255) * (textureDesc.Height - 1)) + imageBytesPerRow;
		mDevice->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);

		Microsoft::WRL::ComPtr<ID3D12Resource> textureBufferUploadHeap;

		// now we create an upload heap to upload our texture to the GPU
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), // resource description for a buffer (storing the image data in this heap just to copy to the default heap)
			D3D12_RESOURCE_STATE_GENERIC_READ, // We will copy the contents from this heap to the default heap above
			nullptr,
			IID_PPV_ARGS(&textureBufferUploadHeap));

		textureBufferUploadHeap->SetName(L"Texture Buffer Upload Resource Heap");

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = imageData;
		textureData.RowPitch = imageBytesPerRow;
		textureData.SlicePitch = imageBytesPerRow * textureDesc.Height;

		UpdateSubresources(mCommandList.GFXCmd(), textureBuffer.Get(), textureBufferUploadHeap.Get(), 0, 0, 1, &textureData);
		mCommandList.GFXCmd()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		mCommandList.GFXCmd()->Close();
		mCommandQueue.ExecuteCommandList(mCommandList.Get());

		while (mCommandQueue.GetFence()->GetCompletedValue() < mCommandQueue.GetCurrentFenceValue()) {
			WaitForSingleObject(mCommandQueue.GetFence(), INFINITE);
		}

		mCommandList.ResetCommandList();

		// now we create a shader resource view (descriptor that points to the texture and describes it)
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		static int createdTextures = 0;
		UINT descriptorHandleIncrementSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += (descriptorHandleIncrementSize * createdTextures);

		mDevice->CreateShaderResourceView(textureBuffer.Get(), &srvDesc, handle);
		
		++createdTextures;
	}

	void Renderer::RenderText(ScreenText& text, const DirectX::XMFLOAT2& pos)
	{
		if (!text.IsInitialized())
		{
			text.InitializeFont(text.font, mWidth, mHeight);
			text.InitializeGraphicsContext(mDevice, mCommandList, mCommandQueue);
		}

		auto textGraphicsContext = text.GetGraphicsContext();

		if (!text.InScreenSpace())
		{
			auto viewProjectionMatrix = mScene->camera->viewMatrix() * mScene->camera->projectionMatrix();
			auto worldViewProjectionMatrix = text.textTransformComponent.mat4() * viewProjectionMatrix;
			std::memmove(textGraphicsContext.textPassData.GetCPUMemory(), &worldViewProjectionMatrix, sizeof(worldViewProjectionMatrix));
		}

		// clear the depth buffer so we can draw over everything
		mCommandList.GFXCmd()->ClearDepthStencilView(mDepthHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// set the text pipeline state object
		mCommandList.GFXCmd()->SetPipelineState(textGraphicsContext.textPipeline.Get());

		// this way we only need 4 vertices per quad rather than 6 if we were to use a triangle list topology
		mCommandList.GFXCmd()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// set the text vertex buffer
		mCommandList.GFXCmd()->IASetVertexBuffers(0, 1, &textGraphicsContext.textVertexBufferView[mSwapChain.GetCurrentFrameIndex()]);

		ID3D12DescriptorHeap* heaps[] = { textGraphicsContext.textDescriptorHeap };
		mCommandList.GFXCmd()->SetDescriptorHeaps(_countof(heaps), heaps);

		mCommandList.GFXCmd()->SetGraphicsRootSignature(textGraphicsContext.textPipeline.GetRootSignature());
		mCommandList.GFXCmd()->SetGraphicsRootConstantBufferView(0, textGraphicsContext.textPassData.Get()->GetGPUVirtualAddress());

		// bind the text srv. We will assume the correct descriptor heap and table are currently bound and set
		mCommandList.GFXCmd()->SetGraphicsRootDescriptorTable(1, text.Font().srvHandle);

		int numCharacters = 0;

		float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
		float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

		float x = topLeftScreenX;
		float y = topLeftScreenY;

		float horrizontalPadding = (text.Font().leftpadding + text.Font().rightpadding) * text.padding.x;
		float verticalPadding = (text.Font().toppadding + text.Font().bottompadding) * text.padding.y;

		// cast the gpu virtual address to a textvertex, so we can directly store our vertices there
		Text::Vertex* vert = (Text::Vertex*)textGraphicsContext.textVBGPUAddress[mSwapChain.GetCurrentFrameIndex()];

		wchar_t lastChar = -1; // no last character to start with

		for (int i = 0; i < text.text.size(); ++i)
		{
			wchar_t c = text.text[i];

			Text::FontChar* fc = text.Font().GetChar(c);

			// character not in font char set
			if (fc == nullptr)
				continue;

			// end of string
			if (c == L'\0')
				break;

			// new line
			if (c == L'n')
			{
				x = topLeftScreenX;
				y -= (text.Font().lineHeight + verticalPadding) * text.scale.y;
				continue;
			}

			// don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
			if (numCharacters >= text.MaxNumTextCharacters())
				break;

			float kerning = 0.0f;
			if (i > 0)
				kerning = text.Font().GetKerning(lastChar, c);

			vert[numCharacters] = Text::Vertex(text.color.x,
				text.color.y,
				text.color.z,
				text.color.w,
				fc->u,
				fc->v,
				fc->twidth,
				fc->theight,
				x + ((fc->xoffset + kerning) * text.scale.x),
				y - (fc->yoffset * text.scale.y),
				fc->width * text.scale.x,
				fc->height * text.scale.y);

			numCharacters++;

			// remove horrizontal padding and advance to next char position
			x += (fc->xadvance - horrizontalPadding) * text.scale.x;

			lastChar = c;
		}

		// we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
		mCommandList.GFXCmd()->DrawInstanced(4, numCharacters, 0, 0);
	}
}