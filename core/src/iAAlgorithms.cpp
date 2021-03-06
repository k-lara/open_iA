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
#include "iAAlgorithms.h"

#include "iAConnector.h"
#include "iALogger.h"
#include "iAProgress.h"

#include <itkTriangleCell.h>

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <QMessageBox>

iAAlgorithms::iAAlgorithms( QString fn, FilterID fid, vtkImageData* idata, vtkPolyData* p, iALogger * logger, QObject *parent )
	: QThread( parent )
{
	m_elapsed = 0;
	m_isRunning = false;
	m_filterName = fn;
	m_filterID = fid;
	m_image = idata;
	m_polyData = p;
	m_logger = logger;
	for (int i = 0; i < 2; ++i)
		m_connectors.push_back(new iAConnector());
	m_itkProgress = new iAProgress;

	connect(parent, SIGNAL( rendererDeactivated(int) ), this, SLOT( updateVtkImageData(int) ));
	connect(m_itkProgress, SIGNAL( pprogress(int) ), this, SIGNAL( aprogress(int) ));
}

iAAlgorithms::iAAlgorithms( vtkImageData* idata, vtkPolyData* p, iALogger* logger, QObject *parent )
: QThread( parent )
{
	m_image = idata;
	m_polyData = p;
	m_logger = logger;
	for (int i = 0; i < 2; ++i)
		m_connectors.push_back(new iAConnector());
	m_itkProgress = new iAProgress;
}


iAAlgorithms::~iAAlgorithms()
{
	foreach(iAConnector* c, m_connectors)
		delete c;
	m_connectors.clear();
	delete m_itkProgress;
}


void iAAlgorithms::run()
{
	addMsg(tr("  unknown filter type"));
}


void iAAlgorithms::setImageData(vtkImageData* imgData)
{
	m_image = imgData;
}

QDateTime iAAlgorithms::Start()
{
	m_elapsed = 0; 
	m_time.start();
	m_isRunning = true;
	return QDateTime::currentDateTime();
}


int iAAlgorithms::Stop()
{
	if (m_isRunning) 
	{	
		m_isRunning = false;
		m_elapsed = m_time.elapsed();
	}
	return m_elapsed;
}


void iAAlgorithms::setup(QString fn, FilterID fid, vtkImageData* i, vtkPolyData* p, iALogger * l)
{
	m_filterName = fn; 
	m_filterID = fid; 
	m_image = i; 
	m_polyData = p; 
	m_logger = l;
}

void iAAlgorithms::addMsg(QString txt)
{
	if (m_logger)
	{
		m_logger->log(txt);
	}
}


iALogger* iAAlgorithms::getLogger() const
{
	return m_logger;
}

QString iAAlgorithms::getFilterName() const
{
	return m_filterName;
}

vtkImageData* iAAlgorithms::getVtkImageData()
{
	return m_image;
}

vtkPolyData* iAAlgorithms::getVtkPolyData()
{
	return m_polyData;
}

iAConnector *iAAlgorithms::getConnector(int c)
{
	while (m_connectors.size() <= c)
		m_connectors.push_back(new iAConnector());
	return m_connectors[c];
}


iAConnector* iAAlgorithms::getConnector() const
{
	return m_connectors[0];
}

iAConnector *const * iAAlgorithms::getConnectorArray() const
{
	return m_connectors.data();
}

iAConnector ** iAAlgorithms::getConnectorArray()
{
	return m_connectors.data();
}

iAConnector* iAAlgorithms::getFixedConnector() const
{
	return m_connectors[1];
}

bool iAAlgorithms::deleteConnector(iAConnector* c)
{
	bool isDeleted = false;
	int ind = m_connectors.indexOf(c);
	if (ind >= 0)		
	{
		m_connectors.remove(ind);
		isDeleted = true;
	}
	delete c;
	return isDeleted;
}

void iAAlgorithms::allocConnectors(int size)
{
	while (m_connectors.size() < size)
		m_connectors.push_back(new iAConnector());
}


iAProgress* iAAlgorithms::getItkProgress()
{
	return m_itkProgress;
}

void iAAlgorithms::updateVtkImageData(int ch)
{
	m_image->ReleaseData();
	m_image->Initialize();
	m_image->DeepCopy(m_connectors[ch]->GetVTKImage());
	m_image->CopyInformationFromPipeline(m_connectors[ch]->GetVTKImage()->GetInformation());
	m_image->Modified();
}


void iAAlgorithms::itkMesh_vtkPolydata( MeshType::Pointer mesh, vtkPolyData* polyData )
{
	int numPoints =  mesh->GetNumberOfPoints();

	typedef MeshType::CellsContainerPointer	CellsContainerPointer;
	typedef MeshType::CellsContainerIterator CellsContainerIterator;
	typedef MeshType::CellType CellType; 
	typedef MeshType::PointsContainer MeshPointsContainer;
	typedef MeshType::PointType MeshPointType;
	typedef MeshPointsContainer::Pointer InputPointsContainerPointer;
	typedef MeshPointsContainer::Iterator InputPointsContainerIterator;
	InputPointsContainerPointer myPoints = mesh->GetPoints();
	InputPointsContainerIterator points = myPoints->Begin();
	MeshPointType point;

	vtkPoints  * pvtkPoints = vtkPoints::New();
	vtkCellArray * pvtkPolys = vtkCellArray::New();

	if (numPoints == 0)
		return; 

	pvtkPoints->SetNumberOfPoints(numPoints);

	int idx=0;
	double vpoint[3];
	while( points != myPoints->End() ) 	
	{   
		point = points.Value();
		vpoint[0]= point[0];
		vpoint[1]= point[1];
		vpoint[2]= point[2];
		pvtkPoints->SetPoint(idx++,vpoint);
		points++;
	}

	polyData->SetPoints(pvtkPoints);
	pvtkPoints->Delete();

	CellsContainerPointer cells = mesh->GetCells();
	CellsContainerIterator cellIt = cells->Begin();
	vtkIdType pts[3];

	while ( cellIt != cells->End() )
	{
		CellType *nextCell = cellIt->Value();
		CellType::PointIdIterator pointIt = nextCell->PointIdsBegin() ;
		MeshPointType  p;
		int i;

		switch (nextCell->GetType()) 
		{
		case CellType::VERTEX_CELL:
		case CellType::LINE_CELL:
		case CellType::POLYGON_CELL:
			break;        
		case CellType::TRIANGLE_CELL:
			i=0;
			while (pointIt != nextCell->PointIdsEnd() ) {
				pts[i++] = *pointIt++;  
			}
			pvtkPolys->InsertNextCell(3,pts);
			break;
		default:
			printf("something \n");
		}
		cellIt++;
	}
	polyData->SetPolys(pvtkPolys);
	pvtkPolys->Delete();
}


int iAAlgorithms::getFilterID() const
{
	return m_filterID;
}

void iAAlgorithms::vtkPolydata_itkMesh(vtkPolyData* polyData, MeshType::Pointer mesh)
{
	// Transfer the points from the vtkPolyData into the itk::Mesh
	const unsigned long numberOfPoints = polyData->GetNumberOfPoints();
	vtkPoints * vtkpoints = polyData->GetPoints();

	mesh->GetPoints()->Reserve( numberOfPoints );

	for( unsigned long p = 0; p < numberOfPoints; p++ )
	{
		double * apoint = vtkpoints->GetPoint( p );
		MeshType::PointType point = MeshType::PointType( apoint );
		mesh->SetPoint( p, point);
		mesh->SetPointData( p, 1);
	}

	// Transfer the cells from the vtkPolyData into the itk::Mesh
	vtkCellArray * triangleStrips = polyData->GetStrips();
	vtkIdType  * cellPoints;
	vtkIdType    numberOfCellPoints;

	// First count the total number of triangles from all the triangle strips.
	unsigned long numberOfTriangles = 0;
	triangleStrips->InitTraversal();
	while( triangleStrips->GetNextCell( numberOfCellPoints, cellPoints ) )
		numberOfTriangles += numberOfCellPoints-2;

	vtkCellArray * polygons = polyData->GetPolys();
	polygons->InitTraversal();
	while( polygons->GetNextCell( numberOfCellPoints, cellPoints ) )
		if( numberOfCellPoints == 3 )
			numberOfTriangles ++;

	// Reserve memory in the itk::Mesh for all those triangles
	mesh->GetCells()->Reserve( numberOfTriangles );

	// Copy the triangles from vtkPolyData into the itk::Mesh
	typedef MeshType::CellType   CellType;
	typedef itk::TriangleCell< CellType > TriangleCellType;

	int cellId = 0;

	// first copy the triangle strips
	triangleStrips->InitTraversal();
	while( triangleStrips->GetNextCell( numberOfCellPoints, cellPoints ) )
	{
		unsigned long numberOfTrianglesInStrip = numberOfCellPoints - 2;
		unsigned long pointIds[3];

		pointIds[0] = cellPoints[0];
		pointIds[1] = cellPoints[1];
		pointIds[2] = cellPoints[2];

		for( unsigned long t=0; t < numberOfTrianglesInStrip; t++ )
		{
			MeshType::CellAutoPointer c;
			TriangleCellType * tcell = new TriangleCellType;
			tcell->SetPointIds( (TriangleCellType::PointIdConstIterator)pointIds );
			c.TakeOwnership( tcell );
			mesh->SetCell( cellId, c );
			cellId++;
			pointIds[0] = pointIds[1];
			pointIds[1] = pointIds[2];
			pointIds[2] = cellPoints[t+3];
		}
	}

	// then copy the normal triangles
	polygons->InitTraversal();
	while( polygons->GetNextCell( numberOfCellPoints, cellPoints ) )
	{
		if( numberOfCellPoints !=3 ) // skip any non-triangle.
		{
			continue;
		}
		MeshType::CellAutoPointer c;
		TriangleCellType * t = new TriangleCellType;
		t->SetPointIds( (TriangleCellType::PointIdConstIterator)cellPoints );
		c.TakeOwnership( t );
		mesh->SetCell( cellId, c );
		cellId++;
	}

	std::cout << "Mesh  " << std::endl;
	std::cout << "Number of Points =   " << mesh->GetNumberOfPoints() << std::endl;
	std::cout << "Number of Cells  =   " << mesh->GetNumberOfCells()  << std::endl;
}


void iAAlgorithms::SafeTerminate()
{
	if(isRunning())
	{
		terminate();
	}
}
