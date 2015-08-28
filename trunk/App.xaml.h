﻿//
// App.xaml.h
// Declaration of the App class.
//

#pragma once

#include "App.g.h"
#include "DirectXPage.xaml.h"
#include "Database\ROMDatabase.h"
#include "EmulatorSettings.h"
#include "live_connect.h"

namespace VBA10
{
		/// <summary>
	/// Provides application-specific behavior to supplement the default Application class.
	/// </summary>
	ref class App sealed
	{
	public:
		App();
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		static ROMDatabase^ ROMDB;
		static live::live_client* LiveClient;
		static Platform::String^ ExportFolderID;
		static bool HasAds;
		static bool IsPremium;
		static void CheckProductLicense();
		
	protected:
		virtual void OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs^ args) override;

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnResuming(Platform::Object ^sender, Platform::Object ^args);
		
		void LaunchApp(Windows::ApplicationModel::Activation::ApplicationExecutionState previousState,
			Windows::ApplicationModel::Activation::FileActivatedEventArgs^ args);
		
		DirectXPage^ m_directXPage;



	};
}
