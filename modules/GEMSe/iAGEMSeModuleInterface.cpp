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
#include "iAGEMSeModuleInterface.h"

#include "dlg_commoninput.h"
#include "iAConsole.h"
#include "iAFileUtils.h"
#include "iAGEMSeAttachment.h"
#include "iASEAFile.h"
#include "iAModuleDispatcher.h"

// cross library boundaries!
#include "dlg_modalities.h"
#include "iAModality.h"
#include "iAModalityExplorerModuleInterface.h"
#include "iAModalityExplorerAttachment.h"

#include "mainwindow.h"
#include "mdichild.h"

#include <QFileDialog>
#include <QList>
#include <QSettings>
#include <QTextDocument>

#include <cassert>


iAGEMSeModuleInterface::iAGEMSeModuleInterface():
	m_toolbar(0)
{}

void iAGEMSeModuleInterface::Initialize()
{
	QMenu * toolsMenu = m_mainWnd->getToolsMenu();
	QMenu * menuMultiChannelSegm = getMenuWithTitle( toolsMenu, QString( "GEMSe" ), false );
	
	QAction * actionMetricVis = new QAction( m_mainWnd );
	actionMetricVis->setText( QApplication::translate( "MainWindow", "GEMSe", 0 ) );
	AddActionToMenuAlphabeticallySorted(menuMultiChannelSegm, actionMetricVis, true);
	connect(actionMetricVis, SIGNAL(triggered()), this, SLOT(StartGEMSe()));

	QAction * actionPreCalculated = new QAction( m_mainWnd );
	actionPreCalculated->setText( QApplication::translate( "MainWindow", "Load Pre-Calculated Results", 0 ));
	AddActionToMenuAlphabeticallySorted(menuMultiChannelSegm, actionPreCalculated, false);
	connect(actionPreCalculated, SIGNAL(triggered()), this, SLOT(LoadPreCalculatedData()));
}

namespace
{
	iAModalityExplorerModuleInterface* GetModalityExplorer(MainWindow* mainWnd, iAModuleDispatcher* dispatcher)
	{
		// TODO: find better solution than to duplicate ModalityExplorer here
		//       maybe include ModalityExplorer in core?
		static iAModalityExplorerModuleInterface* result(0);
		if (!result)
		{
			result = new iAModalityExplorerModuleInterface();
			result->SetMainWindow(mainWnd);
			result->SetDispatcher(dispatcher);
		}
		assert (result);
		return result;
	}
}

bool iAGEMSeModuleInterface::StartGEMSe()
{
	PrepareActiveChild();
	if (!m_mdiChild)
	{
		return false;
	}
	bool result = AttachToMdiChild( m_mdiChild );
	
	iAModalityExplorerAttachment* modalityAttachment = GetModalityExplorer(m_mainWnd, m_dispatcher)->GetAttachment(m_mdiChild);
	if (!modalityAttachment->GetModalitiesDlg()->GetModalities())
	{
		QSharedPointer<iAModalityList> modList(new iAModalityList);
		modList->Add(QSharedPointer<iAModality>(new iAModality("Modality 1", m_mdiChild->currentFile(), m_mdiChild->getImagePointer(), 0)));
		modalityAttachment->SetModalities(modList);
	}
	return result;
}

iAModuleAttachmentToChild* iAGEMSeModuleInterface::CreateAttachment(MainWindow* mainWnd, iAChildData childData)
{
	iAGEMSeAttachment* result = iAGEMSeAttachment::create( mainWnd, childData, GetModalityExplorer(mainWnd, m_dispatcher)->GetAttachment(m_mdiChild));
	if (result)
	{
		SetupToolbar();
	}
	return result;
}

void iAGEMSeModuleInterface::LoadPreCalculatedData()
{
	QString fileName = QFileDialog::getOpenFileName(m_mainWnd,
		tr("Load Precalculated Sampling & Clustering Data"),
		QString() // TODO get directory of current file
		,
		tr("Precalculated Segmentation Explorer Analysis file (*.sea);;" ) );
	if (fileName != "")
	{
		iASEAFile seaFile(fileName);
		LoadPreCalculatedData(seaFile);
	}
}

void iAGEMSeModuleInterface::LoadPreCalculatedData(iASEAFile const & seaFile)
{
	MdiChild *child = m_mainWnd->createMdiChild();

	QSharedPointer<iAModalityList> modList(new iAModalityList);

	if (!seaFile.good())
	{
		DEBUG_LOG("Given precalculated data file could not be read.\n");
		return;
	}
	modList->Load(seaFile.GetModalityFileName(), 0);
	if (modList->size() == 0)
	{
		DEBUG_LOG("You need to specify at least one modality!\n");
		return;
	}

	child->setImageData(modList->Get(0)->GetFileName(), modList->Get(0)->GetImage());
	child->show();
	child->showMaximized();
	m_mdiChild = child;
	UpdateChildData();
	
	m_mdiChild->waitForPreviousIO();

	// load segmentation explorer:
	bool result = AttachToMdiChild( m_mdiChild );
	assert(result);
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}

	iAModalityExplorerAttachment* modalityExplorerAttachment = GetModalityExplorer(m_mainWnd, m_dispatcher)->GetAttachment(m_mdiChild);
	modalityExplorerAttachment->SetModalities(modList);
	// load seeds/labels:
	if (!gemseAttach->LoadSeeds(seaFile.GetSeedsFileName()) ||
	// load sampling data:
		!gemseAttach->LoadSampling(seaFile.GetSamplingFileName()) ||
	// load cluster result:
		!gemseAttach->LoadClustering(seaFile.GetClusteringFileName()))
	{
		DEBUG_LOG("Precomputed Data Loading failed!\n");
	}

	if (seaFile.GetLayoutName() != "")
	{
		m_mainWnd->loadLayout(child, seaFile.GetLayoutName());
	}
}

#include <QToolBar>

#include "ui_GEMSeToolBar.h"
#include "iAQTtoUIConnector.h"

class iAGEMSeToolbar : public QToolBar, public Ui_GEMSeToolBar
{
public:
	iAGEMSeToolbar(QWidget* parent) : QToolBar("GEMSe ToolBar", parent)
	{
		this->setupUi(this);
	}
};

void iAGEMSeModuleInterface::SetupToolbar()
{
	if (m_toolbar)
	{
		return;
	}
	m_toolbar = new iAGEMSeToolbar(m_mainWnd);
	m_mainWnd->addToolBar(Qt::BottomToolBarArea, m_toolbar);

	connect(m_toolbar->action_ResetFilter, SIGNAL(triggered()), this, SLOT(ResetFilter()));
	connect(m_toolbar->action_ToggleAutoShrink, SIGNAL(triggered()), this, SLOT(ToggleAutoShrink()));
	connect(m_toolbar->action_ToggleTitleBar, SIGNAL(triggered()), this, SLOT(ToggleDockWidgetTitleBar()));
	connect(m_toolbar->action_ExportIDs, SIGNAL(triggered()), this, SLOT(ExportClusterIDs()));
	connect(m_toolbar->action_ExportAttributeRangeRanking, SIGNAL(triggered()), this, SLOT(ExportAttributeRangeRanking()));
	connect(m_toolbar->action_ExportRanking, SIGNAL(triggered()), this, SLOT(ExportRankings()));
	connect(m_toolbar->action_ImportRanking, SIGNAL(triggered()), this, SLOT(ImportRankings()));
}

void iAGEMSeModuleInterface::ResetFilter()
{
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}
	gemseAttach->ResetFilter();
}

void iAGEMSeModuleInterface::ToggleAutoShrink()
{
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}
	gemseAttach->ToggleAutoShrink();
}

void iAGEMSeModuleInterface::ToggleDockWidgetTitleBar()
{
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}
	gemseAttach->ToggleDockWidgetTitleBar();
}

void iAGEMSeModuleInterface::ExportClusterIDs()
{
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}
	gemseAttach->ExportClusterIDs();
}

void iAGEMSeModuleInterface::ExportAttributeRangeRanking()
{
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}
	gemseAttach->ExportAttributeRangeRanking();
}


void iAGEMSeModuleInterface::ExportRankings()
{
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}
	gemseAttach->ExportRankings();
}


void iAGEMSeModuleInterface::ImportRankings()
{
	iAGEMSeAttachment* gemseAttach = GetAttachment<iAGEMSeAttachment>();
	if (!gemseAttach)
	{
		DEBUG_LOG("GEMSE module is not attached!");
		return;
	}
	gemseAttach->ImportRankings();
}
