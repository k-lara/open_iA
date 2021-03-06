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
 
#ifndef iAFuzzyFeatureTrackingAttachment_h__
#define iAFuzzyFeatureTrackingAttachment_h__

#include "iAModuleInterface.h"
#include "iAModuleAttachmentToChild.h"

class MdiChild;
class dlg_trackingGraph;
class dlg_dataView4DCT;
class dlg_trackingGraph;
class dlg_eventExplorer;
class iAVolumeStack;

class iAFuzzyFeatureTrackingAttachment : public iAModuleAttachmentToChild
{
	Q_OBJECT

public:
	iAFuzzyFeatureTrackingAttachment( MainWindow * mainWnd, iAChildData childData );
	~iAFuzzyFeatureTrackingAttachment();

protected:
	bool create4DCTDataViewWidget();
	bool create4DCTTrackingGraphWidget();
	bool create4DCTEventExplorerWidget();

protected slots:
	void updateViews();
protected:
	dlg_trackingGraph * trackingGraph;
	dlg_dataView4DCT * m_dlgDataView4DCT;
	dlg_trackingGraph * m_dlgTrackingGraph;
	dlg_eventExplorer * m_dlgEventExplorer;
	iAVolumeStack * m_volumeStack;
};

#endif // iAFuzzyFeatureTrackingAttachment_h__
