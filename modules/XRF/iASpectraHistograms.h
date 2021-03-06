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
 
#ifndef IA_SPECTRA_HISTOGRAMS_H
#define IA_SPECTRA_HISTOGRAMS_H

class iAXRFData;

#include "iAEnergySpectrum.h"

#include <QSharedPointer>

class iASpectraHistograms
{
public:
	iASpectraHistograms(QSharedPointer<iAXRFData> xrfData, long numBins = 1, double minCount = 0, double maxCount = 0);
	~iASpectraHistograms();
	void compute(long numBins, double maxCount, double minCount);
	CountType * histData() const;
	long numBins() const;
	size_t numHist() const;
	CountType maxValue() const;
private:
	void computeHistograms();
	void computeMaximumVal();

private:
	long	m_numBins;			///< number of bins in a histogram
	size_t	m_numHistograms;	///< number of energy bins is the number of histograms
	double	m_countRange[2];	///< range of XRF 

	double			m_binWidth;			///< width of a histogram bin
	CountType	*	m_histData;			///< raw data containing a 2D array, first dimension - histograms, second - bins
	
	QSharedPointer<iAXRFData> m_xrfData;	///< pointer to the input xrf data set
	CountType	m_maxValue;			///< maximum value of all histograms' bins
};

#endif // IA_SPECTRA_HISTOGRAMS_H
