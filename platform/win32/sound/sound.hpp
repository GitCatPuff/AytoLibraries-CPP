
#pragma once


#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <wrl.h>
#include <comdef.h>


namespace ayto::exceptions
{
	class wasapi_error : public exception_base
	{};
}

#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define STR_INDIR(x) #x
#define STR(x) STR_INDIR(x)

#define THROW_IF_FAILED(hr)                                                                                   \
do																											  \
{																											  \
	if (FAILED(hr))																							  \
	{																										  \
		_com_error error(hr);																				  \
		std::wstring error_string;																			  \
		error_string += L"Exception: \n    ";																  \
		error_string += error.ErrorMessage();																  \
		error_string += L"\n    File: " WIDEN(__FILE__) L"\n    Line: " WIDEN(STR(__LINE__));				  \
																											  \
		throw wasapi_error( error_string.c_str() );															  \
	}                               																		  \
} while (false)

namespace ayto::v10::win32::sound
{

	void initialize()
	{
		THROW_IF_FAILED(CoInitialize(nullptr));
	}
	void deinitialize()
	{
		CoUninitialize();
	}

	struct audio_source
	{
	private:
		struct sample
		{
			FLOAT left;
			FLOAT right;
		};

		FLOAT get_frame_value(const double normalized_position) const
		{
			return std::sin(normalized_position * math::constants::tau);
		}
		FLOAT get_frame_value_square(const double normalized_position) const
		{
			if (normalized_position < 0.5)
				return 1 * 0.2;
			else
				return -1 * 0.2;
		}

	public:
		HRESULT set_format(PWAVEFORMATEX pWfx)
		{
			std::cout
				<< std::format(
					"Format Tag: {}\n"
					"Number Of Channels: {}\n"
					"Samples Per Second: {}\n"
					"Average Bytes Per Second: {}\n"
					"Block Align: {}\n"
					"Bits Per Sample: {}\n",
					pWfx->wFormatTag,
					pWfx->nChannels,
					pWfx->nSamplesPerSec,
					pWfx->nAvgBytesPerSec,
					pWfx->nBlockAlign,
					pWfx->wBitsPerSample
				);
			return S_OK;
		}

		const double seconds = 8;
		const UINT32 max_frames = 48'000 * seconds;
		UINT32 written_frames = 0;
		double frequency = 220;

		HRESULT load_data(UINT32 numFrames, void* pData, DWORD* flags)
		{
			if (written_frames >= max_frames)
			{
				*flags = AUDCLNT_BUFFERFLAGS_SILENT;
				return S_OK;
			}
			*flags = 0;

			const UINT32 wave_frames = 48'000.0 / frequency;
			sample* samples = static_cast<sample*>(pData);
			std::cout << numFrames << '\n';

			for (UINT32 i{}; i < numFrames; i++)
			{
				const auto current_written_frames = i + written_frames;
				const auto current_ratio = (double)current_written_frames / (double)max_frames;

				frequency = math::smoothstep(0.0, 1.0, current_ratio) * 110 + 10.0 * math::smoothstep(0.0, 1.0, current_ratio) + 20;
				const auto mod = std::fmod(current_ratio * seconds, 1.0 / frequency) * frequency;

				auto output = get_frame_value(mod) * sin(current_ratio * math::constants::pi) * 0.2;

				frequency = math::smoothstep(0.0, 1.0, current_ratio) * 50 + 110.0 * math::smoothstep(0.0, 1.0, current_ratio) + 20;
				const auto mod2 = std::fmod(current_ratio * seconds, 1.0 / frequency) * frequency;
				output += get_frame_value(mod2) * sin(current_ratio * math::constants::pi) * 0.2;


				frequency = math::smoothstep(0.0, 1.0, current_ratio) * 60 + 60.0 * math::smoothstep(0.0, 1.0, current_ratio) + 20;
				const auto mod3 = std::fmod(current_ratio * seconds, 1.0 / frequency) * frequency;
				output += get_frame_value(mod3) * sin(current_ratio * math::constants::pi) * 0.2;

				samples[i].left = output * sin(current_ratio);
				samples[i].right = output * sin(current_ratio + math::constants::pi);
			}
			written_frames += numFrames;

			return S_OK;
		}
	};




	static constexpr const auto REFTIMES_PER_SEC = 10000000;
	static constexpr auto REFTIMES_PER_MILLISEC = 10000;
	HRESULT play_sound(audio_source* pMySource)
	{
		using Microsoft::WRL::ComPtr;

		HRESULT hr;
		REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
		REFERENCE_TIME hnsActualDuration;

		ComPtr<IMMDeviceEnumerator> pDeviceEnumerator;
		ComPtr<IMMDevice> pDevice;
		ComPtr<IAudioClient> pAudioClient;
		ComPtr<IAudioRenderClient> pAudioRenderClient;

		hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), nullptr,
			CLSCTX_ALL,
			IID_PPV_ARGS(pDeviceEnumerator.GetAddressOf())
		);
		THROW_IF_FAILED(hr);

		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, pDevice.GetAddressOf()
		);
		THROW_IF_FAILED(hr);

		hr = pDevice->Activate(
			__uuidof(IAudioClient), CLSCTX_ALL,
			nullptr, (void**)pAudioClient.GetAddressOf()
		);
		THROW_IF_FAILED(hr);

		PWAVEFORMATEX pWfxRawPtr;
		hr = pAudioClient->GetMixFormat(&pWfxRawPtr);
		THROW_IF_FAILED(hr);
		std::unique_ptr < WAVEFORMATEX, decltype([](auto rawPtr) {
			CoTaskMemFree(rawPtr);
			}) > pWfx(pWfxRawPtr);

			hr = pAudioClient->Initialize(
				AUDCLNT_SHAREMODE_SHARED,
				0, hnsRequestedDuration,
				0, pWfx.get(), NULL
			);
			THROW_IF_FAILED(hr);

			hr = pMySource->set_format(pWfx.get());
			THROW_IF_FAILED(hr);

			UINT32 bufferFrameCount;
			hr = pAudioClient->GetBufferSize(&bufferFrameCount);
			THROW_IF_FAILED(hr);

			hr = pAudioClient->GetService(
				IID_PPV_ARGS(pAudioRenderClient.GetAddressOf())
			);
			THROW_IF_FAILED(hr);

			BYTE* pData;
			hr = pAudioRenderClient->GetBuffer(bufferFrameCount, &pData);
			THROW_IF_FAILED(hr);

			DWORD flags = 0;
			hr = pMySource->load_data(bufferFrameCount, pData, &flags);
			THROW_IF_FAILED(hr);

			hr = pAudioRenderClient->ReleaseBuffer(bufferFrameCount, flags);
			THROW_IF_FAILED(hr);

			hnsActualDuration =
				(double)REFTIMES_PER_SEC
				* bufferFrameCount / pWfx->nSamplesPerSec;

			hr = pAudioClient->Start();
			THROW_IF_FAILED(hr);

			while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
			{
				Sleep((DWORD)hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

				UINT32 numFramesPadding;
				hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
				THROW_IF_FAILED(hr);

				UINT32 numFramesAvailable = bufferFrameCount - numFramesPadding;

				hr = pAudioRenderClient->GetBuffer(numFramesAvailable, &pData);
				THROW_IF_FAILED(hr);

				hr = pMySource->load_data(numFramesAvailable, pData, &flags);
				THROW_IF_FAILED(hr);

				hr = pAudioRenderClient->ReleaseBuffer(numFramesAvailable, flags);
				THROW_IF_FAILED(hr);
			}
			Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

			hr = pAudioClient->Stop();
			THROW_IF_FAILED(hr);

			return hr;
	}



}