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
 
#ifndef iAIntensityModuleInterface_h__
#define iAIntensityModuleInterface_h__

#include "iAModuleInterface.h"

class MdiChild;

class iAIntensityModuleInterface : public iAModuleInterface
{
	Q_OBJECT

public:
	void Initialize();

private slots:
	void difference_Image_Filter();
	void subtractimage_Filter();
	void invert_intensity();
	void mask();
	void intensity_windowing();

protected:
	//difference settings
	double difDifferenceThreshold, difToleranceRadius;
	//windowing intensity settings
	double windowMinimum, windowMaximum, outputMinimum, outputMaximum;
};

#endif // iAIntensityModuleInterface_h__
