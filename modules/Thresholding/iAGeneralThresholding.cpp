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
#include "iAGeneralThresholding.h"
#include "iAConnector.h"
#include "iAProgress.h"
#include "iATypedCallHelper.h"

#include <itkThresholdImageFilter.h>

#include <QLocale>

/**
* General threshold template initializes itkThresholdImageFilter .
* \param	l		Lower threshold value. 
* \param	u		Upper threshold value. 
* \param	o		Value outside the range. 
* \param	p		Filter progress information. 
* \param	image	Input image. 
* \param	T		Template datatype. 
* \return	int		Status code. 
*/
template<class T> 
int general_threshold_template( double l, double u, double o, iAProgress* p, iAConnector* image )
{
	typedef itk::Image< T, 3 >   ImageType;
	typedef itk::ThresholdImageFilter <ImageType> GTIFType;
	typename GTIFType::Pointer filter = GTIFType::New();
	filter->SetLower( T(l) );
	filter->SetUpper( T(u) );
	filter->SetOutsideValue( T(o) );
	filter->ThresholdOutside(T(l), T(u));
	filter->SetInput( dynamic_cast< ImageType * >( image->GetITKImage() ) );

	p->Observe( filter );

	filter->Update();
	image->SetImage(filter->GetOutput());
	image->Modified();

	filter->ReleaseDataFlagOn();

	return EXIT_SUCCESS;
}

iAGeneralThresholding::iAGeneralThresholding( QString fn, FilterID fid, vtkImageData* i, vtkPolyData* p, iALogger* logger, QObject* parent )
	: iAFilter( fn, fid, i, p, logger, parent )
{

}

iAGeneralThresholding::~iAGeneralThresholding()
{
}

void iAGeneralThresholding::run()
{
	switch (getFilterID())
	{
	case GENERAL_THRESHOLD: 
		thresholding(); break;
	case UNKNOWN_FILTER: 
	default:
		addMsg(tr("unknown filter type"));
	}
}

void iAGeneralThresholding::thresholding(  )
{
	
	addMsg(tr("%1  %2 started.").arg(QLocale().toString(Start(), QLocale::ShortFormat))
		.arg(getFilterName()));
	getConnector()->SetImage(getVtkImageData()); getConnector()->Modified();

	try
	{
		iAConnector::ITKScalarPixelType itkType = getConnector()->GetITKScalarPixelType();
		ITK_TYPED_CALL(general_threshold_template, itkType,
			lower, upper, outer, getItkProgress(), getConnector());
	}
	catch( itk::ExceptionObject &excep)
	{
		addMsg(tr("%1  %2 terminated unexpectedly. Elapsed time: %3 ms").arg(QLocale().toString(QDateTime::currentDateTime(), QLocale::ShortFormat))
			.arg(getFilterName())														
			.arg(Stop())); 
		addMsg(tr("  %1 in File %2, Line %3").arg(excep.GetDescription())
			.arg(excep.GetFile())
			.arg(excep.GetLine()));
		return;
	}
	addMsg(tr("%1  %2 finished. Elapsed time: %3 ms").arg(QLocale().toString(QDateTime::currentDateTime(), QLocale::ShortFormat))
		.arg(getFilterName())														
		.arg(Stop()));
	emit startUpdate();	
}

