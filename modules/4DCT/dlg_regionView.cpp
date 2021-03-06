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
#include "dlg_regionView.h"
// iA
#include "dlg_4DCTFileOpen.h"

dlg_regionView::dlg_regionView( QWidget * parent )
	: QDialog( parent )
{
	setupUi( this );
	connect( pbSelectImage, SIGNAL( clicked() ), this, SLOT( onSelectButtonClicked() ) );
}

void dlg_regionView::onSelectButtonClicked()
{
	dlg_4DCTFileOpen dialog( this );
	dialog.setData( m_data );
	if( dialog.exec() != QDialog::Accepted )
		return;
	m_file = dialog.getFile();
	lFilename->setText( m_file.Name );
}

void dlg_regionView::setData( iA4DCTData * data )
{
	m_data = data;
}

QString dlg_regionView::getImagePath()
{
	return m_file.Path;
}

double dlg_regionView::getThreshold()
{
	return dspThreshold->value();
}

//void dlg_regionView::getDimension( int * dim )
//{
//	dim[0] = sbDimX->value();
//	dim[1] = sbDimY->value();
//	dim[2] = sbDimZ->value();
//}
