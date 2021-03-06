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
#include "dlg_progress.h"

#include "iAPerformanceHelper.h"

#include <QDateTime>

dlg_progress::dlg_progress(QWidget *parentWidget,
	QSharedPointer<iADurationEstimator const> durationEstimator,
	QSharedPointer<iAAbortListener> abortListener,
	QString const & caption):
	dlg_progressUI(parentWidget),
	m_durationEstimator(durationEstimator),
	m_abortListener(abortListener)
{
	connect(pbAbort, SIGNAL(clicked()), this, SLOT(Abort()));
	setWindowTitle(caption);
}

void dlg_progress::SetProgress(int progress)
{
	progressBar->setValue(progress);


	if (m_durationEstimator->elapsed() > 0)
	{
		double estimatedRemaining = m_durationEstimator->estimatedTimeRemaining();
		lbElapsed->setText(QString("Elapsed: ")+
			formatDuration(m_durationEstimator->elapsed()));
		lbETR->setText(QString("Estimated Time Remaining: ")+
			((estimatedRemaining == -1) ? "unknown" : formatDuration(estimatedRemaining)));
	}
}

void dlg_progress::SetStatus(QString const & status)
{
	lbStatus->setText(QString("Status: ")+status);
}


void dlg_progress::Abort()
{
	m_abortListener->Abort();
}