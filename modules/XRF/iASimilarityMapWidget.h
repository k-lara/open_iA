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
 
#ifndef IA_SIMILARITY_MAP_WIDGET_H
#define IA_SIMILARITY_MAP_WIDGET_H

#include <QWidget>
#include <QSharedPointer>
#include <QVector>
#include <vtkSmartPointer.h>

class vtkImageData;
class QImage;

class iASimilarityMapWidget : public QWidget
{
	Q_OBJECT
public:
	iASimilarityMapWidget( QWidget *parent = 0 );
	~iASimilarityMapWidget( );
	void setImageData( vtkImageData * image );
	void setWindowing( double lowerVal, double upperVal );
	void load( const char* filename );
	typedef double ImageScalarType;
protected:
	void paintEvent(QPaintEvent * );
	virtual void mouseMoveEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
	void drawMap();
	void drawPeak();
	void drawAverageSimilarityPlot();
	void updateQtImage();
	void updateAverageSimilarity();
	void applyWindow( ImageScalarType &val_out, const double( &windowRange )[2] );
	void findPeak( int x, int y );
	void findPeakRanges();
	void binsFromPos( const int( &pos )[2], int( &bins_out )[2] );
	void posFromBins( const int( &bins )[2], int( &pos_out )[2] );
signals:
	void energyBinsSelectedSignal( int binX, int binY );

protected:
	vtkSmartPointer<vtkImageData> m_vtkImageData;
	QVector<ImageScalarType> m_avrgSimilarityVec;
	int m_numBins;
	QSharedPointer<QImage>	m_qtImage;
	double m_WindowRange[2];
	int m_mapWidth;
	int m_peakPos[2]; //TODO: refactor?, struct/class iAPeak?
	int m_peakRange[2][2];
};
#endif
