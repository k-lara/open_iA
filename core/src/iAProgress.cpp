/*********************************  open_iA 2016 06  ******************************** *
* **********  A tool for scientific visualisation and 3D image processing  ********** *
* *********************************************************************************** *
* Copyright (C) 2016  C. Heinzl, M. Reiter, A. Reh, W. Li, M. Arikan, J. Weissenb�ck, *
*                     Artem & Alexander Amirkhanov, B. Fr�hler                        *
* *********************************************************************************** *
* This program is free software: you can redistribute it and/or modify it under the   *
* terms of the GNU General Public License as published by the Free Software           *
* Foundation, either version 3 of the License, or (at your option) any later version. *
*                                                                                     *
* This program is distributed in the hope that it will be useful, but WITHOUT ANY     *
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A     *
* PARTICULAR PURPOSE.  See the GNU General Public License for more details.           *
*                                                                                     *
* You should have received a copy of the GNU General Public License along with this   *
* program.  If not, see http://www.gnu.org/licenses/                                  *
* *********************************************************************************** *
* Contact: FH O� Forschungs & Entwicklungs GmbH, Campus Wels, CT-Gruppe,              *
*          Stelzhamerstra�e 23, 4600 Wels / Austria, Email:                           *
* ************************************************************************************/
 
#include "pch.h"
#include "iAProgress.h"

#include <itkProcessObject.h>

iAProgress::iAProgress( )
{
	m_RedrawCommand = RedrawCommandType::New();
	m_RedrawCommand->SetCallbackFunction( this, &iAProgress::ProcessEvent );
	m_RedrawCommand->SetCallbackFunction( this, &iAProgress::ConstProcessEvent );
	m_ObserverTag = 0;
}


iAProgress::RedrawCommandType * iAProgress::GetRedrawCommand( void ) const
{
	return m_RedrawCommand.GetPointer();
}


void iAProgress::ProcessEvent( itk::Object * caller, const itk::EventObject & event )
{
	if( typeid( itk::ProgressEvent )   ==  typeid( event ) )
	{
		::itk::ProcessObject::Pointer  process = dynamic_cast< itk::ProcessObject *>( caller );
		const int value = static_cast<int>( process->GetProgress() * 100 );
		emit pprogress( value );
	}
}


void iAProgress::ConstProcessEvent( const itk::Object * caller, const itk::EventObject & event )
{
	if( typeid( itk::ProgressEvent )   ==  typeid( event ) ) 
	{
		itk::ProcessObject::ConstPointer  process = dynamic_cast< const itk::ProcessObject *>( caller );
		const int value = static_cast<int>( process->GetProgress() * 100 );
		emit pprogress( value );
	}
}


void iAProgress::Observe( itk::Object *caller )
{
	m_ObserverTag = caller->AddObserver(  itk::ProgressEvent(), m_RedrawCommand.GetPointer() );
	m_Caller = caller;
}


iAProgress::~iAProgress()
{
}
