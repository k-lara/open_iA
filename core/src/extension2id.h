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
 
#ifndef extension2id_h__
#define extension2id_h__

#include <QMap>
#include "defines.h"

typedef QMap<QString, IOType> mapQString2int;

static mapQString2int fill_extensionToId()
{
	mapQString2int m;

	m[""] = UNKNOWN_READER;
	m["MHD"] = MHD_READER;
	m["MHA"] = MHD_READER;
	m["STL"] = STL_READER;
	m["RAW"] = RAW_READER;
	m["VOL"] = RAW_READER;
	m["REC"] = RAW_READER;
	m["PRO"] = PRO_READER;
	m["PARS"] = PARS_READER;
	m["VGI"] = VGI_READER;
	m["TIF"] = TIF_STACK_READER;
	m["TIFF"] = TIF_STACK_READER;
	m["JPG"] = JPG_STACK_READER;
	m["JPEG"] = JPG_STACK_READER;
	m["PNG"] = PNG_STACK_READER;
	m["BMP"] = BMP_STACK_READER;
	m["DCM"] = DCM_READER;
	m["DCM"] = DCM_WRITER;
	m["NRRD"] = NRRD_READER;
	m["PNG"] = MHD_READER;
	m["OIF"] = OIF_READER;
	m["AM"] = AM_READER;
	m["VTI"] = VTI_READER;

	return m;
}
const mapQString2int extensionToId = fill_extensionToId();

static mapQString2int fill_extensionToIdStack()
{
	mapQString2int m;

	m[""] = UNKNOWN_READER;
	m["RAW"] = VOLUME_STACK_READER;
	m["MHD"] = VOLUME_STACK_MHD_READER;
	m["VOLSTACK"] = VOLUME_STACK_VOLSTACK_READER;

	return m;
}
const mapQString2int extensionToIdStack = fill_extensionToIdStack();

#endif // extension2id_h__
