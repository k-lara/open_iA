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
 
#include "pch.h"
#include "iARepresentative.h"

#include "iAToolsITK.h"

#include <QVector>

template <class T>
void diff_marker_tmpl(QVector<iAITKIO::ImagePointer> imgsBase, int differenceMarkerValue, iAITKIO::ImagePointer & result)
{
	typedef itk::Image<T, iAITKIO::m_DIM > ImgType;
	QVector<ImgType*> imgs;
	for (int i = 0; i < imgsBase.size(); ++i)
	{
		imgs.push_back(dynamic_cast<ImgType*>(imgsBase[i].GetPointer()));
	}
	typename ImgType::Pointer out = CreateImage<ImgType>(imgs[0]);
	typename iAITKIO::ImageBaseType::RegionType reg = imgs[0]->GetLargestPossibleRegion();
	typename iAITKIO::ImageBaseType::SizeType size = reg.GetSize();
	typename iAITKIO::ImageBaseType::IndexType idx;
	for (idx[0] = 0; idx[0] < size[0]; ++idx[0])
	{
		for (idx[1] = 0; idx[1] < size[1]; ++idx[1])
		{
			for (idx[2] = 0; idx[2] < size[2]; ++idx[2])
			{
				double pixel = imgs[0]->GetPixel(idx);
				for (int i = 1; i < imgs.size(); ++i)
				{
					if (imgs[i]->GetPixel(idx) != pixel)
					{
						pixel = differenceMarkerValue;
					}
				}
				out->SetPixel(idx, pixel);
			}
		}
	}
	result = out;
}

iAITKIO::ImagePointer CalculateDifferenceMarkers(QVector<iAITKIO::ImagePointer> imgs, int differenceMarkerValue)
{
	if (imgs.size() == 0)
	{
		DEBUG_LOG("No images given for calculating difference marker!\n");
		return iAITKIO::ImagePointer();
	}
	iAITKIO::ImagePointer result;
	ITK_TYPED_CALL(diff_marker_tmpl, GetITKScalarPixelType(imgs[0]), imgs, differenceMarkerValue, result);
	return result;
}
