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
#include "iAGeometricTransformationsModuleInterface.h"

#include "dlg_commoninput.h"
#include "iAGeometricTransformations.h"
#include "mainwindow.h"
#include "mdichild.h"

void iAGeometricTransformationsModuleInterface::Initialize()
{
	QMenu * filtersMenu = m_mainWnd->getFiltersMenu();
	QMenu * menuGeometric_Transformations = getMenuWithTitle(filtersMenu, QApplication::translate("MainWindow", "Geometric Transformations", 0));
	
	QAction * actionResampler = new QAction(QApplication::translate("MainWindow", "Resampler", 0), m_mainWnd );
	QAction * actionExtract_Image = new QAction(QApplication::translate("MainWindow", "Extract Image", 0), m_mainWnd);
	QAction * actionRescale_Image = new QAction(QApplication::translate("MainWindow", "Rescale Image", 0), m_mainWnd);
	
	menuGeometric_Transformations->addAction(actionResampler);
	menuGeometric_Transformations->addAction(actionExtract_Image);
	menuGeometric_Transformations->addAction(actionRescale_Image);

	connect( actionResampler, SIGNAL( triggered() ), this, SLOT( resampler() ) );
	connect( actionExtract_Image, SIGNAL( triggered() ), this, SLOT( extractImage() ) );
	connect(actionRescale_Image, SIGNAL(triggered()), this, SLOT(rescale()));
}

void iAGeometricTransformationsModuleInterface::resampler()
{
	//set parameters
	PrepareActiveChild();
	QStringList inList = (QStringList() << tr( "#OriginX" ) << tr( "#OriginY" ) << tr( "#OriginZ" )
		<< tr( "#SpacingX" ) << tr( "#SpacingY" ) << tr( "#SpacingZ" )
		<< tr( "#SizeX" ) << tr( "#SizeY" ) << tr( "#SizeZ" ));
	QList<QVariant> inPara; 	inPara << tr( "%1" ).arg( rOriginX ) << tr( "%1" ).arg( rOriginY ) << tr( "%1" ).arg( rOriginZ )
		<< tr( "%1" ).arg( m_childData.imgData->GetSpacing()[1] )
		<< tr( "%1" ).arg( m_childData.imgData->GetSpacing()[0] )
		<< tr( "%1" ).arg( m_childData.imgData->GetSpacing()[2] )
		<< tr( "%1" ).arg( m_childData.imgData->GetExtent()[1] )
		<< tr( "%1" ).arg( m_childData.imgData->GetExtent()[3] )
		<< tr( "%1" ).arg( m_childData.imgData->GetExtent()[5] );

	dlg_commoninput dlg( m_mainWnd, "Resampler", 9, inList, inPara, NULL );

	if( dlg.exec() != QDialog::Accepted )
		return;
	rOriginX = dlg.getValues()[0];
	rOriginY = dlg.getValues()[1];
	rOriginZ = dlg.getValues()[2];
	rSpacingX = dlg.getValues()[3];
	rSpacingY = dlg.getValues()[4];
	rSpacingZ = dlg.getValues()[5];
	rSizeX = dlg.getValues()[6];
	rSizeY = dlg.getValues()[7];
	rSizeZ = dlg.getValues()[8];

	//prepare
	QString filterName = tr( "Resampler" );
	PrepareResultChild( filterName );
	m_mdiChild->addStatusMsg( filterName );
	//execute
	rSizeX++; rSizeY++; rSizeZ++;
	iAGeometricTransformations* thread = new iAGeometricTransformations( filterName, RESAMPLER,
		m_childData.imgData, m_childData.polyData, m_mdiChild->getLogger(), m_mdiChild );
	m_mdiChild->connectThreadSignalsToChildSlots( thread );
	thread->setRParameters( rOriginX, rOriginY, rOriginZ, rSpacingX, rSpacingY, rSpacingZ, rSizeX, rSizeY, rSizeZ );
	thread->start();
	m_mainWnd->statusBar()->showMessage( filterName, 5000 );
}

void iAGeometricTransformationsModuleInterface::extractImage()
{
	//set parameters
	PrepareActiveChild();
	QStringList inList = (QStringList() << tr( "*IndexX" ) << tr( "*IndexY" ) << tr( "*IndexZ" )
		<< tr( "*SizeX" ) << tr( "*SizeY" ) << tr( "*SizeZ" ) );
	QList<QVariant> inPara; 	inPara << tr( "%1" ).arg( eiIndexX ) << tr( "%1" ).arg( eiIndexY ) << tr( "%1" ).arg( eiIndexZ )
		<< tr( "%1" ).arg( m_childData.imgData->GetExtent()[1] + 1 )
		<< tr( "%1" ).arg( m_childData.imgData->GetExtent()[3] + 1 )
		<< tr( "%1" ).arg( m_childData.imgData->GetExtent()[5] + 1 );

	dlg_commoninput dlg( m_mainWnd, "Extract Image", 6, inList, inPara, NULL );
	dlg.connectMdiChild( m_mdiChild );
	dlg.setModal( false );
	dlg.show();
	m_mdiChild->activate( MdiChild::cs_ROI );
	m_mdiChild->setROI( eiIndexX, eiIndexY, eiIndexZ,
		m_childData.imgData->GetExtent()[1], m_childData.imgData->GetExtent()[3], m_childData.imgData->GetExtent()[5] );
	m_mdiChild->showROI();
	MdiChild* origChild = m_mdiChild;
	if( dlg.exec() != QDialog::Accepted )
		return;
	eiIndexX = dlg.getSpinBoxValues()[0];
	eiIndexY = dlg.getSpinBoxValues()[1];
	eiIndexZ = dlg.getSpinBoxValues()[2];
	eiSizeX = dlg.getSpinBoxValues()[3];
	eiSizeY = dlg.getSpinBoxValues()[4];
	eiSizeZ = dlg.getSpinBoxValues()[5];
	//prepare
	QString filterName = tr( "Extract Image" );
	PrepareResultChild( filterName );
	m_mdiChild->addStatusMsg( filterName );
	//execute
	m_mdiChild->setUpdateSliceIndicator( true );
	iAGeometricTransformations* thread = new iAGeometricTransformations( filterName, EXTRACT_IMAGE,
		m_childData.imgData, m_childData.polyData, m_mdiChild->getLogger(), m_mdiChild );
	m_mdiChild->connectThreadSignalsToChildSlots( thread );
	thread->setEParameters( eiIndexX, eiIndexY, eiIndexZ, eiSizeX, eiSizeY, eiSizeZ );
	thread->start();
	m_mainWnd->statusBar()->showMessage( filterName, 5000 );
	//only access the child if the main window has not already been closed
	if( m_mainWnd->isVisible() )
	{
		origChild->hideROI();
		origChild->deactivate();
	}
}



void iAGeometricTransformationsModuleInterface::rescale()
{
	//set parameters
	PrepareActiveChild();

	QStringList inList = (QStringList() << tr("#Output Minimum") << tr("#Output Maximum") );


	QList<QVariant> inPara; 	
	inPara << tr("%1").arg(outputMin) << tr("%1").arg(outputMax);

	dlg_commoninput dlg(m_mainWnd, "Rescale Image", 2, inList, inPara, NULL);
	dlg.connectMdiChild(m_mdiChild);
	dlg.setModal(false);
	dlg.show();

	MdiChild* origChild = m_mdiChild;
	if (dlg.exec() != QDialog::Accepted)
		return;

	outputMin = dlg.getValues()[0]; 
	outputMax = dlg.getValues()[1];

	//prepare
	QString filterName = tr("Rescale Image");
	PrepareResultChild(filterName);
	m_mdiChild->addStatusMsg(filterName);
	//execute

	iAGeometricTransformations* thread = new iAGeometricTransformations(filterName, RESCALE_IMAGE,
		m_childData.imgData, m_childData.polyData, m_mdiChild->getLogger(), m_mdiChild);
	m_mdiChild->connectThreadSignalsToChildSlots(thread);

	thread->setRescaleParameters(outputMin, outputMax );
	thread->start();

	m_mainWnd->statusBar()->showMessage(filterName, 5000);

}
