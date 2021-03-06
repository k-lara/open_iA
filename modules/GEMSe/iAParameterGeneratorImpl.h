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
 
#ifndef IA_PARAMETER_GENERATOR_IMPL_H
#define IA_PARAMETER_GENERATOR_IMPL_H

#include "iAParameterGenerator.h"

class iARandomParameterGenerator: public iAParameterGenerator
{
	virtual QString GetName() const;
	virtual ParameterSetsPointer GetParameterSets(QSharedPointer<iAAttributes> parameter, int sampleCount);
};

class iALatinHypercubeParameterGenerator: public iAParameterGenerator
{
	virtual QString GetName() const;
	virtual ParameterSetsPointer GetParameterSets(QSharedPointer<iAAttributes> parameter, int sampleCount);
};

//! as all parameter values are supposed to be equally spaced,
//! and the number of values equally distributed among all parameters,
//! this algorithm will typically give less than the specified amount of samples
class iACartesianGridParameterGenerator : public iAParameterGenerator
{
	virtual QString GetName() const;
	virtual ParameterSetsPointer GetParameterSets(QSharedPointer<iAAttributes> parameter, int sampleCount);
};

QVector<QSharedPointer<iAParameterGenerator> > & GetParameterGenerators();

#endif // IA_PARAMETER_GENERATOR_IMPL_H