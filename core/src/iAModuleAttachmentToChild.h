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
 
#ifndef iAModuleAttachmentToChild_h__
#define iAModuleAttachmentToChild_h__

#include <QObject>

#include "iAChildData.h"
#include "open_iA_Core_export.h"

class MainWindow;

class open_iA_Core_API iAModuleAttachmentToChild : public QObject
{
	Q_OBJECT

public:
	iAModuleAttachmentToChild( MainWindow * mainWnd, iAChildData childData ) : m_mainWnd(mainWnd), m_childData(childData) {}
	virtual ~iAModuleAttachmentToChild() {}
	MdiChild * GetMdiChild() const { return m_childData.child; }
Q_SIGNALS:
	void detach();
protected:
	MainWindow * m_mainWnd;
	iAChildData m_childData;
};

#endif // iAModuleAttachmentToChild_h__
