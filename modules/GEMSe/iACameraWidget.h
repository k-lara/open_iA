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
 
#ifndef IA_CAMERA_WIDGET_H
#define IA_CAMERA_WIDGET_H

#include "iASlicerMode.h"

#include <QWidget>

#include <vtkSmartPointer.h>

class vtkCamera;
class vtkImageData;
class iAImagePreviewWidget;

class QScrollBar;

class iACameraWidget: public QWidget
{
	Q_OBJECT
public:
	enum CameraLayout
	{
		ListLayout,
		GridLayout
	};
	iACameraWidget(QWidget* parent, vtkSmartPointer<vtkImageData>, int labelCount, CameraLayout layout);
	vtkCamera* GetCommonCamera();
	void UpdateView();
	void ShowImage(vtkSmartPointer<vtkImageData> imgData);
signals:
	void ModeChanged(iASlicerMode newMode, int sliceNr);
	void SliceChanged(int sliceNr);
	void ViewUpdated();
private:
	void UpdateScrollBar(int sliceNumber);
	static const int SLICE_VIEW_COUNT = 3;
	iAImagePreviewWidget* m_mainView;
	iAImagePreviewWidget* m_sliceViews[SLICE_VIEW_COUNT];
	vtkCamera*            m_commonCamera;
	QScrollBar*           m_sliceScrollBar;
	iASlicerMode          m_slicerMode;
private slots:
	void MiniSlicerClicked();
	void MiniSlicerUpdated();
	void ScrollBarChanged(int);
};

#endif // IA_CAMERA_WIDGET_H