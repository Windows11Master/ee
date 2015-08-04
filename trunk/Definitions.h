#pragma once

#define DEFAULT_SNAPSHOT  L"Assets/no_snapshot.png"

using namespace Windows::Storage;

namespace VBA10
{
	public delegate void ROMSelectedDelegate(StorageFile ^file, StorageFolder ^folder);

	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class StorageFolderModel sealed
	{
	private:
	public:
		property StorageFolder ^Folder;

		property Platform::String ^Name
		{
			Platform::String ^get()
			{
				return this->Folder->DisplayName;
			}
		}

		property Platform::String ^Path
		{
			Platform::String ^get()
			{
				return this->Folder->Path;
			}
		}

		StorageFolderModel(StorageFolder ^folder);
	};

	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class StorageFileModel sealed
	{
	private:
	public:
		property StorageFile ^File;
		property StorageFolder ^Folder;

		property Platform::String ^Name
		{
			Platform::String ^get()
			{
				return this->File->Name;
			}
		}

		property Platform::String ^Path
		{
			Platform::String ^get()
			{
				return this->Folder->Path;
			}
		}

		StorageFileModel(StorageFile ^file, StorageFolder ^folder);


	};

	public enum  class OneDriveItemType: int
	{
		File,
		Folder,
		ROM,
		SRAM,
		Savestate,
		Zip,
		Rar,
		SevenZip
	};


	[Windows::UI::Xaml::Data::BindableAttribute]
	public ref class OneDriveFileItem sealed
	{
	public:
		property Platform::String^ Name;
		property OneDriveItemType Type;
		property bool Downloading;
		//public Stream Stream{ get; set; } //the Stream corresponding to this item
		property Platform::String^ OneDriveID;
		property Platform::String^ ParentID;
		property Platform::String^ OneDriveLink;
		property int FolderChildrenCount;

	};

}