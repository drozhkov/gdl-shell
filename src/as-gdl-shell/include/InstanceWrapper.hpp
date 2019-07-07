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

#ifndef __ASGDL_INSTANCE_WRAPPER_H
#define __ASGDL_INSTANCE_WRAPPER_H


#include <exception>

#include "windows.h"


namespace asgdl {

	class InstanceException : std::exception {
	};

	template <class I> class InstanceWrapper {
	protected:
		I * m_i;

	public:
		InstanceWrapper( HRESULT hr, I * i )
			: m_i( NULL )
		{
			if (FAILED( hr )) {
				throw InstanceException;
			}

			m_i = i;
		}

		InstanceWrapper( const IID & clsId, CLSCTX clsContext )
			: m_i( NULL )
		{
			HRESULT hr = CoCreateInstance( clsId, NULL, clsContext, IID_PPV_ARGS( &m_i ) );

			if (FAILED( hr )) {
				m_i = NULL;
			}
		}

		virtual ~InstanceWrapper()
		{
			if (m_i != NULL) {
				m_i->Release();
			}
		}

		bool IsGood() const
		{
			return (m_i != NULL);
		}

		I * Get()
		{
			return m_i;
		}

		template <typename F> void Execute( const F & f )
		{
			auto hr = f();

			if (FAILED( hr )) {
				throw InstanceException;
			}
		}

		inline explicit operator bool() const
		{
			return IsGood();
		}
	};

}


#endif
