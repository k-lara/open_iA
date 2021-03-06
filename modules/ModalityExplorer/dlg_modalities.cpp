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
#include "dlg_modalities.h"

#include "dlg_modalityProperties.h"
#include "dlg_modalityRenderer.h"
#include "dlg_planeSlicer.h"
#include "iAConsole.h"
#include "iAFast3DMagicLensWidget.h"
#include "iAModality.h"
#include "iAModalityDisplay.h"
#include "iAModalityTransfer.h"
#include "iARenderer.h"
#include "iARenderSettings.h"
#include "iASlicer.h"
#include "iASlicerData.h"

#include <QVTKInteractor.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <QFileDialog>
#include <QSettings>

#include <cassert>


dlg_modalities::dlg_modalities(iAFast3DMagicLensWidget* modalityRenderer,
	dlg_planeSlicer* planeSlicer) :
	//m_mdiChild(mdiChild),
	modalities(new iAModalityList),
	m_selectedRow(-1),
	renderer(modalityRenderer),
	m_planeSlicer(planeSlicer)
{
	connect(pbAdd,    SIGNAL(clicked()), this, SLOT(AddClicked()));
	connect(pbRemove, SIGNAL(clicked()), this, SLOT(RemoveClicked()));
	connect(pbEdit,   SIGNAL(clicked()), this, SLOT(EditClicked()));
	connect(pbStore,  SIGNAL(clicked()), this, SLOT(Store()));
	connect(pbLoad, SIGNAL(clicked()), this, SLOT(Load()));
	connect(cbManualRegistration, SIGNAL(clicked()), this, SLOT(ManualRegistration()));
	connect(cbShowMagicLens, SIGNAL(clicked()), this, SLOT(MagicLens()));
	connect(cbCuttingPlane, SIGNAL(clicked()), this, SLOT(CuttingPlane()) );
	
	connect(lwModalities, SIGNAL(itemClicked(QListWidgetItem*)),
		this, SLOT(ListClicked(QListWidgetItem*)));

	connect(modalityRenderer, SIGNAL(MouseMoved()), this, SLOT(RendererMouseMoved()));

	m_cuttingPlaneActor = vtkSmartPointer<vtkActor>::New();
	m_planeSource = vtkSmartPointer<vtkPlaneSource>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(m_planeSource->GetOutputPort());
	m_boundingBoxMin[0] = m_boundingBoxMin[1] = m_boundingBoxMin[2] = -0.1;
	m_boundingBoxMax[0] = m_boundingBoxMax[1] = m_boundingBoxMax[2] =  0.1;
	m_planeSource->SetPoint1(m_boundingBoxMin);
	m_planeSource->SetPoint1(m_boundingBoxMax);
	m_cuttingPlaneActor->SetMapper(mapper);
	m_cuttingPlaneActor->GetProperty()->SetInterpolationToFlat();
	m_cuttingPlaneActor->GetProperty()->LightingOff();

	int tableSize = 1;
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetNumberOfTableValues(tableSize);
	lut->Build();
	lut->SetTableValue(0, 0, 0, 0, 1);  //Black
	mapper->SetScalarRange(0, tableSize - 1);
	mapper->SetLookupTable(lut);

	QHBoxLayout* hbox = new QHBoxLayout();
	hbox->setSpacing(0);
	hbox->setMargin(2);
	histogramContainer->setLayout(hbox);
}

void dlg_modalities::SetModalities(QSharedPointer<iAModalityList> modList)
{
	modalities = modList;
	lwModalities->clear();
	connect(modalities.data(), SIGNAL(Added(QSharedPointer<iAModality>)), this, SLOT(ModalityAdded(QSharedPointer<iAModality>)));
	for (int i=0; i<modalities->size(); ++i)
	{
		ModalityAdded(modalities->Get(i));
	}
	m_selectedRow = 0;
}

void dlg_modalities::Store()
{
	QString modalitiesFileName = QFileDialog::getSaveFileName(
		QApplication::activeWindow(),
		tr("Select Output File"),
		QString(), // TODO get directory of current file
		tr("Segmentation Explorer Modalities file (*.mod);;All files (*.*)" ) );
	Store(modalitiesFileName);
}

void dlg_modalities::Store(QString const & filename)
{
	modalities->Store(filename, renderer->getMainRenderer()->GetActiveCamera());
}

void dlg_modalities::Load()
{
	QString modalitiesFileName = QFileDialog::getOpenFileName(
		QApplication::activeWindow(),
		tr("Open Input File"),
		QString(), // TODO get directory of current file
		tr("Segmentation Explorer Modalities file (*.mod);;All files (*.*)" ) );
	if (!modalitiesFileName.isEmpty() && Load(modalitiesFileName))
	{
		EnableButtons();
		emit ModalityAvailable();
	}
}

bool dlg_modalities::Load(QString const & filename)
{
	return modalities->Load(filename, renderer->getMainRenderer()->GetActiveCamera());
}

QString GetCaption(iAModality const & mod)
{
	QFileInfo fi(mod.GetFileName());
	return mod.GetName()+" ("+fi.fileName()+")";
}

void dlg_modalities::AddClicked()
{
	QSharedPointer<iAModality> newModality(new iAModality);
	dlg_modalityProperties prop(this, newModality);
	if (prop.exec() == QDialog::Rejected)
	{
		return;
	}
	modalities->Add(newModality);
}

void dlg_modalities::MagicLens()
{
	if (cbShowMagicLens->isChecked())
	{
		renderer->magicLensOn();
	}
	else
	{
		renderer->magicLensOff();
	}
}

void dlg_modalities::CuttingPlane()
{
	if (cbCuttingPlane->isChecked())
	{
		renderer->getMainRenderer()->AddActor(m_cuttingPlaneActor);
	}
	else
	{
		renderer->getMainRenderer()->RemoveActor(m_cuttingPlaneActor);
	}
}

void dlg_modalities::ModalityAdded(QSharedPointer<iAModality> mod)
{
	QListWidgetItem* listItem = new QListWidgetItem(GetCaption(*mod));
	lwModalities->addItem (listItem);
	EnableButtons();
	m_selectedRow = lwModalities->row( listItem );
	vtkSmartPointer<vtkImageData> imgData = mod->GetImage();
	QSharedPointer<ModalityTransfer> modTransfer(new ModalityTransfer(
		imgData,
		this,
		2048));
	mod->SetTransfer(modTransfer);
	modTransfer->ShowHistogram(histogramContainer);
	QSharedPointer<ModalityDisplay> modDisp(new ModalityDisplay(modTransfer, imgData));
	mod->SetDisplay(modDisp);
	if (mod->hasRenderFlag(iAModality::MainRenderer))
	{
		renderer->getMainRenderer()->AddVolume(modDisp->volume);
	}
	if (mod->hasRenderFlag(iAModality::MagicLens))
	{
		renderer->getLensRenderer()->AddVolume(modDisp->volume);
	}
	if (modalities->size() == 1)
	{
		renderer->getMainRenderer()->ResetCamera();
		renderer->getLensRenderer()->ResetCamera();
	}
	determineBoundingBox();

	m_planeSlicer->AddImage(imgData, modTransfer->getColorFunction(), 0.5);

	emit ModalityAvailable();
}

void dlg_modalities::RemoveClicked()
{
	int idx = lwModalities->currentRow();
	if (idx < 0 || idx >= modalities->size())
	{
		DEBUG_LOG(QString("Index out of range (%1)\n").arg(idx));
		return;
	}
	// TODO: refactor
	QSharedPointer<ModalityDisplay> modDisp = modalities->Get(idx)->GetDisplay();
	if (modalities->Get(idx)->hasRenderFlag(iAModality::MainRenderer))
	{
		renderer->getMainRenderer()->RemoveVolume(modDisp->volume);
	}
	if (modalities->Get(idx)->hasRenderFlag(iAModality::MagicLens))
	{
		renderer->getLensRenderer()->RemoveVolume(modDisp->volume);
	}
	modalities->Remove(idx);
	delete lwModalities->takeItem(idx);
	EnableButtons();
}

void dlg_modalities::EditClicked()
{
	int idx = lwModalities->currentRow();
	if (idx < 0 || idx >= modalities->size())
	{
		DEBUG_LOG(QString("Index out of range (%1)\n").arg(idx));
		return;
	}
	QString fnBefore = modalities->Get(idx)->GetFileName();
	int renderFlagsBefore = modalities->Get(idx)->RenderFlags();
	QSharedPointer<iAModality> editModality(modalities->Get(idx));
	dlg_modalityProperties prop(this, editModality);
	if (prop.exec() == QDialog::Rejected)
	{
		return;
	}
	if (fnBefore != editModality->GetFileName())
	{
		DEBUG_LOG("Changing file not supported!\n");
	}
	QSharedPointer<ModalityDisplay> modDisp = modalities->Get(idx)->GetDisplay();
	if ((renderFlagsBefore & iAModality::MainRenderer) == iAModality::MainRenderer
		&& !editModality->hasRenderFlag(iAModality::MainRenderer))
	{
		renderer->getMainRenderer()->RemoveVolume(modDisp->volume);
	}
	if ((renderFlagsBefore & iAModality::MainRenderer) == 0
		&& editModality->hasRenderFlag(iAModality::MainRenderer))
	{
		renderer->getMainRenderer()->AddVolume(modDisp->volume);
	}
	if ((renderFlagsBefore & iAModality::MagicLens) == iAModality::MagicLens
		&& !editModality->hasRenderFlag(iAModality::MagicLens))
	{
		renderer->getLensRenderer()->RemoveVolume(modDisp->volume);
	}
	if ((renderFlagsBefore & iAModality::MagicLens) == 0
		&& editModality->hasRenderFlag(iAModality::MagicLens))
	{
		renderer->getLensRenderer()->AddVolume(modDisp->volume);
	}
	lwModalities->item(idx)->setText(GetCaption(*editModality));
}

void dlg_modalities::EnableButtons()
{
	bool enable = modalities->size() > 0;
	pbEdit->setEnabled(enable);
	pbRemove->setEnabled(enable);
	pbStore->setEnabled(enable);
}

void dlg_modalities::ManualRegistration()
{
	if (cbManualRegistration->isChecked())
	{
		renderer->GetInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballActor::New());
	}
	else
	{
		renderer->GetInteractor()->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
	}
}

void dlg_modalities::ListClicked(QListWidgetItem* item)
{
	m_selectedRow = lwModalities->row( item );
	QSharedPointer<iAModality> currentData = modalities->Get(m_selectedRow);
	if (m_selectedRow >= 0)
	{
		QSharedPointer<ModalityTransfer> modTransfer = currentData->GetTransfer();
		modTransfer->ShowHistogram(histogramContainer);
	}
	emit ShowImage(currentData->GetImage());
}

QSharedPointer<iAModalityList const> dlg_modalities::GetModalities() const
{
	return modalities;
}

QSharedPointer<iAModalityList> dlg_modalities::GetModalities()
{
	return modalities;
}

int dlg_modalities::GetSelected() const
{
	return m_selectedRow;
}

vtkSmartPointer<vtkColorTransferFunction> dlg_modalities::GetCTF(int modality)
{
	return modalities->Get(modality)->GetTransfer()->getColorFunction();
}

vtkSmartPointer<vtkPiecewiseFunction> dlg_modalities::GetOTF(int modality)
{
	return modalities->Get(modality)->GetTransfer()->getOpacityFunction();
}

void dlg_modalities::ChangeRenderSettings(RenderSettings const & rs)
{
	for (int i = 0; i < modalities->size(); ++i)
	{
		QSharedPointer<ModalityDisplay> modDisp = modalities->Get(i)->GetDisplay();
		modDisp->SetRenderSettings(rs);
	}
	renderer->getMainRenderer()->SetBackground(rs.BackgroundColor.redF(), rs.BackgroundColor.greenF(), rs.BackgroundColor.blueF());
	renderer->getLensRenderer()->SetBackground(rs.BackgroundColor.redF(), rs.BackgroundColor.greenF(), rs.BackgroundColor.blueF());
}

void dlg_modalities::ChangeMagicLensSize(int size)
{
	renderer->setLensSize(size, size);
}

void dlg_modalities::determineBoundingBox()
{
	for (int i = 0; i < modalities->size(); ++i)
	{
		QSharedPointer<iAModality> m = modalities->Get(i);
		QSharedPointer<ModalityDisplay> disp = m->GetDisplay();
		if (!disp)
			continue;
		vtkSmartPointer<vtkVolume> vol = disp->volume;
		double * bounds = vol->GetBounds();
		if (!bounds)
		{
			DEBUG_LOG(QString("Modality %1: Bounds not defined!").arg(i));
			continue;
		}

		for (int i = 0; i < 3; ++i)
		{
			double min = bounds[i * 2];
			double max = bounds[1 + i * 2];
			if (min < m_boundingBoxMin[i])
			{
				m_boundingBoxMin[i] = min;
			}
			if (max > m_boundingBoxMax[i])
			{
				m_boundingBoxMax[i] = max;
			}
		}
	}
	m_planeSource->SetPoint1(m_boundingBoxMin);
	m_planeSource->SetPoint1(m_boundingBoxMax);
}

void dlg_modalities::RendererMouseMoved()
{
	double baseVector[3] = { 0, 0, 1 };
	double basePosition[3] = { -0.1, -0.1, -0.1 };

	m_planeSlicer->SetCuttingPlane(basePosition, baseVector);

	/*
	double* orientation = m_cuttingPlaneActor->GetOrientation(); // this is angles. we have to calculate a vector...
	double* position = m_cuttingPlaneActor->GetCenter();

	double * planePosition = m_planeSource->GetCenter();
	double * planeNormal = m_planeSource->GetNormal();


	m_planeSlicer->SetCuttingPlane(position, planeNormal);
	*/
}
