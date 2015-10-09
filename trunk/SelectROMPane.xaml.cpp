﻿//
// SelectROMPane.xaml.cpp
// Implementation of the SelectROMPane class
//

#include "pch.h"

#include "SelectROMPane.xaml.h"

#include "DirectXPage.xaml.h"
#include "EmulatorSettings.h"
#include "EmulatorFileHandler.h"
#include "SelectStatePane.xaml.h"
#include "Database\ROMDBEntry.h"
#include "Converter.h"
#include "App.xaml.h"
#include "AdControl.xaml.h"



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
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage;
using namespace Windows::Storage::AccessCache;
using namespace Windows::UI::ViewManagement;
using namespace Windows::ApplicationModel::Resources;
using namespace Windows::Devices::HumanInterfaceDevice;
using namespace Windows::Devices::Enumeration;

StorageFolderModel::StorageFolderModel(StorageFolder ^folder)
{ 
	this->Folder = folder;
}

StorageFileModel::StorageFileModel(StorageFile ^file, StorageFolder ^folder)
{
	this->File = file;
	this->Folder = folder;
}

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236
SelectROMPane::SelectROMPane()
{
	initdone = false;

	this->InitializeComponent();


	auto loader = Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse();

	//move command bar to bottom if setting says so
	if ((EmulatorSettings::Current->CommandButtonPosition == 0 && Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
		|| EmulatorSettings::Current->CommandButtonPosition == 2)
	{
		svContainer->SetValue(Grid::RowProperty, 0);
		LayoutRoot->RowDefinitions->GetAt(0)->Height = GridLength(1, GridUnitType::Star);

		LayoutRoot->RowDefinitions->GetAt(1)->Height = GridLength::Auto;

		topbar->SetValue(Grid::RowProperty, 2);

		if (App::HasAds)
		{
			AdControl^ adControl = ref new AdControl();
			LayoutRoot->Children->Append(adControl);
			adControl->SetValue(Grid::RowProperty, 1);
		}
		
	}
	else
	{
		topbar->SetValue(Grid::RowProperty, 0);
		LayoutRoot->RowDefinitions->GetAt(0)->Height = GridLength::Auto;

		svContainer->SetValue(Grid::RowProperty, 1);
		LayoutRoot->RowDefinitions->GetAt(1)->Height = GridLength(1, GridUnitType::Star);

		if (App::HasAds)
		{
			AdControl^ adControl = ref new AdControl();
			LayoutRoot->Children->Append(adControl);
			adControl->SetValue(Grid::RowProperty, 2);
		}

	}




	//bind list of ROM to display
	cvsAllROMEntries->Source = App::ROMDB->AllROMDBEntries;
	romList->SelectedItem = nullptr;
	
	//refresh last played image
	ResetLastPlayedImage();
	

	//disable the command bar if no rom is loaded
	topbar->IsEnabled = IsROMLoaded();





	initdone = true;
}

void SelectROMPane::ResetLastPlayedImage()
{
	//find the most recently play game
	if (App::ROMDB->AllROMDBEntries->Size == 0) //no rom in list
	{
		lastRomGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;  //collapse
		return;
	}

	int index = 0;
	for (unsigned int i = 1; i < App::ROMDB->AllROMDBEntries->Size; i++)
	{

		if (App::ROMDB->AllROMDBEntries->GetAt(i)->LastPlayed.UniversalTime > App::ROMDB->AllROMDBEntries->GetAt(index)->LastPlayed.UniversalTime)
			index = i;
	}


	if (App::ROMDB->AllROMDBEntries->GetAt(index)->LastPlayed.UniversalTime > 0) // have been played
		lastRomGrid->DataContext = App::ROMDB->AllROMDBEntries->GetAt(index);
	else
		lastRomGrid->Visibility = Windows::UI::Xaml::Visibility::Collapsed;  //collapse
}




//void SelectROMPane::DeleteFolderClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
//{
//	Button ^button = safe_cast<Button ^>(sender);
//	StorageFolderModel ^model = safe_cast<StorageFolderModel ^>(button->DataContext);
//	StorageApplicationPermissions::FutureAccessList->Remove(model->Name);
//	this->RemoveFolderByToken(model->Name);
//	this->romDirList->ItemsSource = this->storageFolderVector;
//	this->RefreshROMList();
//}

//void SelectROMPane::RemoveFolderByToken(Platform::String ^token)
//{
//	for (int i = 0; i < this->storageFolderVector->Size; i++)
//	{
//		if(this->storageFolderVector->GetAt(i)->Name == token)
//		{
//			this->storageFolderVector->RemoveAt(i);
//			break;
//		}
//	}
//}

void SelectROMPane::RefreshROMList(void)
{
	this->storageFileVector->Clear();

	if(this->storageFolderVector->Size == 0)
	{
		StorageFolder ^localfolder = ApplicationData::Current->LocalFolder;
		create_task([localfolder]()
		{

		return localfolder->GetFileAsync("Bunny Advance (Demo).gba");

		}).then([this, localfolder](StorageFile ^file)
		{
			StorageFileModel ^model = ref new StorageFileModel(file, localfolder);
			this->storageFileVector->Append(model);
		}).then([this]()
		{
			this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
			{
				this->romList->ItemsSource = this->storageFileVector;
			}));
		}).then([](task<void> t)
		{
			try
			{
				t.get();
			}catch(...)
			{
	#if _DEBUG
				OutputDebugStringW(L"Cannot find demo ROM.");
	#endif
			}
		});
		return;
	}

	Search::QueryOptions ^options = ref new Search::QueryOptions();

	options->FileTypeFilter->Append(".gba");
	options->FileTypeFilter->Append(".gbc");
	options->FileTypeFilter->Append(".gb");

	
	
	StorageFolder ^folder = this->storageFolderVector->GetAt(0)->Folder;
	task<void> t = create_task([this, folder, options]()
	{
		return folder->CreateFileQueryWithOptions(options)->GetFilesAsync();		
	}).then([this, folder](IVectorView<StorageFile ^> ^files)
	{
		for (unsigned int i = 0; i < files->Size; i++)
		{
			StorageFileModel ^model = ref new StorageFileModel(files->GetAt(i), folder);
			this->storageFileVector->Append(model);
		}
	});

	for (unsigned int i = 1; i < this->storageFolderVector->Size; i++)
	{
		StorageFolder ^folder = this->storageFolderVector->GetAt(i)->Folder;
		t = t.then([this, folder, options]()
		{
			return folder->CreateFileQueryWithOptions(options)->GetFilesAsync();		
		}).then([this, folder](IVectorView<StorageFile ^> ^files)
		{
			for (unsigned int i = 0; i < files->Size; i++)
			{
				StorageFileModel ^model = ref new StorageFileModel(files->GetAt(i), folder);
				this->storageFileVector->Append(model);
			}
		});
	}
	
	StorageFolder ^localfolder = ApplicationData::Current->LocalFolder;
	t = t.then([localfolder]()
	{

		return localfolder->GetFileAsync("Bunny Advance (Demo).gba");

	}).then([this, localfolder](StorageFile ^file)
	{
		StorageFileModel ^model = ref new StorageFileModel(file, localfolder);
		this->storageFileVector->Append(model);
	}).then([](task<void> t)
	{
		try
		{
			t.get();
		}catch(...)
		{
#if _DEBUG
			OutputDebugStringW(L"Cannot find demo ROM.");
#endif
		}
	});

	t.then([this]()
	{
		this->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
		{
			this->romList->ItemsSource = this->storageFileVector;
		}));
	});
}


void SelectROMPane::romList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	//StorageFileModel ^model =	safe_cast<StorageFileModel ^>(this->romList->SelectedValue);


	//DirectXPage::Current->LoadROM(model->File, model->Folder);
	if (initdone && this->romList->SelectedItem)
	{
		ROMDBEntry^ entry = safe_cast<ROMDBEntry^> (this->romList->SelectedValue);
		DirectXPage::Current->LoadROM(entry);

		//create_task([ entry]
		//{
		//	return entry->Folder->GetFileAsync(entry->FileName);

		//}).then([entry](StorageFile^ file)
		//{
		//	return DirectXPage::Current->LoadROM(entry);
		//});


#if _DEBUG
		Platform::String ^message = entry->FileName;
		wstring wstr(message->Begin(), message->End());
		OutputDebugStringW(wstr.c_str());
#endif
	}
}


void SelectROMPane::saveBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (IsSaveConfirmationDisabled())
	{
		DirectXPage::Current->SaveState();
	}
	else
	{
		auto loader = Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse();

		MessageDialog ^dialog = ref new MessageDialog(loader->GetString("OverwriteSaveConfirmText"),loader->GetString("OverwriteAsk"));
		UICommand ^confirm = ref new UICommand(loader->GetString("YesText"),
			ref new UICommandInvokedHandler([this](IUICommand ^cmd)
		{
			DirectXPage::Current->SaveState();
		}));

		//UICommand ^confirmRemember = ref new UICommand("Yes, don't ask again",
		//	ref new UICommandInvokedHandler([this](IUICommand ^cmd)
		//{
		//	DisableSaveConfirmation(true);
		//	DirectXPage::Current->SaveState();
		//}));

		UICommand ^no = ref new UICommand(loader->GetString("NoText"),
			ref new UICommandInvokedHandler([this](IUICommand ^cmd)
		{
			//do nothing
		}));

		dialog->Commands->Append(confirm);
		//dialog->Commands->Append(confirmRemember); //windows phone crashes when there are 3 options
		dialog->Commands->Append(no);

		dialog->DefaultCommandIndex = 0;
		dialog->CancelCommandIndex = 1; //would be two if confirm remember is used

		dialog->ShowAsync();
	}
}


void SelectROMPane::loadBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (IsLoadConfirmationDisabled())
	{
		DirectXPage::Current->LoadState();
	}
	else
	{
		auto loader = Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse();


		MessageDialog ^dialog = ref new MessageDialog(loader->GetString("LoadConfirmPrompt"), loader->GetString("WarningText"));
		UICommand ^confirm = ref new UICommand(loader->GetString("YesText"),
			ref new UICommandInvokedHandler([this](IUICommand ^cmd)
		{
			DirectXPage::Current->LoadState();
		}));

		//UICommand ^confirmRemember = ref new UICommand("Yes, don't ask again",
		//	ref new UICommandInvokedHandler([this](IUICommand ^cmd)
		//{
		//	DisableLoadConfirmation(true);
		//	DirectXPage::Current->LoadState();
		//}));

		UICommand ^no = ref new UICommand(loader->GetString("NoText"),
			ref new UICommandInvokedHandler([this](IUICommand ^cmd)
		{
			//do nothing
		}));

		dialog->Commands->Append(confirm);
		//dialog->Commands->Append(confirmRemember);
		dialog->Commands->Append(no);

		dialog->DefaultCommandIndex = 0;
		dialog->CancelCommandIndex = 1;

		dialog->ShowAsync();
	}
}


void SelectROMPane::resetBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto loader = Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse();

	MessageDialog ^dialog = ref new MessageDialog(loader->GetString("ResetConfirmPrompt"), loader->GetString("WarningText"));
	UICommand ^confirm = ref new UICommand(loader->GetString("YesText"),
		ref new UICommandInvokedHandler([this](IUICommand ^cmd)
	{
		DirectXPage::Current->Reset();
	}));


	UICommand ^no = ref new UICommand(loader->GetString("NoText"),
		ref new UICommandInvokedHandler([this](IUICommand ^cmd)
	{
		//do nothing
	}));

	dialog->Commands->Append(confirm);
	dialog->Commands->Append(no);

	dialog->DefaultCommandIndex = 0;
	dialog->CancelCommandIndex = 1;

	dialog->ShowAsync();
}


void SelectROMPane::selectStateBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Popup ^statePopup = ref new Popup();
	statePopup->IsLightDismissEnabled = true;
	statePopup->MaxHeight = Window::Current->Bounds.Height - 48;
	statePopup->IsLightDismissEnabled = true;

	SelectStatePane ^pane = ref new SelectStatePane(GetSavestateSlot());
	statePopup->Child = pane;
	pane->Width = statePopup->Width;
	pane->MaxHeight = statePopup->MaxHeight;


	//auto transform = ((UIElement^)sender)->TransformToVisual(nullptr); //nullptr to get position related to windows
	auto transform = ((UIElement^)topbar)->TransformToVisual(nullptr);

	Windows::Foundation::Point point = transform->TransformPoint(Windows::Foundation::Point());
	statePopup->HorizontalOffset = point.X + 1; //+ selectStateBtn->ActualWidth / 2.0f - pane->Width / 2.0f;

	//command bar at bottom
	if ((EmulatorSettings::Current->CommandButtonPosition == 0 && Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
		|| EmulatorSettings::Current->CommandButtonPosition == 2)
	{

		statePopup->VerticalOffset = 0;
		
	}
	else
		statePopup->VerticalOffset = point.Y + selectStateBtn->ActualHeight;
	
	//statePopup->Measure(Windows::Foundation::Size(Window::Current->Bounds.Width, Window::Current->Bounds.Height));
	//statePopup->SetValue(Canvas::LeftProperty, Window::Current->Bounds.Left);
	//statePopup->SetValue(Canvas::TopProperty, this->windowBounds.Height - (88 + pane->DesiredSize.Height));


	statePopup->IsOpen = true;
}


void SelectROMPane::addROMbtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	DirectXPage::Current->GoToPage(2);
}


void SelectROMPane::lastRomImage_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	ROMDBEntry^ entry = (ROMDBEntry^)this->lastRomImage->DataContext;
	DirectXPage::Current->LoadROM(entry);
}

void SelectROMPane::RemoveROMButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Button ^button = safe_cast<Button ^>(sender);
	ROMDBEntry ^entry = safe_cast<ROMDBEntry ^>(button->DataContext);
}

Rect SelectROMPane::GetElementRect(FrameworkElement^ element)
{
	GeneralTransform^ buttonTransform = element->TransformToVisual(nullptr);
	const Windows::Foundation::Point pointOrig(0, 0);
	const Windows::Foundation::Point pointTransformed = buttonTransform->TransformPoint(pointOrig);
	const Rect rect(pointTransformed.X, pointTransformed.Y, safe_cast<float>(element->ActualWidth), safe_cast<float>(element->ActualHeight));
	return rect;
}


void SelectROMPane::ContextMenuBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	// Create a menu and add commands specifying a callback delegate for each.
	// Since command delegates are unique, no need to specify command Ids.
	
	Button ^button = safe_cast<Button ^>(sender);
	ROMDBEntry ^entry = safe_cast<ROMDBEntry ^>(button->DataContext);
	auto rect = GetElementRect(safe_cast<FrameworkElement^>(sender));

	ShowContextMenu( entry, rect);
	
}


void VBA10::SelectROMPane::lbAllROMMainGrid_RightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e)
{
	Grid ^grid = safe_cast<Grid ^>(sender);
	ROMDBEntry ^entry = safe_cast<ROMDBEntry ^>(grid->DataContext);

	auto point = e->GetPosition(nullptr);
	auto rect = Rect(point.X, point.Y, 10.0f, 10.0f);
	ShowContextMenu( entry, rect);
}

void SelectROMPane::ShowContextMenu(ROMDBEntry^ entry, Windows::Foundation::Rect rect)
{
	auto loader = Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse();

	auto menu = ref new PopupMenu();
	menu->Commands->Append(ref new UICommand(loader->GetString("DeleteText"), ref new UICommandInvokedHandler([this, entry, loader](IUICommand^ command)
	{
		if (IsROMLoaded() && entry->FolderPath + L"\\" + entry->FileName == ROMFile->Path) //rom is runnning
		{
			MessageDialog ^dialog = ref new MessageDialog(loader->GetString("CannotDeleteRunningROMError"));
			dialog->ShowAsync();
		}
		else
		{
			MessageDialog ^dialog;

			if (entry->LocationType == 0)  //private folder
				dialog = ref new MessageDialog(loader->GetString("DeleteROMPrompt"), loader->GetString("ConfirmText"));

			else
				dialog = ref new MessageDialog(loader->GetString("RemoveROMPrompt"), loader->GetString("ConfirmText"));

			UICommand ^confirm = ref new UICommand(loader->GetString("YesText"),
				ref new UICommandInvokedHandler([this, entry](IUICommand ^cmd)
			{
				//find the index of the entry in the list
				int idx = -1;
				for (unsigned int i = 0; i < App::ROMDB->AllROMDBEntries->Size; i++)
				{
					ROMDBEntry^ entry2 = App::ROMDB->AllROMDBEntries->GetAt(i);
					if (entry2->Token == entry->Token && entry2->FileName == entry->FileName) //found the entry
					{
						idx = i;
						break;
					}
				}

				if (idx == -1) //not found error
					return;

				if (entry->LocationType == 0)
				{
					wstring name(entry->FileName->Begin(), entry->FileName->End());
					int index = name.find_last_of('.');
					//wstring ext = name.substr(index + 1);

					wstring filenamenoext = name.substr(0, index);

					wstring snapshotname = filenamenoext + L".jpg";
					Platform::String^ psnapshotname = ref new Platform::String(snapshotname.c_str());

					wstring autosavename = filenamenoext + L"9.sgm";
					Platform::String^ pautosavename = ref new Platform::String(autosavename.c_str());

					wstring sramname = filenamenoext + L".sav";
					Platform::String^ psramname = ref new Platform::String(sramname.c_str());

					//delete the rom file
					create_task(entry->Folder->GetFileAsync(entry->FileName)).then([] (task<StorageFile^> tfile) 
					{
						try
						{
							StorageFile^ file = tfile.get();
							file->DeleteAsync();
						}
						catch (Platform::Exception^) {}
					});

					//delete the snapshot file
					create_task(entry->Folder->GetFileAsync(psnapshotname)).then([](task<StorageFile^> tfile)
					{
						try
						{
							StorageFile^ file = tfile.get();
							file->DeleteAsync();
						}
						catch (Platform::Exception^) {}
					});

					//delete the auto save file
					create_task(entry->Folder->GetFileAsync(pautosavename)).then([](task<StorageFile^> tfile)
					{
						try
						{
							StorageFile^ file = tfile.get();
							file->DeleteAsync();
						}
						catch (Platform::Exception^) {}
					});

					//delete the sram file
					create_task(entry->Folder->GetFileAsync(psramname)).then([](task<StorageFile^> tfile)
					{
						try
						{
							StorageFile^ file = tfile.get();
							file->DeleteAsync();
						}
						catch (Platform::Exception^) {}
					});

				}
				App::ROMDB->AllROMDBEntries->RemoveAt(idx);
				App::ROMDB->RemoveAsync(entry);
				ResetLastPlayedImage();
			}));


			UICommand ^no = ref new UICommand(loader->GetString("NoText"),
				ref new UICommandInvokedHandler([this](IUICommand ^cmd)
			{
				//do nothing
			}));

			dialog->Commands->Append(confirm);
			dialog->Commands->Append(no);

			dialog->DefaultCommandIndex = 1;
			dialog->CancelCommandIndex = 1;

			dialog->ShowAsync();
		}
	})));



	// We don't want to obscure content, so pass in a rectangle representing the sender of the context menu event.
	
	create_task(menu->ShowForSelectionAsync(rect, Placement::Below)).then([this](IUICommand^ command)
	{
		if (command == nullptr)
		{
			// The command is null if no command was invoked.
		}
	});
}


void SelectROMPane::maximizebtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ApplicationView^ view = ApplicationView::GetForCurrentView();


	if (view->IsFullScreenMode)
	{
		view->ExitFullScreenMode();
	}
	else
	{
		view->TryEnterFullScreenMode();
	}
}
