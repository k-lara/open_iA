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
 
#ifndef IA_MULTI_CHANNEL_SEGMENTATION_MODULE_H
#define IA_MULTI_CHANNEL_SEGMENTATION_MODULE_H

#include "iAModuleInterface.h"

class iAGEMSeToolbar;
class iASEAFile;

class iAGEMSeModuleInterface : public iAModuleInterface
{
	Q_OBJECT

public:
	iAGEMSeModuleInterface();
	void Initialize();
protected:
	virtual iAModuleAttachmentToChild* CreateAttachment(MainWindow* mainWnd, iAChildData childData);
private slots:
	//! @{ Menu entries:
	bool StartGEMSe();
	void LoadPreCalculatedData();
	//! @}
	//! @{ Toolbar actions:
	void ResetFilter();
	void ToggleAutoShrink();
	void ToggleDockWidgetTitleBar();
	void ExportClusterIDs();
	void ExportAttributeRangeRanking();
	void ExportRankings();
	void ImportRankings();
	//! @}
private:
	void LoadPreCalculatedData(iASEAFile const & seaFile);
	void SetupToolbar();
	
	iAGEMSeToolbar* m_toolbar;
};

#endif // IA_MULTI_CHANNEL_SEGMENTATION_MODULE_H
