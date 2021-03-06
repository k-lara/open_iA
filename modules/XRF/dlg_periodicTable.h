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
 
#ifndef DLG_PERIODICTABLE_H
#define DLG_PERIODICTABLE_H

#include "iAPeriodicTableWidget.h"

#include <QDockWidget>
#include "ui_PeriodicTable.h"
#include "iAQTtoUIConnector.h"
typedef iAQTtoUIConnector<QDockWidget, Ui_PeriodicTable> dlg_periodicTableContainer;

class iAElementSelectionListener;

class dlg_periodicTable : public dlg_periodicTableContainer
{
	Q_OBJECT
public:
	dlg_periodicTable(QWidget *parent): dlg_periodicTableContainer(parent)
	{
		m_periodicTableWidget = new iAPeriodicTableWidget(parent);
		m_periodicTableWidget->setObjectName(QString::fromUtf8("PeriodicTable"));
		setWidget(m_periodicTableWidget);
	}
	void setConcentration(QString const & elementName, double percentage, QColor const & color)
	{
		m_periodicTableWidget->setConcentration(elementName, percentage, color);
	}
	void setListener(QSharedPointer<iAElementSelectionListener> listener)
	{
		m_periodicTableWidget->setListener(listener);
	}
	int GetCurrentElement() const
	{
		return m_periodicTableWidget->GetCurrentElement();
	}
private:
	iAPeriodicTableWidget* m_periodicTableWidget;
};

#endif /* DLG_PERIODICTABLE_H */
