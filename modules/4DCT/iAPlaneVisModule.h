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
 
#ifndef IAPLANEVISMODULE_H
#define IAPLANEVISMODULE_H
// iA
#include "iAVisModule.h"
#include "iA4DCTDefects.h"
// vtk
#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
// Qt
#include <QString>
#include <QColor>
#include <QVector>

class vtkActor;
class vtkPlaneSource;
class vtkPolyDataMapper;
class vtkTexture;

struct iAPlaneVisSettings
{
	enum Direction { XY, XZ, YZ };
	Direction Dir;
	double Slice;
	double Opacity;
	bool Shading;
};

class iAPlaneVisModule : public iAVisModule
{
public:
				iAPlaneVisModule( );
	void		enable( );
	void		disable( );
	void		setSize( double * size );
	void		setImage( QString fileName );
	void		setSlice( double slice );
	void		setOpacity( double opacity );
	void		enableShading( );
	void		disableShading( );
	void		setDirXY( );
	void		setDirXZ( );
	void		setDirYZ( );

	template<typename T>
	void		highlightDefects( QVector<QString> defects, QVector<QColor> colors, QString labeledImgPath );
	template<typename T>
	void		densityMap( QString defect, QColor color, QString labeledImgPath, int * size );
	template<typename T>
	void		labledImageToMask(vtkImageData* img, iA4DCTDefects::VectorDataType list);

	iAPlaneVisSettings		settings;

private:
	void		setPlanePosition( double slice );

	vtkSmartPointer<vtkPlaneSource>			m_plane;
	vtkSmartPointer<vtkTexture>				m_texture;
	vtkSmartPointer<vtkPolyDataMapper>		m_mapper;
	vtkSmartPointer<vtkActor>				m_actor;
	vtkSmartPointer<vtkImageReslice>		m_reslice;
	vtkSmartPointer<vtkImageData>			m_img;

	//enum Direction { XY, XZ, YZ };
	//Direction		m_dir;
	double			m_size[3];
	double			m_imgSize[3];
	double			m_imgSpacing[3];
	double			m_axialElements[16];
};

//==============================================
//
//			Template methods
//
//==============================================

template<typename T>
void iAPlaneVisModule::highlightDefects( QVector<QString> defects, QVector<QColor> colors, QString labeledImgPath )
{
	QVector<iA4DCTDefects::VectorDataType> defectsLists;
	for( int i = 0; i < defects.size(); i++ )
	{
		iA4DCTDefects::VectorDataType defList = iA4DCTDefects::load( defects[i] );
		defectsLists.push_back( defList );
	}

	// read the labeled image
	vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New( );
	reader->SetFileName( labeledImgPath.toStdString().c_str( ) );
	reader->Update( );
	vtkImageData * labeledImg = reader->GetOutput();

	// hash the defects
	QVector<iA4DCTDefects::HashDataType> hashes;
	for ( auto l : defectsLists ) {
		iA4DCTDefects::HashDataType hash = iA4DCTDefects::DefectDataToHash( l );
		hashes.push_back( hash );
	}

	// scalars to colors
	vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
	lookupTable->SetRange(0, 255);
	lookupTable->SetValueRange(0., 1.);
	lookupTable->SetSaturationRange(0., 0.);
	lookupTable->SetRampToLinear();
	lookupTable->Build();

	vtkSmartPointer<vtkImageMapToColors> scalarValuesToColors =	vtkSmartPointer<vtkImageMapToColors>::New();
	scalarValuesToColors->PassAlphaToOutputOn( );
	scalarValuesToColors->SetLookupTable( lookupTable );
	scalarValuesToColors->SetInputData( m_img );
	scalarValuesToColors->Update( );
	m_img = scalarValuesToColors->GetOutput( );

	int * dims = labeledImg->GetDimensions( );
	for (int x = 0; x < dims[0]; x++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int z = 0; z < dims[2]; z++)
			{
				T * labeledPixel = static_cast<T *>( labeledImg->GetScalarPointer( x, y, z ) );
				for (int i = 0; i < hashes.size(); i++)
				{
					if ( hashes[i].contains( labeledPixel[0] ) ) {
						unsigned char * imgPixel = static_cast<unsigned char *>( m_img->GetScalarPointer( x, y, z ) );
						imgPixel[0] = colors[i].red( );
						imgPixel[1] = colors[i].green( );
						imgPixel[2] = colors[i].blue( );
						imgPixel[3] = colors[i].alpha( );
						break;
					}
				}
			}
		}
	}

	m_reslice->SetInputData( m_img );
}

template<typename T>
void iAPlaneVisModule::densityMap( QString defect, QColor color, QString labeledImgPath, int * size )
{
	// read the labeled image
	vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New( );
	reader->SetFileName( labeledImgPath.toStdString().c_str( ) );
	reader->Update( );
	vtkImageData * labeledImg = reader->GetOutput();

	// hash the defect
	iA4DCTDefects::VectorDataType list = iA4DCTDefects::load( defect );

	//int size[0] = 5; int size[1] = 5; int size[2] = 5;
	int*** densityMap;
	densityMap = new int**[size[0]];
	for( int x = 0; x < size[0]; x++ )
	{
		densityMap[x] = new int*[size[1]];
		for(int y = 0; y < size[1]; y++ )
		{
			densityMap[x][y] = new int[size[2]];
			for( int z = 0; z < size[2]; z++ )
			{
				densityMap[x][y][z] = 0.;
			}
		}
	}

	labledImageToMask<T>( labeledImg, list );

	int * dims = labeledImg->GetDimensions( );
	int maxDensity = 0;
	for (int x = 0; x < dims[0]; x++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int z = 0; z < dims[2]; z++)
			{
				T * labeledPixel = static_cast<T *>( labeledImg->GetScalarPointer( x, y, z ) );
				if( labeledPixel[0] > 0)
				{
					int newX = x * size[0] / dims[0];
					int newY = y * size[1] / dims[1];
					int newZ = z * size[2] / dims[2];
					int density = densityMap[newX][newY][newZ]++;
					if( density > maxDensity ) maxDensity = density;
				}
			}
		}
	}

	// scalars to colors
	vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
	lookupTable->SetRange(0, 255);
	lookupTable->SetValueRange(0., 1.);
	lookupTable->SetSaturationRange(0., 0.);
	lookupTable->SetRampToLinear();
	lookupTable->Build();

	vtkSmartPointer<vtkImageMapToColors> scalarValuesToColors =	vtkSmartPointer<vtkImageMapToColors>::New();
	scalarValuesToColors->PassAlphaToOutputOn( );
	scalarValuesToColors->SetLookupTable( lookupTable );
	scalarValuesToColors->SetInputData( m_img );
	scalarValuesToColors->Update( );
	m_img = scalarValuesToColors->GetOutput( );

	for (int x = 0; x < dims[0]; x++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int z = 0; z < dims[2]; z++)
			{
				unsigned char * imgPixel = static_cast<unsigned char *>( m_img->GetScalarPointer( x, y, z ) );
				int newX = x * size[0] / dims[0];
				int newY = y * size[1] / dims[1];
				int newZ = z * size[2] / dims[2];
				double coef = (double)densityMap[newX][newY][newZ] / maxDensity;
				imgPixel[0] = imgPixel[0] + (color.red()   - imgPixel[0] ) * coef;
				imgPixel[1] = imgPixel[1] + (color.green() - imgPixel[1] ) * coef;
				imgPixel[2] = imgPixel[2] + (color.blue()  - imgPixel[2] ) * coef;
			}
		}
	}

	m_reslice->SetInputData( m_img );

	for( int x = 0; x < size[0]; x++ )
	{
		for(int y = 0; y < size[1]; y++ )
		{
			delete [] densityMap[x][y];
		}
		delete [] densityMap[x];
	}
	delete [] densityMap;
}

template<typename T>
void iAPlaneVisModule::labledImageToMask( vtkImageData* img, iA4DCTDefects::VectorDataType list )
{
	iA4DCTDefects::HashDataType hash = iA4DCTDefects::DefectDataToHash(list);

	int* dims = img->GetDimensions();
	for (int x = 0; x < dims[0]; x++) {
		for (int y = 0; y < dims[1]; y++) {
			for (int z = 0; z < dims[2]; z++) {
				T* pixel = static_cast<T*>(img->GetScalarPointer(x, y, z));
				if (hash.contains(pixel[0])) {
					pixel[0] = 1;
				} else {
					pixel[0] = 0;
				}
			}
		}
	}
}

#endif // IAPLANEVISMODULE_H