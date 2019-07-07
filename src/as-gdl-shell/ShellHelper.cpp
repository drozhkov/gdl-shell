/*
	ARCHICAD GDL Shell Add-On
	Copyright (C) 2019 Denis Rozhkov <denis@rozhkoff.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <unordered_map>
#include <memory>

#include "windows.h"
#include "shobjidl.h"

#include "include/InstanceWrapper.hpp"

#include "include/ShellHelper.hpp"


namespace asgdl {

	bool ShellHelper::Init()
	{
		return true;
	}

	void ShellHelper::Uninit()
	{
	}

	bool ShellHelper::DialogOpenFile( std::wstring & result )
	{
		InstanceWrapper<IFileDialog> wrapper( CLSID_FileOpenDialog, CLSCTX_INPROC_SERVER );

		if (!wrapper.IsGood()) {
			return false;
		}

		try {
			if (SUCCEEDED( wrapper.Get()->Show( NULL ) )) {
				IShellItem * item;

				if (SUCCEEDED( wrapper.Get()->GetResult( &item ) )) {
					LPWSTR path;

					if (SUCCEEDED( item->GetDisplayName( SIGDN_FILESYSPATH, &path ) )) {
						result.assign( path );
						return true;
					}
				}
			}
		}
		catch (...) {
		}

		return false;
	}

	bool ShellHelper::DialogSaveFile( std::wstring & result, const std::wstring & ext )
	{
		InstanceWrapper<IFileDialog> wrapper( CLSID_FileSaveDialog, CLSCTX_INPROC_SERVER );

		if (!wrapper) {
			return false;
		}

		try {
			if (!ext.empty()) {
				wrapper.Get()->SetDefaultExtension( ext.c_str() );
			}

			if (SUCCEEDED( wrapper.Get()->Show( NULL ) )) {
				IShellItem * item;

				if (SUCCEEDED( wrapper.Get()->GetResult( &item ) )) {
					LPWSTR path;

					if (SUCCEEDED( item->GetDisplayName( SIGDN_FILESYSPATH, &path ) )) {
						result.assign( path );
						return true;
					}
				}
			}
		}
		catch (...) {
		}

		return false;
	}

	bool ShellHelper::DialogOpenDirectory( std::wstring & result )
	{
		InstanceWrapper<IFileDialog> wrapper( CLSID_FileOpenDialog, CLSCTX_INPROC_SERVER );

		if (!wrapper.IsGood()) {
			return false;
		}

		try {
			FILEOPENDIALOGOPTIONS options = {};

			wrapper.Get()->GetOptions( &options );

			options |= FOS_PICKFOLDERS;

			wrapper.Get()->SetOptions( options );

			if (SUCCEEDED( wrapper.Get()->Show( NULL ) )) {
				IShellItem * item;

				if (SUCCEEDED( wrapper.Get()->GetResult( &item ) )) {
					LPWSTR path;

					if (SUCCEEDED( item->GetDisplayName( SIGDN_FILESYSPATH, &path ) )) {
						result.assign( path );
						return true;
					}
				}
			}
		}
		catch (...) {
		}

		return false;
	}

	void ShellHelper::Convert( std::string & to, std::wstring & from )
	{
		auto size = WideCharToMultiByte( CP_ACP, 0, from.c_str(), -1, NULL, 0, NULL, NULL );

		if (size > 0) {
			std::unique_ptr<char[]> buffer( new char[size] );
			WideCharToMultiByte( CP_ACP, 0, from.c_str(), -1, buffer.get(), size, NULL, NULL );
			to.assign( buffer.get() );
		}
	}

}
