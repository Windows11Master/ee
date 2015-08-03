﻿//
// SettingsPage.xaml.cpp
// Implementation of the SettingsPage class
//

#include "pch.h"
#include "SettingsPage.xaml.h"
#include "EmulatorSettings.h"
#include <string>
#include <sstream>
#include <ppltasks.h>

using namespace VBA10;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Popups;
using namespace Windows::Globalization;
using namespace Windows::UI::ViewManagement;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

SettingsPage::SettingsPage()
	: initdone(false), emulator(EmulatorGame::GetInstance())
{
	InitializeComponent();
	this->touchToggle->IsOn = TouchControlsEnabled();
	this->UpdateTextBox(this->leftKeyBox, GetLeftKeyBinding());
	this->UpdateTextBox(this->rightKeyBox, GetRightKeyBinding());
	this->UpdateTextBox(this->upKeyBox, GetUpKeyBinding());
	this->UpdateTextBox(this->downKeyBox, GetDownKeyBinding());
	this->UpdateTextBox(this->startKeyBox, GetStartKeyBinding());
	this->UpdateTextBox(this->selectKeyBox, GetSelectKeyBinding());
	this->UpdateTextBox(this->aKeyBox, GetAKeyBinding());
	this->UpdateTextBox(this->bKeyBox, GetBKeyBinding());
	this->UpdateTextBox(this->lKeyBox, GetLKeyBinding());
	this->UpdateTextBox(this->rKeyBox, GetRKeyBinding());
	this->UpdateTextBox(this->turboModeBox, GetTurboKeyBinding());
	this->controllerScaleSlider->Value = (double)EmulatorSettings::Current->ControllerScale;
	this->buttonScaleSlider->Value = (double)EmulatorSettings::Current->ButtonScale;
	this->controllerOpacitySlider->Value = (double)GetControllerOpacity();
	this->deadzoneSlider->Value = (double)GetDeadzone();
	this->dpadComboBox->SelectedIndex = EmulatorSettings::Current->DPadStyle;

	//change the settings that depend on enabletouchcontrol
	touchToggle_Toggled(nullptr, nullptr);

	//video
	this->enableTurboToggle->IsOn = EmulatorSettings::Current->EnableTurbo;
	//this->skipComboBox->SelectedIndex = (GetFrameSkip() + 1 < this->skipComboBox->Items->Size) ? (GetFrameSkip() + 1) : (this->skipComboBox->Items->Size - 1);
	this->turboSkipComboBox->SelectedIndex = (EmulatorSettings::Current->TurboFrameSkip - 1 < this->turboSkipComboBox->Items->Size) ? (EmulatorSettings::Current->TurboFrameSkip - 1) : (this->turboSkipComboBox->Items->Size - 1);
	this->monitorComboBox->SelectedIndex = GetMonitorType();
	this->fpsToggle->IsOn = ShowingFPS();
	switch (GetAspectRatio())
	{
	default:
	case AspectRatioMode::Original:
		this->aspectComboBox->SelectedIndex = 0;
		break;
	case AspectRatioMode::Stretch:
		this->aspectComboBox->SelectedIndex = 1;
		break;
	case AspectRatioMode::FourToThree:
		this->aspectComboBox->SelectedIndex = 2;
		break;
	case AspectRatioMode::FiveToFour:
		this->aspectComboBox->SelectedIndex = 3;
		break;
	case AspectRatioMode::One:
		this->aspectComboBox->SelectedIndex = 4;
		break;
	}
	this->linearFilterToggle->IsOn = EmulatorSettings::Current->LinearFilterEnabled;
	this->fullscreenToggle->IsOn = EmulatorSettings::Current->FullScreen;

	//general
	this->loadConfirmationToggle->IsOn = IsLoadConfirmationDisabled();
	this->saveConfirmationToggle->IsOn = IsSaveConfirmationDisabled();
	this->soundToggle->IsOn = SoundEnabled();
	this->soundSyncToggle->IsOn = SynchronizeAudio();


	initdone = true;
}

void SettingsPage::watchVideobtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//reset xbox controller timer

	this->emulator->ResetXboxTimer();
	

	MessageDialog ^dialog = ref new MessageDialog("Thanks! Enjoy your Xbox controller for the next hour. Click this button again after the time expires to continue using Xbox controller.");
	dialog->ShowAsync();
}

void SettingsPage::touchToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (initdone)
	{
		EnableTouchControls(this->touchToggle->IsOn);
	}
	this->controllerScaleSlider->IsEnabled = this->touchToggle->IsOn;
	this->controllerOpacitySlider->IsEnabled = this->touchToggle->IsOn;
	this->deadzoneSlider->IsEnabled = this->touchToggle->IsOn;
	this->dpadComboBox->IsEnabled = this->touchToggle->IsOn;
}

void SettingsPage::UpdateTextBox(Windows::UI::Xaml::Controls::TextBox ^box, VirtualKey vk)
{
	auto s = vk.ToString();
	if (s->Length() >= 20)
	{
		s = "Unknown";
	}

	box->Text = s;
}


void SettingsPage::keyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	e->Handled = true;
}

void SettingsPage::leftKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->leftKeyBox, e->Key);
	SetLeftKeyBinding(e->Key);
}


void SettingsPage::upKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->upKeyBox, e->Key);
	SetUpKeyBinding(e->Key);
}


void SettingsPage::rightKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->rightKeyBox, e->Key);
	SetRightKeyBinding(e->Key);
}


void SettingsPage::downKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->downKeyBox, e->Key);
	SetDownKeyBinding(e->Key);
}


void SettingsPage::bKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->bKeyBox, e->Key);
	SetBKeyBinding(e->Key);
}


void SettingsPage::aKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->aKeyBox, e->Key);
	SetAKeyBinding(e->Key);
}


void SettingsPage::lKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->lKeyBox, e->Key);
	SetLKeyBinding(e->Key);
}


void SettingsPage::rKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->rKeyBox, e->Key);
	SetRKeyBinding(e->Key);
}


void SettingsPage::startKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->startKeyBox, e->Key);
	SetStartKeyBinding(e->Key);
}


void SettingsPage::selectKeyBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->selectKeyBox, e->Key);
	SetSelectKeyBinding(e->Key);
}


void SettingsPage::turboModeBox_KeyUp_1(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	this->UpdateTextBox(this->turboModeBox, e->Key);
	SetTurboKeyBinding(e->Key);
}




void SettingsPage::controllerScaleSlider_ValueChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (this->valueLabel)
	{
		wstringstream wss;
		wss << (int)e->NewValue;

		this->valueLabel->Text = ref new String(wss.str().c_str());

		if (initdone)
		{
			EmulatorSettings::Current->ControllerScale = (int)e->NewValue;
		}
	}
}


void SettingsPage::buttonScaleSlider_ValueChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (this->valueLabel2)
	{
		wstringstream wss;
		wss << (int)e->NewValue;

		this->valueLabel2->Text = ref new String(wss.str().c_str());

		if (initdone)
		{
			EmulatorSettings::Current->ButtonScale = (int)e->NewValue;
		}
	}
}

void SettingsPage::controllerOpacitySlider_ValueChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (this->opacityValueLabel)
	{
		wstringstream wss;
		wss << (int)e->NewValue;

		this->opacityValueLabel->Text = ref new String(wss.str().c_str());

		if (initdone)
		{
			SetControllerOpacity((int)e->NewValue);
		}
	}
}


void SettingsPage::dpadComboBox_SelectionChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (this->initdone)
	{
		EmulatorSettings::Current->DPadStyle = this->dpadComboBox->SelectedIndex;
	}
}


void SettingsPage::deadzoneSlider_ValueChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if (this->deadzoneLabel)
	{
		wstringstream wss;
		wss << (((int)(e->NewValue * 10.0)) / 10.0f);

		this->deadzoneLabel->Text = ref new Platform::String(wss.str().c_str());

		if (initdone)
		{
			SetDeadzone((float)e->NewValue);
		}
	}
}



//###########VIDEO SETTINGS 

void SettingsPage::monitorComboBox_SelectionChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (initdone)
	{
		SetMonitorType(this->monitorComboBox->SelectedIndex);
	}
}



//void SettingsPage::skipComboBox_SelectionChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
//{
//	if (initdone)
//	{
//		SetFrameSkip(this->skipComboBox->SelectedIndex - 1);
//	}
//}


void SettingsPage::turboSkipComboBox_SelectionChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (initdone)
	{
		EmulatorSettings::Current->TurboFrameSkip = this->turboSkipComboBox->SelectedIndex + 1;
	}
}



void SettingsPage::aspectComboBox_SelectionChanged_1(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (initdone)
	{
		AspectRatioMode mode = AspectRatioMode::Original;
		switch (this->aspectComboBox->SelectedIndex)
		{
		case 0:
			mode = AspectRatioMode::Original;
			break;
		case 1:
			mode = AspectRatioMode::Stretch;
			break;
		case 2:
			mode = AspectRatioMode::FourToThree;
			break;
		case 3:
			mode = AspectRatioMode::FiveToFour;
			break;
		case 4:
			mode = AspectRatioMode::One;
		}
		SetAspectRatio(mode);
	}
}

void SettingsPage::fpsToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (initdone)
	{
		ShowFPS(this->fpsToggle->IsOn);
	}
}


//####### SOUND SETTING
void SettingsPage::saveConfirmationToggle_Toggled_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->initdone)
	{
		DisableSaveConfirmation(this->saveConfirmationToggle->IsOn);
	}
}


void SettingsPage::loadConfirmationToggle_Toggled_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->initdone)
	{
		DisableLoadConfirmation(this->loadConfirmationToggle->IsOn);
	}
}

void SettingsPage::soundToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (initdone)
	{
		EnableSound(this->soundToggle->IsOn);
	}
}


void SettingsPage::soundSyncToggle_Toggled_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (initdone)
	{
		SetSynchronizeAudio(this->soundSyncToggle->IsOn);
	}
}




void SettingsPage::linearFilterToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (initdone)
	{
		EmulatorSettings::Current->LinearFilterEnabled = this->linearFilterToggle->IsOn;
	}
}


void SettingsPage::enableTurboToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (initdone)
	{
		EmulatorSettings::Current->EnableTurbo = this->enableTurboToggle->IsOn;
	}
}


void SettingsPage::fullscreenToggle_Toggled(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (initdone)
	{
		//save setting
		EmulatorSettings::Current->FullScreen = this->fullscreenToggle->IsOn;

		//try enter/exit full screenmode
		ApplicationView^ view = ApplicationView::GetForCurrentView();


		if (this->fullscreenToggle->IsOn)
		{
			view->TryEnterFullScreenMode();
			view->PreferredLaunchWindowingMode = ApplicationViewWindowingMode::FullScreen;
		}
		else
		{
			view->ExitFullScreenMode();
			view->PreferredLaunchWindowingMode = ApplicationViewWindowingMode::Auto;
		}
	}
}
