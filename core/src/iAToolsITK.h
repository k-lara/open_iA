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
 
#ifndef TOOLS_ITK_H
#define TOOLS_ITK_H

#include "iAConsole.h"
#include "iAITKIO.h"
#include "open_iA_Core_export.h"

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>

#include <QString>

// TODO: unify with iAITKIO !

open_iA_Core_API itk::ImageIOBase::IOComponentType GetITKScalarPixelType(iAITKIO::ImagePointer image);
open_iA_Core_API iAITKIO::ImagePointer AllocateImage(iAITKIO::ImagePointer img);
open_iA_Core_API iAITKIO::ImagePointer AllocateImage(int const size[3], double const spacing[3], itk::ImageIOBase::IOComponentType type);
open_iA_Core_API void StoreImage(iAITKIO::ImagePointer image, QString const & filename, bool useCompression);

//! @{
//! Generic access to pixels of any ITK image as double.
//! Slow! If you need to access more than a few pixels,
//! convert the whole image first (maybe using templates) and then access directly!
open_iA_Core_API double GetITKPixel(iAITKIO::ImagePointer img, iAITKIO::ImageBaseType::IndexType idx);
open_iA_Core_API void SetITKPixel(iAITKIO::ImagePointer img, iAITKIO::ImageBaseType::IndexType idx, double value);
//! @}

//! Source: http://itk.org/Wiki/ITK/Examples/Utilities/DeepCopy
template<typename TImage>
void DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output)
{
	output->SetRegions(input->GetLargestPossibleRegion());
	output->SetSpacing(input->GetSpacing());
	output->Allocate();
 
	itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
	itk::ImageRegionIterator<TImage> outputIterator(output, output->GetLargestPossibleRegion());
 
	while(!inputIterator.IsAtEnd())
	{
		outputIterator.Set(inputIterator.Get());
		++inputIterator;
		++outputIterator;
	}
}

template <typename TImage>
typename TImage::Pointer CreateImage(typename TImage::SizeType size, typename TImage::SpacingType spacing)
{
	typename TImage::Pointer image = TImage::New();
	typename TImage::IndexType start;
	start.Fill(0);
	typename TImage::RegionType region(start, size);
	image->SetRegions(region);
	image->Allocate();
	image->FillBuffer(0);
	image->SetSpacing(spacing);
	return image;
}

template <typename TImage>
typename TImage::Pointer CreateImage(typename TImage::Pointer otherImg)
{
	typename TImage::Pointer image = TImage::New();
	typename TImage::RegionType reg(
		otherImg->GetLargestPossibleRegion().GetIndex(),
		otherImg->GetLargestPossibleRegion().GetSize()
	);
	image->SetRegions(reg);
	image->Allocate();
	image->FillBuffer(0);
	image->SetSpacing(otherImg->GetSpacing());
	return image;
}

template <typename TImage>
void StoreImage(TImage * image, QString const & filename, bool useCompression = true)
{
	typename itk::ImageFileWriter<TImage>::Pointer writer = itk::ImageFileWriter<TImage>::New();
	try
	{
		writer->SetFileName(filename.toStdString());
		writer->SetUseCompression(useCompression);
		writer->SetInput(image);
		writer->Update();
	}
	catch (itk::ExceptionObject const & e)
	{
		DEBUG_LOG(QString("Error while writing image file '%1': %2\n")
			.arg(filename)
			.arg(e.what()));
	}
}


#endif
