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
 
#ifndef IA4DCTVISWIN_H
#define IA4DCTVISWIN_H
// Ui
#include "ui_iA4DCTVisWin.h"
// iA
#include "iABoundingBoxVisModule.h"
#include "iAQTtoUIConnector.h"
#include "iAVisModulesCollection.h"
// vtk
#include <vtkSmartPointer.h>
// Qt
#include <QDockWidget>
#include <QMainWindow>
#include <QTimer>
#include <QSharedPointer>

class QString;
class vtkRenderer;
class iA4DCTAllVisualizationsDockWidget;
class iA4DCTCurrentVisualizationsDockWidget;
class iA4DCTFractureVisDockWidget;
class iA4DCTMainWin;
class iA4DCTPlaneDockWidget;
class iA4DCTRegionViewDockWidget;
class iA4DCTSettingsDockWidget;
class iA4DCTToolsDockWidget;
class iAVisModule;

const float SCENE_SCALE = 0.01;

class iA4DCTVisWin : public QMainWindow, public Ui::VisWin
{
	Q_OBJECT
public:
						iA4DCTVisWin( iA4DCTMainWin * parent = 0 );
						~iA4DCTVisWin( );
	void				setImageSize( double * size );
	void				setNumberOfStages( int number );

	bool				showDialog( QString & imagePath );

public slots:
	void				updateRenderWindow( );
	void				addedVisualization( );
	void				selectedVisModule( iAVisModule * visModule );
	void				updateVisualizations();


protected:
	void				setEnabledToolsDockWidgets( bool enabled );

	vtkRenderer *							m_mainRen;
	vtkRenderer *							m_magicLensRen;
	double									m_size[3];
	int										m_currentStage;
	QTimer									m_timer;
	iA4DCTMainWin *							m_mainWin;
	iAVisModulesCollection					m_visModules;

	// dock widgets (prefix: dw)
	iA4DCTFractureVisDockWidget *			m_dwFractureVis;
	iA4DCTPlaneDockWidget *					m_dwPlane;
	iA4DCTAllVisualizationsDockWidget *		m_dwAllVis;
	iA4DCTCurrentVisualizationsDockWidget *	m_dwCurrentVis;
	iA4DCTRegionViewDockWidget *			m_dwRegionVis;
	iA4DCTSettingsDockWidget *				m_dwSettings;
	iA4DCTToolsDockWidget *					m_dwTools;

	bool									m_isVirgin;


private slots:
	// GUI
	void				onStageSliderValueChanged( int val );
	void				onFirstButtonClicked( );
	void				onPreviousButtonClicked( );
	void				onNextButtonClicked( );
	void				onLastButtonClicked( );
	void				onPlayButtonClicked( bool checked );
	void				onIntervalValueChanged( int val );

	// camera
	void				resetCamera( );
	void				setXYView( );
	void				setXZView( );
	void				setYZView( );
	void				setXYBackView( );
	void				setXZBackView( );
	void				setYZBackView( );

	// fracture vis
	//void				onSaveButtonClicked( );
	void				onLoadButtonClicked( );
	void				onExtractButtonClicked( );
	// defect density maps
	void				addSurfaceVis( );
	void				calcDensityMap();
	// bounding box
	void				addBoundingBox( );
	// defect view
	void				addDefectView( );
	// magic lens
	void				enableMagicLens( bool enable );
};

#endif // IA4DCTVISWIN_H