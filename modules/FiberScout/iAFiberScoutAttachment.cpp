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
#include "iAFiberScoutAttachment.h"

#include "dlg_FiberScout.h"
#include "iABlobCluster.h"

#include "iARenderer.h"
#include "mainwindow.h"

#include <vtkOpenGLRenderer.h>

iAFiberScoutAttachment::iAFiberScoutAttachment(MainWindow* mainWnd, iAChildData childData) :
	iAModuleAttachmentToChild(mainWnd, childData)
{
	blobRen = vtkOpenGLRenderer::New();
	blobVisEnabled = false;
}

iAFiberScoutAttachment::~iAFiberScoutAttachment()
{
	blobRen->Delete();
}

void iAFiberScoutAttachment::init(int filterID)
{
	imgFS = new dlg_FiberScout(m_childData.child, static_cast<FilterID>(filterID), blobRen);
	connect(imgFS, SIGNAL(updateViews()), m_childData.child, SLOT(updateViews()));

	blobRen->SetLayer(1);
	blobRen->UseDepthPeelingOn();
	blobRen->SetMaximumNumberOfPeels(12);

	m_childData.child->getRaycaster()->AddRenderer(blobRen);
	blobRen->SetActiveCamera(m_childData.child->getRaycaster()->getCamera());
	connect(m_childData.child->getRaycaster(), SIGNAL(onSetCamera()), this, SLOT(rendererSetCamera()));
}

void iAFiberScoutAttachment::disableBlobVisualization()
{
	// we can't disable blob vis if it is already disabled
	if (!blobVisEnabled) return;
	blobVisEnabled = false;

	while (!blobList.isEmpty())
		delete blobList.takeFirst();
}

void iAFiberScoutAttachment::enableBlobVisualization()
{
	// we can't initialize blob vis twice
	if (blobVisEnabled) return;
	blobVisEnabled = true;
	vtkSmartPointer<vtkImageData> imageData = m_childData.child->getImagePointer();
	double size[3] = {
		imageData->GetBounds()[1] - imageData->GetBounds()[0],
		imageData->GetBounds()[3] - imageData->GetBounds()[2],
		imageData->GetBounds()[5] - imageData->GetBounds()[4]
	};
}


void iAFiberScoutAttachment::rendererSetCamera()
{
	blobRen->SetActiveCamera(m_childData.child->getRaycaster()->getCamera());
}

bool iAFiberScoutAttachment::FiberScout_Options(int idx)
{
	if (!imgFS)
		return false;
	return imgFS->changeFiberScout_Options(idx);
}
