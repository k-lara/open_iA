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
 
#ifndef iAXRFAttachment_h__
#define iAXRFAttachment_h__

#include "iAModuleInterface.h"
#include "iAModuleAttachmentToChild.h"

#include <vtkSmartPointer.h>

class MainWindow;
class MdiChild;
class dlg_periodicTable;
class dlg_RefSpectra;
class dlg_SimilarityMap;
class dlg_XRF;
class iASlicer;
class iAIO;

class vtkPiecewiseFunction;

class iAXRFAttachment : public iAModuleAttachmentToChild
{
	Q_OBJECT

public:
	iAXRFAttachment( MainWindow * mainWnd, iAChildData childData );
	~iAXRFAttachment();

Q_SIGNALS:
	void xrfLoaded();

private slots:
	void visualizeXRF( int isOn );
	void updateXRFOpacity( int value );
	void updateXRF();
	void updateXRFVoxelEnergy( int x, int y, int z, int mode );
	void xrfLoadingDone();
	void xrfLoadingFailed();
	void reInitXRF();
	void initXRF();
	void deinitXRF();
	void initXRF( bool enableChannel );
	bool filter_SimilarityMap();
	void magicLensToggled( bool isOn );
	void ioFinished();

protected:
	void updateSlicerXRFOpacity();
	QObject* recalculateXRF();
	void initSlicerXRF( bool enableChannel );

protected:
	dlg_periodicTable * dlgPeriodicTable;
	dlg_RefSpectra* dlgRefSpectra;
	dlg_SimilarityMap * dlgSimilarityMap;
	dlg_XRF * dlgXRF;
	iAIO * ioThread;
	vtkSmartPointer<vtkPiecewiseFunction> m_otf;

	iASlicer * slicerXZ;
	iASlicer * slicerXY;
	iASlicer * slicerYZ;
};

#endif // iAXRFAttachment_h__
