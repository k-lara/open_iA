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
 
#ifndef IA_GRAPH_WEIGHTS_H
#define IA_GRAPH_WEIGHTS_H

#include "iAImageGraphTypes.h"

#include <QSharedPointer>
#include <QVector>

class iANormalizer;
class iAImageGraph;
class iASpectraDistance;

class iAGraphWeights
{
public:
	iAGraphWeights(iAEdgeIndexType edgeCount);
	void Normalize(QSharedPointer<iANormalizer> normalizeFunc);
	iAEdgeWeightType GetMaxWeight() const;
	iAEdgeWeightType GetWeight(iAEdgeIndexType edgeIdx) const;
	void SetWeight(iAEdgeIndexType edgeIdx, iAEdgeWeightType weight);
	int GetEdgeCount() const;
private:
	QVector<iAEdgeWeightType> m_weights;
};

QSharedPointer<iAGraphWeights> CalculateGraphWeights(
	iAImageGraph const & graph,
	iASpectralVoxelData const & voxelData,
	iASpectraDistance const & distanceFunc
);

QSharedPointer<iAGraphWeights const> CombineGraphWeights(
	QVector<QSharedPointer<iAGraphWeights>> const & graphWeights,
	QVector<double> const & weight
);

#endif // IA_GRAPH_WEIGHTS_H