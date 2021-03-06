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
#include "iAWatershedSegmentation.h"

#include "iAConnector.h"
#include "iAProgress.h"
#include "iATypedCallHelper.h"

#ifdef __GNUC__
#include <inttypes.h>
#endif

#include <itkCastImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkMorphologicalWatershedImageFilter.h>
#include <itkScalarToRGBPixelFunctor.h>
#include <itkUnaryFunctorImageFilter.h>
#include <itkWatershedImageFilter.h>

#include <vtkImageData.h>

#include <QLocale>

/**
* Watershed template initializes itkWatershedImageFilter .
* \param	l		SetLevel. 
* \param	t		SetThreshold. 
* \param	p		Filter progress information. 
* \param	image	Input image. 
* \param			The. 
* \return	int		Status code 
*/
template<class T> 
int watershed_template( double l, double t, iAProgress* p, iAConnector* image, vtkImageData* imageDataNew )
{
	typedef itk::Image< T, DIM >   InputImageType;

	typedef itk::WatershedImageFilter < InputImageType > WIFType;
	typename WIFType::Pointer filter = WIFType::New();
	filter->SetLevel ( l );
	filter->SetThreshold ( t);
	filter->SetInput( dynamic_cast< InputImageType * >( image->GetITKImage() ) );

	p->Observe( filter );

	filter->Update();

	typedef itk::Image< typename WIFType::OutputImagePixelType, 3 > IntImageType;
	typedef itk::Image<	unsigned long, 3>  LongImageType;
	typedef itk::CastImageFilter< IntImageType, LongImageType > CastFilterType;
	typename CastFilterType::Pointer longcaster = CastFilterType::New();
	longcaster->SetInput(0, filter->GetOutput() );
	image->SetImage( longcaster->GetOutput() );
	image->Modified();
 
	imageDataNew->Initialize();
	imageDataNew->DeepCopy(image->GetVTKImage());
	imageDataNew->CopyInformationFromPipeline(image->GetVTKImage()->GetInformation());

	filter->ReleaseDataFlagOn();
	longcaster->ReleaseDataFlagOn();

	return EXIT_SUCCESS;
}

template<class T>
int morph_watershed_template( QString mwsRGBFilePath, double mwsLevel, bool mwsMarkWSLines, bool mwsFullyConnected, bool mwsSaveRGBImage,
							  iAProgress* p, iAConnector* image, vtkImageData* imageDataNew )
{
	typedef itk::Image< T, DIM >   InputImageType;
	typedef itk::Image< unsigned long, DIM > OutputImageType;
	typedef itk::RGBPixel< unsigned char > RGBPixelType;
	typedef itk::Image< RGBPixelType, DIM > RGBImageType;
	typedef  itk::ImageFileWriter< RGBImageType  > WriterType;

	typedef itk::MorphologicalWatershedImageFilter<InputImageType, OutputImageType> MWIFType;
	MWIFType::Pointer mWSFilter = MWIFType::New();
	mwsMarkWSLines ? mWSFilter->MarkWatershedLineOn() : mWSFilter->MarkWatershedLineOff();
	mwsFullyConnected ? mWSFilter->FullyConnectedOn() : mWSFilter->FullyConnectedOff();
	mWSFilter->SetLevel( mwsLevel );
	mWSFilter->SetInput( dynamic_cast< InputImageType * >( image->GetITKImage() ) );

	p->Observe( mWSFilter );
	mWSFilter->Update();

	typedef itk::Image< typename MWIFType::OutputImagePixelType, 3 > IntImageType;
	typedef itk::Image<	unsigned long, 3>  LongImageType;
	typedef itk::CastImageFilter< IntImageType, LongImageType > CastFilterType;
	typename CastFilterType::Pointer longcaster = CastFilterType::New();
	longcaster->SetInput( 0, mWSFilter->GetOutput() );

	typedef MWIFType::OutputImageType  LabeledImageType;
	typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long> ColorMapFunctorType;
	typedef itk::UnaryFunctorImageFilter<LabeledImageType, RGBImageType, ColorMapFunctorType> ColorMapFilterType;
	ColorMapFilterType::Pointer colormapper = ColorMapFilterType::New();
	colormapper->SetInput( mWSFilter->GetOutput() );

	if ( !mwsRGBFilePath.isEmpty() && mwsSaveRGBImage )
	{
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName( mwsRGBFilePath.toStdString() );
		writer->SetInput( colormapper->GetOutput() );
		try
		{
			writer->Update();
		}
		catch ( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception caught !" << std::endl;
			std::cerr << excep << std::endl;
			return EXIT_FAILURE;
		}
	}

	image->SetImage( longcaster->GetOutput() );
	image->Modified();

	imageDataNew->Initialize();
	imageDataNew->DeepCopy( image->GetVTKImage() );
	imageDataNew->CopyInformationFromPipeline( image->GetVTKImage()->GetInformation() );

	mWSFilter->ReleaseDataFlagOn();
	longcaster->ReleaseDataFlagOn();

	return EXIT_SUCCESS;
}

iAWatershedSegmentation::iAWatershedSegmentation( QString fn, FilterID fid, vtkImageData* i, vtkPolyData* p, iALogger* logger, QObject* parent ) 
	: iASegmentation( fn, fid, i, p, logger, parent )
{
	imageDataNew = vtkImageData::New();
}

iAWatershedSegmentation::~iAWatershedSegmentation()
{

}

void iAWatershedSegmentation::run()
{
	switch (getFilterID())
	{
	case WATERSHED: 
		watershed(); break;
	case MORPH_WATERSHED:
		morph_watershed(); break;
	case UNKNOWN_FILTER: 
	default:
		addMsg(tr("  unknown filter type"));
	}
}

void iAWatershedSegmentation::watershed(  )
{
	addMsg(tr("%1  %2 started.").arg(QLocale().toString(Start(), QLocale::ShortFormat))
		.arg(getFilterName()));
	getConnector()->SetImage(getVtkImageData()); getConnector()->Modified();

	try
	{
		iAConnector::ITKScalarPixelType itkType = getConnector()->GetITKScalarPixelType();
		ITK_TYPED_CALL(watershed_template, itkType, level, threshold, getItkProgress(), getConnector(), imageDataNew);
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

void iAWatershedSegmentation::morph_watershed()
{
	addMsg( tr( "%1  %2 started." ).arg( QLocale().toString( Start(), QLocale::ShortFormat ) )
			.arg( getFilterName() ) );
	getConnector()->SetImage( getVtkImageData() ); getConnector()->Modified();

	try
	{
		iAConnector::ITKScalarPixelType itkType = getConnector()->GetITKScalarPixelType();
		ITK_TYPED_CALL( morph_watershed_template, itkType, mwsRGBFilePath, mwsLevel, mwsMarkWSLines,
						mwsFullyConnected, mwsSaveRGBImage, getItkProgress(), getConnector(), imageDataNew );
	}
	catch ( itk::ExceptionObject &excep )
	{
		addMsg( tr( "%1  %2 terminated unexpectedly. Elapsed time: %3 ms" ).arg( QLocale().toString( QDateTime::currentDateTime(), QLocale::ShortFormat ) )
				.arg( getFilterName() )
				.arg( Stop() ) );
		addMsg( tr( "  %1 in File %2, Line %3" ).arg( excep.GetDescription() )
				.arg( excep.GetFile() )
				.arg( excep.GetLine() ) );
		return;
	}
	addMsg( tr( "%1  %2 finished. Elapsed time: %3 ms" ).arg( QLocale().toString( QDateTime::currentDateTime(), QLocale::ShortFormat ) )
			.arg( getFilterName() )
			.arg( Stop() ) );
	if ( mwsSaveRGBImage )
		addMsg( tr( "RGB image saved to : %1" ).arg( mwsRGBFilePath ) );
	emit startUpdate();
}
