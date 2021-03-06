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
 
#ifndef IA_SPECTRUM_TYPE_H
#define IA_SPECTRUM_TYPE_H

#include "iAFunction.h"

#include <QSharedPointer>
#include <QVector>

#include <cstddef> // for size_t

class iASpectrumType;

typedef double iASpectrumDataType;

//! base class for access to multi-channel data, arranged as array
class iASpectralVoxelData
{
private:
	mutable iASpectrumDataType m_maxSum;
public:
	iASpectralVoxelData();
	virtual ~iASpectralVoxelData();
	virtual size_t size() const =0;
	virtual size_t channelCount() const =0;
	virtual QSharedPointer<iASpectrumType const> get(size_t voxelIdx) const =0;
	virtual iASpectrumDataType get(size_t voxelIdx, size_t channelIdx) const =0;
	iASpectrumDataType getMaxSum() const;
};

//! accessor class to the data of all channels for one voxel inside iASpectralVoxelData 
class iASpectrumType
{
public:
	typedef size_t IndexType;
	virtual iASpectrumDataType operator[](size_t channelIdx) const;
	virtual iASpectrumDataType get(size_t channelIdx) const =0;
	virtual IndexType size() const =0;
	virtual QSharedPointer<iASpectrumType const> normalized() const;
};

class iAStandaloneSpectrumType: public iASpectrumType
{
private:
	std::vector<iASpectrumDataType> m_data;
public:
	iAStandaloneSpectrumType(IndexType size);
	virtual iASpectrumDataType get(size_t channelIdx) const;
	virtual IndexType size() const;
	void set(IndexType, iASpectrumDataType);
};

class iADirectAccessSpectrumType: public iASpectrumType
{
private:
	iASpectralVoxelData const & m_data;
	size_t m_voxelIdx;
public:
	iADirectAccessSpectrumType(iASpectralVoxelData const & data, size_t voxelIdx);
	iASpectrumDataType get(size_t channelIdx) const;
	IndexType size() const;
};


//QSharedPointer<iASpectrumType> getNormalized(QSharedPointer<iASpectrumType const> spec);

#endif // IA_SPECTRUM_TYPE_H