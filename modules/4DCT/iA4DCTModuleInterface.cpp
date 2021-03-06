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
#include "iA4DCTModuleInterface.h"
// iA
#include "dlg_4dctRegistration.h"
#include "dlg_commoninput.h"
#include "dlg_densityMap.h"
#include "iA4DCTAttachment.h"
#include "iA4DCTMainWin.h"
#include "iA4DCTSettings.h"
#include "iAConnector.h"
#include "iAConsole.h"
//#include "iAConvolutionFilter.h"
//#include "iAFilterLabelImageFilter.h"
#include "iASlicer.h"
#include "iASlicerWidget.h"
#include "mainwindow.h"
#include "mdichild.h"
// vtk
#include <vtkMath.h>
// itk
#include <itkConvolutionImageFilter.h>
#include <itkEllipseSpatialObject.h>
#include <itkImageFileWriter.h>
#include <itkImageKernelOperator.h>
#include <itkImageToVTKImageFilter.h>
#include <itkLabelGeometryImageFilter.h>
#include <itkNormalizedCorrelationImageFilter.h>
#include <itkSpatialObjectToImageFilter.h>
#include <itkSubtractImageFilter.h>
#include <itkVTKImageToImageFilter.h>
// Qt
#include <QColor>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QMdiSubWindow>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QObject>
#include <QSettings>
// std
#include <limits>

#define RAD_TO_DEG 57.295779513082320876798154814105

iA4DCTModuleInterface::iA4DCTModuleInterface()
{ /* not implemented */ }

iA4DCTModuleInterface::~iA4DCTModuleInterface()
{ /* not implemented */ }

void iA4DCTModuleInterface::Initialize()
{
	QMenu* toolsMenu = m_mainWnd->getToolsMenu();

	// ToFix: the menu should be added through the standard way of adding modules menus.
	// But a new menu won't be enabled by the default till a mdichild is created or opened.
	// This hack allows to be the menu enabled by the default.
	QMenu* menu4DCT = new QMenu(toolsMenu);
	menu4DCT->setTitle(tr("4DCT"));
	toolsMenu->addMenu(menu4DCT);

	QAction * newProj = new QAction(m_mainWnd);
	newProj->setText(QApplication::translate("MainWindows", "New 4DCT project", 0));
	newProj->setShortcut(QKeySequence(Qt::ALT + Qt::Key_4, Qt::Key_N));
	connect(newProj, SIGNAL(triggered()), this, SLOT(newProj()));
	menu4DCT->addAction(newProj);

	QAction * openProj = new QAction(m_mainWnd);
	openProj->setText(QApplication::translate("MainWindows", "Open 4DCT project", 0));
	openProj->setShortcut(QKeySequence(Qt::ALT + Qt::Key_4, Qt::Key_O));
	connect(openProj, SIGNAL(triggered()), this, SLOT(openProj()));
	menu4DCT->addAction(openProj);

	QAction* saveProj = new QAction(m_mainWnd);
	saveProj->setText(QApplication::translate("MainWindows", "Save 4DCT project", 0));
	saveProj->setShortcut(QKeySequence(Qt::ALT + Qt::Key_4, Qt::Key_S));
	connect(saveProj, SIGNAL(triggered()), this, SLOT(saveProj()));
	menu4DCT->addAction(saveProj);
}

iAModuleAttachmentToChild * iA4DCTModuleInterface::CreateAttachment(MainWindow* mainWnd, iAChildData childData)
{
	return new iA4DCTAttachment(mainWnd, childData);
}

/*============

	Slots

============*/

void iA4DCTModuleInterface::openProj()
{
	QSettings settings;
	QString fileName = QFileDialog::getOpenFileName(
		m_mainWnd,
		tr("Open 4DCT proj"),
		settings.value(S_4DCT_OPEN_DIR).toString(), 
		tr("4DCT project (*.xml)") );

	QFileInfo file(fileName);
	if (!file.exists()) {
		return;
	}
	settings.setValue(S_4DCT_OPEN_DIR, file.absolutePath());

	iA4DCTMainWin* sv = new iA4DCTMainWin(m_mainWnd);
	sv->load(fileName);
	m_mainWnd->mdiArea->addSubWindow(sv);
	sv->show();
}

void iA4DCTModuleInterface::newProj()
{
	iA4DCTMainWin* sv = new iA4DCTMainWin(m_mainWnd);
	m_mainWnd->mdiArea->addSubWindow(sv);
	sv->show();
}

void iA4DCTModuleInterface::saveProj()
{
	QMdiSubWindow* subWnd = m_mainWnd->mdiArea->currentSubWindow();
	iA4DCTMainWin* stackView = qobject_cast<iA4DCTMainWin*>(subWnd->widget());
	if (stackView != NULL) {
		stackView->save();
	}
}

//void iA4DCTModuleInterface::enableDensityMap()
//{
//	PrepareActiveChild();
//	/*m_densityMap = new dlg_densityMap(m_mainWnd, m_mdiChild);
//	m_mdiChild->tabifyDockWidget(m_childData.logs, m_densityMap);*/
//
//	dlg_4dctRegistration* reg = new dlg_4dctRegistration();
//	m_mainWnd->addSubWindow(reg);
//	reg->show();
//
//	QList<QMdiSubWindow*> list = m_mainWnd->MdiChildList();
//	foreach(QMdiSubWindow* window, m_mainWnd->MdiChildList())
//	{
//		MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
//		mdiChild->getSlicerXY()->widget()->set4DCTRegistration(reg);
//		mdiChild->getSlicerXZ()->widget()->set4DCTRegistration(reg);
//		mdiChild->getSlicerYZ()->widget()->set4DCTRegistration(reg);
//	}
//}
