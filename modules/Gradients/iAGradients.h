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
 
#ifndef IAGRADIENTS_H
#define IAGRADIENTS_H

#pragma once

#include "iAFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include <itkCastImageFilter.h>

struct HOAccGradientDerrivativeSettings
{
	unsigned int order, direction, orderOfAcc;
};

/**
 * An implementation of itkDerivativeImageFilter and itkGradientMagnitudeImageFilter.
 * For itkDerivativeImageFilter refer to http://www.itk.org/Doxygen/html/classitk_1_1DerivativeImageFilter.html
 * For itkGradientMagnitudeImageFilter refer to http://www.itk.org/Doxygen/html/classitk_1_1GradientMagnitudeImageFilter.html
 * \remarks	Kana, 01/12/2010. 
 */
class iAGradients : public iAFilter
{
public:
	iAGradients( QString fn, FilterID fid, vtkImageData* i, vtkPolyData* p, iALogger* logger, QObject *parent = 0 );
	~iAGradients();

	/**
	 * Sets iAGradients parameters. 
	 * \param	o		SetOrder. 
	 * \param	d		SetDirection.
	 */

	void setDParameters(double o, double d) { order = o; direction = d; };
	void setHOAGDParameters(const HOAccGradientDerrivativeSettings * settings) { m_HOAGDSettings = *settings; };

protected:
    void run();
	void derivative();
	void hoa_derivative();
	void gradient_magnitude();

private:
	unsigned int order, direction;
	HOAccGradientDerrivativeSettings m_HOAGDSettings;

};
#endif
