#include "Database\ROMDBEntry.h"
#include "Definitions.h";



using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace Platform::Collections;

namespace VBA10
{
	ROMDBEntry::ROMDBEntry(int locationtype, Platform::String^ displayname, Platform::String^ filename, Platform::String^ filepath,
		Platform::String^ snapshoturi)
		:_locationType(locationtype), _displayName(displayname), _fileName(filename), _filePath(filepath), _snapshotUri(snapshoturi)
	{
		_lastPlayed = DateTime{ 0 };
		_lastSaveIndex = 0;
		_autoSaveIndex = 0;
		_snapshot = nullptr;
		
	}

	ROMDBEntry::ROMDBEntry(int locationtype, Platform::String^ displayname, Platform::String^ filename, Platform::String^ filepath,
		DateTime lastplayed, int lastsaveindex, int autosaveindex, Platform::String^ snapshoturi)
		:_locationType(locationtype), _displayName(displayname), _fileName(filename), _filePath(filepath),
		_lastPlayed(lastplayed), _lastSaveIndex(lastsaveindex), _autoSaveIndex(autosaveindex), _snapshotUri(snapshoturi)
	{
		
		_snapshot = nullptr;
	}



}