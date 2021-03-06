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
#include "dlg_imageproperty.h"

#include <vtkImageData.h>
#include <vtkImageAccumulate.h>

dlg_imageproperty::dlg_imageproperty(QWidget *parent, vtkImageData* src, vtkImageAccumulate* accum, QString Filename) : QDockWidget(parent)
{
	setupUi(this);
	enterMsg( tr ("  Filename: %1 ").arg(Filename));
	updateProperties(src, accum);
}

void dlg_imageproperty::enterMsg(QString txt)
{
	lWidget->addItem(txt); 
	lWidget->scrollToBottom();
}

void dlg_imageproperty::updateProperties(vtkImageData* src, vtkImageAccumulate* accum, bool cl )
{
	if (cl) lWidget->clear();
	enterMsg(tr("  Extent: [%1 %2]  [%3 %4]  [%5 %6]").arg(src->GetExtent()[0])
															.arg(src->GetExtent()[1])
															.arg(src->GetExtent()[2])
															.arg(src->GetExtent()[3])
															.arg(src->GetExtent()[4])
															.arg(src->GetExtent()[5]));

	enterMsg(tr("  Spacing:  %1  %2  %3").arg(src->GetSpacing()[0])
								   .arg(src->GetSpacing()[1])
								   .arg(src->GetSpacing()[2]));

	enterMsg(tr("  Origin: %1 %2 %3").arg(src->GetOrigin()[0])
								   .arg(src->GetOrigin()[1])
								   .arg(src->GetOrigin()[2]));

	QString txt = src->GetScalarTypeAsString();
	enterMsg("  Datatype: " + txt);

	enterMsg(tr("  VoxelCount: %1;  Min: %2;  Max: %3;  Mean: %4;  StdDev: %5;")
		.arg(accum->GetVoxelCount())
        .arg(*accum->GetMin())
		.arg(*accum->GetMax())
		.arg(*accum->GetMean())
		.arg(*accum->GetStandardDeviation()));
	this->show();
}
