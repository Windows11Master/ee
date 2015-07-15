﻿//
// ImportPage.xaml.h
// Declaration of the ImportPage class
//

#pragma once

#include "ImportPage.g.h"

using namespace Windows::Storage;

namespace VBA10
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ImportPage sealed
	{
	public:
		ImportPage();
		void ImportFile(StorageFile^ file);

	private:
		void chooseFolderbtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		StorageFolder^ tmpfolder;
		void importSavbtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
