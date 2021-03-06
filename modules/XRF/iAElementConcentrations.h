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
 
#ifndef IA_ELEMENT_CONCENTRATIONS_H
#define IA_ELEMENT_CONCENTRATIONS_H

#include "iAEnergySpectrum.h"

#include <vector>

#include <QSharedPointer>
#include <QVector>

#include <vtkSmartPointer.h>

class vtkImageData;

class iAAccumulatedXRFData;
class iAElementSpectralInfo;
class iAXRFData;

class iAElementConcentrations
{
public:
	typedef vtkImageData ImageType;
	typedef vtkSmartPointer<ImageType> ImagePointerType;
	typedef QVector<double> VoxelConcentrationType;
	typedef std::vector<ImagePointerType> ImageListType;

	iAElementConcentrations();
	~iAElementConcentrations();

	ImageListType * getImageListPtr();

	ImagePointerType getImage(int idx);

	VoxelConcentrationType getConcentrationForVoxel(int x, int y, int z);
	VoxelConcentrationType const & getAvgConcentration();
	void clear();
	bool calculateAverageConcentration(
		QSharedPointer<iAXRFData const> xrfData,
		QVector<iAElementSpectralInfo*> const & elements,
		QSharedPointer<iAAccumulatedXRFData const> accumulatedXRF);
	bool hasAvgConcentration() const;
private:
	void initImages(int elemCount, int extent[6], double spacing[3], double origin[3]);
	QSharedPointer<QVector<QSharedPointer<iAEnergySpectrum> > > GetAdaptedSpectra(
		QSharedPointer<iAXRFData const> xrfData,
		QVector<iAElementSpectralInfo*> const & elements);
	bool calculateAverageConcentration(
		QSharedPointer<QVector<QSharedPointer<iAEnergySpectrum> > > elements,
		QSharedPointer<iAAccumulatedXRFData const> accumulatedXRF);

	int m_elementCount;
	VoxelConcentrationType  m_averageConcentration;
	ImageListType m_ElementConcentration;

	friend class iADecompositionCalculator;
};


#endif /* IA_ELEMENT_CONCENTRATIONS_H */
