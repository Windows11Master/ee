﻿#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "SampleFpsTextRenderer.h"
#include "EmulatorRenderer.h"
#include "Emulator.h"

// Renders Direct2D and 3D content on the screen.
namespace VBA10
{
	class VBA10Main : public DX::IDeviceNotify
	{
	public:
		VBA10Main(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~VBA10Main();
		void CreateWindowSizeDependentResources();

		void StartRenderLoop();
		void StopRenderLoop();
		Concurrency::critical_section& GetCriticalSection() { return m_criticalSection; }

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

		//variables
		EmulatorGame *emulator;
	private:

		void Update();
		bool Render();

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: Replace with your own content renderers.
		
		std::unique_ptr<EmulatorRenderer> renderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;

		Windows::Foundation::IAsyncAction^ m_renderLoopWorker;
		Concurrency::critical_section m_criticalSection;

		// Rendering loop timer.
		DX::StepTimer m_timer;


	};
}