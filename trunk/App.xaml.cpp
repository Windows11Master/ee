﻿//
// App.xaml.cpp
// Implementation of the App class.
//

#include "pch.h"
#include "App.xaml.h"
#include "DirectXPage.xaml.h"
#include "EmulatorSettings.h"
#include "live_connect.h"

using namespace VBA10;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::ApplicationModel::Store;

ROMDatabase^ VBA10::App::ROMDB = nullptr;
live::live_client* VBA10::App::LiveClient = nullptr;
Platform::String^ VBA10::App::ExportFolderID = "";
bool VBA10::App::HasAds = true;
bool VBA10::App::IsPremium = false;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>

App::App()
{
	InitializeComponent();
	Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
	Resuming += ref new EventHandler<Object^>(this, &App::OnResuming);

	ROMDB = ref new ROMDatabase();
	LiveClient = new live::live_client();
}

void App::CheckProductLicense()
{
	HasAds = true;
	IsPremium = false;


	if (CurrentApp::LicenseInformation->ProductLicenses->Lookup("noads_premium")->IsActive)
	{
		HasAds = false;
		IsPremium = true;
		return; //no need to check for other 2 licenses
	}

	if (CurrentApp::LicenseInformation->ProductLicenses->Lookup("removeads")->IsActive)
		HasAds = false;

	if (CurrentApp::LicenseInformation->ProductLicenses->Lookup("premiumfeatures")->IsActive)
		IsPremium = true;


}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used when the application is launched to open a specific file, to display
/// search results, and so forth.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
#if _DEBUG
	if (IsDebuggerPresent())
	{
		//DebugSettings->EnableFrameRateCounter = true;
	}
#endif

	//change minimum suze to 320 px
	Windows::Foundation::Size minsize = { 320.0f, 320.0f };
	ApplicationView::GetForCurrentView()->SetPreferredMinSize(minsize);

	//check license
	CheckProductLicense();

	App::ROMDB->Initialize().then([this, e] {
		if (m_directXPage == nullptr)
		{
			m_directXPage = ref new DirectXPage();
		}



		// Create a AppShell to act as the navigation context and navigate to the first page
		//auto shell = ref new AppShell();

		//if (shell->AppFrame->Content == nullptr)
		//{
		//	// When the navigation stack isn't restored navigate to the first page,
		//	// suppressing the initial entrance animation and configuring the new 
		//	// page by passing required information as a navigation parameter
		//	//shell->AppFrame->Navigate(TypeName(Views::LandingPage::typeid), e->Arguments, ref new Windows::UI::Xaml::Media::Animation::SuppressNavigationTransitionInfo());
		//}

		if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
		{
			m_directXPage->LoadInternalState(ApplicationData::Current->LocalSettings->Values);
		}

		// Place the page in the current window and ensure that it is active.
	
		Window::Current->Content = m_directXPage;
		Window::Current->Activate();
	});
	
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
	(void) sender;	// Unused parameter
	
	auto deferral = e->SuspendingOperation->GetDeferral();

	create_task(m_directXPage->SaveInternalState(ApplicationData::Current->LocalSettings->Values)).then([deferral]
	{
		deferral->Complete();
	});
}

/// <summary>
/// Invoked when application execution is being resumed.
/// </summary>
/// <param name="sender">The source of the resume request.</param>
/// <param name="args">Details about the resume request.</param>
void App::OnResuming(Object ^sender, Object ^args)
{
	(void) sender; // Unused parameter
	(void) args; // Unused parameter

	m_directXPage->LoadInternalState(ApplicationData::Current->LocalSettings->Values);
}
