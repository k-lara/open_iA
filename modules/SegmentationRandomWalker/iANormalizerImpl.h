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
 
#ifndef IA_NORMALIZER_IMPL_H
#define IA_NORMALIZER_IMPL_H

#include "iANormalizer.h"

#include <QSharedPointer>
#include <QString>

enum NormalizeModes
{
	tfInvalid = -1,

	nmNone,
	nmLinear,
	nmGaussian,

	// don't change the order or position of these two:
	nmCount,
	nmDefaultNormalizer = nmGaussian
};

class iANoNormalizer: public iANormalizer
{
public:
	virtual iADistanceType Normalize(iADistanceType d) const;
	virtual void SetMaxValue(iADistanceType d);
	virtual char const * const GetName() const;
};

class iALinearNormalizer: public iANormalizer
{
public:
	virtual iADistanceType Normalize(iADistanceType d) const;
	virtual void SetMaxValue(iADistanceType d);
	virtual char const * const GetName() const;
private:
	iADistanceType m_normalizeFactor;
};

class iAGaussianNormalizer: public iANormalizer
{
public:
	iAGaussianNormalizer();
	virtual iADistanceType Normalize(iADistanceType d) const;
	virtual void SetMaxValue(iADistanceType d);
	virtual char const * const GetName() const;
	void SetBeta(double beta);
private:
	double m_beta;
	iADistanceType m_maxValue;
	iADistanceType m_valueFactor;

	void UpdateValueFactor();
};

char const * const * const GetNormalizerNames();
QSharedPointer<iANormalizer> CreateNormalizer(QString const & name);

#endif