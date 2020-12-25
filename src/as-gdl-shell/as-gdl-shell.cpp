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

#define _REPORTGD_TRANSL_

#include <cstdio>

#if defined (_MSC_VER)
#include <cstdlib>
#endif

#include <sstream>

#include "ACGDinc.h"
#include "InputOutput/FileSystem.hpp"

#include "include/ShellHelper.hpp"

#include "include/as-gdl-shell.hpp"


asgdl::ShellHelper ShellHelper;


GSErr __GDLEXT_CALL	Initialize( void )
{
	GSErr err = NoError;

	ASGDL_WRITE_REPORT( "AS-SHELL extension has been loaded", false );

	if (!ShellHelper.Init()) {
		err = Error;
	}

	return err;
}

GSErr __GDLEXT_CALL	GetAddonInfo( Int32 /*channel*/, const GS::UniString & /*filename*/, const GS::UniString & /*paramstring*/, bool * isDeterministicAddOn )
{
	GSErr err = NoError;

	*isDeterministicAddOn = false;

	return err;
}

GSErr __GDLEXT_CALL	OpenDataFile( Int32 channel, const GS::UniString & fileName, const GS::UniString & paramString, GSHandle * optionhdl )
{
	GSErr err = NoError;

	UNUSED_PARAMETER( channel );
	UNUSED_PARAMETER( optionhdl );

	std::ostringstream ss;
	ss << "AS-SHELL OPEN file <" << static_cast<const char *> (fileName.ToCStr())
		<< "> parameters <" << static_cast<const char *> (paramString.ToCStr()) << ">";

	ASGDL_WRITE_REPORT( const_cast<char *>(ss.str().c_str()), false );

	return err;
}

#ifdef ASGDL_ARCHICAD22
GSErr __GDLEXT_CALL	InputFromDataFile( Int32 channel, const GS::UniString & recordID, const GS::UniString & fieldID, Int32 nrvals, Int32 * nrgotvals, GS::Array<GdlValueRecord> & values, GS::Array<GS::UniString> & stringvals )
#else
GSErr __GDLEXT_CALL InputFromDataFile( Int32 channel, const GS::UniString & recordID, const GS::UniString & fieldID, Int32 nrvals, Int32 * nrgotvals, GDLRequestResult & values )
#endif
{
	if (nrgotvals == nullptr || nrvals < 1) {
		return Error;
	}

	GSErr err = NoError;
	*nrgotvals = 0;

	std::ostringstream ss;
	ss << "AS-SHELL INPUT channel " << channel
		<< " recordID <" << static_cast<const char *> (recordID.ToCStr())
		<< "> fieldID <" << static_cast<const char *> (fieldID.ToCStr()) << "> "
		<< nrvals << " value(s)";

	ASGDL_WRITE_REPORT( const_cast<char *>(ss.str().c_str()), false );

	bool result = false;
	std::wstring path;

	if (std::string( "file-open" ).compare( recordID.ToLowerCase().ToCStr() ) == 0) {
		result = ShellHelper.DialogOpenFile( path );
	}
	else if ((std::string( "file-save" ).compare( recordID.ToLowerCase().ToCStr() ) == 0)) {
		result = ShellHelper.DialogSaveFile( path, std::wstring( fieldID.ToUStr() ) );
	}
	else if ((std::string( "dir-open" ).compare( recordID.ToLowerCase().ToCStr() ) == 0)) {
		result = ShellHelper.DialogOpenDirectory( path );
	}

	if (!result) {
		return Error;
	}

#ifdef ASGDL_ARCHICAD22
	GdlValueRecord tmpVal;
	stringvals.Insert( 0, GS::UniString( path.c_str() ) );
	tmpVal.SetString( 0 );
	values.Push( tmpVal );
#else
	values.AddString( GS::UniString( path.c_str() ) );
#endif

	*nrgotvals = 1;

	return err;
}

#ifdef ASGDL_ARCHICAD22
GSErr __GDLEXT_CALL OutputToDataFile( Int32, const GS::UniString &, const GS::UniString &, const GS::Array<GdlValueRecord> &, const GS::Array<GS::UniString> & )
#else
GSErr __GDLEXT_CALL OutputToDataFile( Int32, const GS::UniString &, const GS::UniString &, const GDLRequestResult & )
#endif
{
	return NoError;
}

GSErr __GDLEXT_CALL	CloseDataFile( Int32 channel )
{
	GSErr err = NoError;

	std::ostringstream ss;
	ss << "AS-SHELL CLOSE channel " << channel;
	ASGDL_WRITE_REPORT( const_cast<char *>(ss.str().c_str()), false );

	return err;
}

GSErr __GDLEXT_CALL	FreeData( void )
{
	GSErr err = NoError;

	ASGDL_WRITE_REPORT( "Unloading AS-SHELL extension...", false );

	ShellHelper.Uninit();

	return err;
}
