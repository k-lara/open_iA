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
#include "iAConvolutionFilter.h"

#include "iAConnector.h"
#include "iAProgress.h"
#include "iATypedCallHelper.h"

#include <itkCastImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkConvolutionImageFilter.h>
#include <itkFFTConvolutionImageFilter.h>
#include <itkFFTNormalizedCorrelationImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkImageFileReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkImportImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkNormalizedCorrelationImageFilter.h>
#include <itkPipelineMonitorImageFilter.h>
#include <itkRecursiveMultiResolutionPyramidImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkScalarConnectedComponentImageFilter.h>
#include <itkScalarImageKmeansImageFilter.h>
#include <itkStreamingImageFilter.h>

#include <vtkContourFilter.h>
#include <vtkCutter.h>
#include <vtkKochanekSpline.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSplineFilter.h>
#include <vtkStripper.h>
#include <vtkTubeFilter.h>

#include <QLocale>

iAConvolutionFilter::iAConvolutionFilter(QString fn, FilterID fid, vtkImageData* i, vtkPolyData* p , iALogger* logger, QObject *parent )
	: iAFilter(fn, fid, i, p, logger, parent) {

	pData = vtkPolyData::New(); 
	pData = p; 
}

iAConvolutionFilter::~iAConvolutionFilter() {}

template<class T> int convolutionFilterTemplate(iAConnector* image, iAProgress* p, std::string templateFileName)
{
	typedef itk::Image<T, DIM> ImageType;	
	typedef itk::Image<float, DIM> KernelImageType; 
	typename ImageType::Pointer img = dynamic_cast<ImageType *>(image->GetITKImage());

	typedef itk::ImageFileReader<KernelImageType> ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	reader->SetFileName(templateFileName);
	reader->Update();

	p->Observe(reader); 

	typename KernelImageType::Pointer kernelImg = reader->GetOutput();

	typedef itk::ConvolutionImageFilter<ImageType, KernelImageType, KernelImageType> ConvFilterType;
	typename ConvFilterType::Pointer filter = ConvFilterType::New();

	filter->SetInput(img);

#if ITK_VERSION_MAJOR >= 4
	filter->SetKernelImage(kernelImg);
#else
	filter->SetImageKernelInput(kernelImg);
#endif

	p->Observe(filter);
	filter->Update();

	image->SetImage(filter->GetOutput());
	image->Modified();

	return EXIT_SUCCESS;
}

//fft based convolution instead of spatial domain convolution
template<class T> int fft_convolutionFilterTemplate(iAConnector* image, iAProgress* p, std::string templateFileName)
{
	typedef itk::Image<T, DIM> ImageType;
	typedef itk::Image<float, DIM> KernelImageType;
	typename ImageType::Pointer img = dynamic_cast<ImageType *>(image->GetITKImage());

	typedef itk::ImageFileReader<KernelImageType> ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	reader->SetFileName(templateFileName);
	reader->Update();

	p->Observe(reader);

	typename KernelImageType::Pointer kernelImg = reader->GetOutput();

	typedef itk::FFTConvolutionImageFilter<ImageType, KernelImageType, KernelImageType> ConvFilterType;
	typename ConvFilterType::Pointer filter = ConvFilterType::New();

	filter->SetInput(img);

#if ITK_VERSION_MAJOR >= 4
	filter->SetKernelImage(kernelImg);
#else
	filter->SetImageKernelInput(kernelImg);
#endif

	filter->SetNormalize(true); 

	p->Observe(filter);
	filter->Update();

	image->SetImage(filter->GetOutput());
	image->Modified();

	return EXIT_SUCCESS;
}

template<class T> int kmeansclusterFilterTemplate(iAConnector* image, iAProgress* p, std::string templateFileName)
{
	typedef itk::Image<T, DIM> ImageType;
	typedef itk::Image<int, DIM> IntImageType; 
	typedef itk::Image<float, DIM> FloatImageType;


	typedef itk::ScalarImageKmeansImageFilter<ImageType, ImageType> KMeansFilterType;
	typename KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();

	kmeansFilter->SetInput(dynamic_cast<ImageType*> (image->GetITKImage()));

	const unsigned int numberOfInitialClasses = 2;
	const unsigned int useNonContiguousLabels = true;

	kmeansFilter->SetUseNonContiguousLabels(useNonContiguousLabels);

	//bimodal distribution -> fibre datasets -> always?
	//initial mean values for each class
	kmeansFilter->AddClassWithInitialMean(itk::NumericTraits<unsigned short>::max() * 1 / 3);
	kmeansFilter->AddClassWithInitialMean(itk::NumericTraits<unsigned short>::max() * 2 / 3);

	p->Observe(kmeansFilter); 

	kmeansFilter->Update();

	typename KMeansFilterType::ParametersType estimatedMeans = kmeansFilter->GetFinalMeans();
	const unsigned int numberOfActualClasses = estimatedMeans.Size();
	std::cout << "number of actual  classes : " << numberOfActualClasses << std::endl;

	for (size_t i = 0; i < numberOfActualClasses; i++)
	{
		std::cout << "cluster[" << i << "]";
		std::cout << "	estimated mean: " << estimatedMeans[i] << std::endl;
	}

	typedef typename KMeansFilterType::OutputImageType OutputImageType;

	typedef itk::ScalarConnectedComponentImageFilter <ImageType, IntImageType> ConnectedComponentImageFilterType;
	typename ConnectedComponentImageFilterType::Pointer labelFilter = ConnectedComponentImageFilterType::New();
	labelFilter->FullyConnectedOn();
	labelFilter->SetInput(kmeansFilter->GetOutput());
	labelFilter->SetDistanceThreshold(4); 

	labelFilter->Update();

	std::cout << "num of Objects: " << labelFilter->GetObjectCount() << std::endl;

	typedef itk::RelabelComponentImageFilter<IntImageType, IntImageType>  RelabelFilterType;
	typename RelabelFilterType::Pointer relabeler = RelabelFilterType::New();

	relabeler->SetInput(labelFilter->GetOutput());

	relabeler->Update(); 

	typedef itk::RescaleIntensityImageFilter<IntImageType> RescaleFilterType;
	typename RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

	rescaler->SetInput(relabeler->GetOutput());
	rescaler->SetOutputMinimum(0);
	rescaler->SetOutputMaximum(itk::NumericTraits<unsigned short>::max());

	rescaler->Update(); 

	typedef std::vector<RelabelFilterType::ObjectSizeType> SizesType;
	const SizesType & sizes = relabeler->GetSizeOfObjectsInPixels();

	std::cout << "size of sizes : " << sizes.size() << std::endl; 
	SizesType::const_iterator sizeItr = sizes.begin();
	SizesType::const_iterator sizeEnd = sizes.end();

	//std::cout << "Number of Pixels per class " << std::endl;

	//unsigned int kclass = 0;

	//while (sizeItr != sizeEnd)
	//{
	//	std::cout << "Class " << kclass << " = " << *sizeItr << std::endl;
	//	kclass++;
	//	sizeItr++;
	//}

	image->SetImage(rescaler->GetOutput());
	image->Modified();

	return 0; 
}

template<class T> int correlationFilterTemplate(iAConnector* image, iAProgress* p, std::string templateFileName)
{
	typedef itk::Image<T, DIM> ImageType;
	typedef itk::Image<float, DIM> KernelImageType;
	typename ImageType::Pointer img = dynamic_cast<ImageType *>(image->GetITKImage());

	typedef itk::ImageFileReader<KernelImageType> ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	reader->SetFileName(templateFileName);
	reader->Update();

	typename KernelImageType::Pointer template_img = dynamic_cast<KernelImageType*>(reader->GetOutput()); 

	p->Observe(reader);

	typedef itk::NormalizedCorrelationImageFilter<ImageType, KernelImageType, KernelImageType> CorrelationFilterType;
	// The radius of the kernel must be the radius of the patch, NOT the size of the patch
	itk::Size<3> radius = template_img->GetLargestPossibleRegion().GetSize();
	radius[0] = (radius[0] - 1) / 2;
	radius[1] = (radius[1] - 1) / 2;
	radius[2] = (radius[2] - 1) / 2;
	
	itk::ImageKernelOperator<float, 3> kernelOperator;
	kernelOperator.SetImageKernel(template_img);
	kernelOperator.CreateToRadius(radius);

	typename CorrelationFilterType::Pointer filter = CorrelationFilterType::New();
	filter->SetInput(img);
	filter->SetTemplate(kernelOperator);

	p->Observe(filter);
	filter->Update();

	image->SetImage(filter->GetOutput());
	image->Modified();

	return EXIT_SUCCESS;
}

//NCC calculation using fft
template<class T> int fft_correlationFilterTemplate(iAConnector* image, iAProgress* p, std::string templateFileName, vtkPolyData*pdata)
{
	typedef itk::Image<T, DIM> ImageType;
	typedef itk::Image<float, DIM> KernelImageType;
	typename ImageType::Pointer img = dynamic_cast<ImageType *>(image->GetITKImage());

	typedef itk::ImageFileReader<KernelImageType> ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	reader->SetFileName(templateFileName);
	reader->Update();

	typename KernelImageType::Pointer template_img = dynamic_cast<KernelImageType*>(reader->GetOutput());

	//cast input image to float
	typedef itk::CastImageFilter<ImageType, KernelImageType> CasterType;
	typename CasterType::Pointer caster = CasterType::New();
	caster->SetInput(img);
	
	typedef itk::FFTNormalizedCorrelationImageFilter<KernelImageType, KernelImageType> CorrelationFilterType;
	typename CorrelationFilterType::Pointer filter = CorrelationFilterType::New();
	filter->SetInput(caster->GetOutput()); 
	//filter->SetFixedImage(img);
	filter->SetMovingImage(template_img); 
	filter->Modified(); 

	p->Observe(filter);
	filter->Update();

	image->SetImage(filter->GetOutput());
	image->Modified();

	return EXIT_SUCCESS;
}

template<class T> int streamed_fft_correlationFilterTemplate(iAConnector* image, iAProgress* p, std::string templateFileName, vtkPolyData*pData)
{
	std::cout << "starting streamed FFT NCC calculation" << std::endl; 

	typedef itk::Image<T, DIM> ImageType;
	typedef itk::Image<float, DIM> KernelImageType;
	typename ImageType::Pointer img = dynamic_cast<ImageType *>(image->GetITKImage());

	typedef itk::ImageFileReader<KernelImageType> ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	reader->SetFileName(templateFileName);
	reader->Update();

	typename KernelImageType::Pointer template_img = dynamic_cast<KernelImageType*>(reader->GetOutput());
	
	//cast input image to float
	typedef itk::CastImageFilter<ImageType, KernelImageType> CasterType;
	typename CasterType::Pointer caster = CasterType::New();
	caster->SetInput(img);

	typedef itk::PipelineMonitorImageFilter<KernelImageType> MonitorFilterType;
	typename MonitorFilterType::Pointer monitorFilter = MonitorFilterType::New(); 
	monitorFilter->SetInput(caster->GetOutput());
	monitorFilter->DebugOn(); 

	int numStreamDivisions = 100;
	std::cout << "Number of stream divisions: " << numStreamDivisions << std::endl;

	typedef itk::StreamingImageFilter<KernelImageType, KernelImageType> StreamingFilterType;
	typename StreamingFilterType::Pointer streamer = StreamingFilterType::New(); 
	streamer->SetInput(monitorFilter->GetOutput());
	streamer->SetNumberOfStreamDivisions(numStreamDivisions); 	
	
	typedef itk::FFTNormalizedCorrelationImageFilter<KernelImageType, KernelImageType> CorrelationFilterType;
	//typedef itk::NormalizedCorrelationImageFilter<KernelImageType, KernelImageType, KernelImageType> CorrelationFilterType;
	typename CorrelationFilterType::Pointer filter = CorrelationFilterType::New();
	filter->SetInput(streamer->GetOutput());
	//filter->SetFixedImage(img);
	filter->SetMovingImage(template_img);

	p->Observe(filter);
	filter->Update();

	image->SetImage(filter->GetOutput());
	image->Modified();
	
	return EXIT_SUCCESS;

}

// fft ncc only on contour
template<class T> int fft_contour_correlationFilterTemplate2(iAConnector* image, iAProgress* p, std::string templateFileName, vtkPolyData*pData)
{
	typedef itk::Image<T, DIM> ImageType;
	typedef itk::Image<float, DIM> KernelImageType;
	typename ImageType::Pointer img = dynamic_cast<ImageType *>(image->GetITKImage());

	
	typedef itk::ImageFileReader<KernelImageType> ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	reader->SetFileName(templateFileName);
	reader->Update();

	typename KernelImageType::Pointer template_img = dynamic_cast<KernelImageType*>(reader->GetOutput());	

	typedef itk::ImageToVTKImageFilter<ImageType> ConverterType; 
	typename ConverterType::Pointer converter = ConverterType::New(); 
	converter->SetInput(img); 
	converter->Update(); 

	//vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter = 
	//	vtkSmartPointer<vtkImageDataGeometryFilter>::New();
	//imageDataGeometryFilter->SetInputData(converter->GetOutput());
	//imageDataGeometryFilter->Update(); 

	vtkSmartPointer<vtkContourFilter> skinExtractor =   vtkSmartPointer<vtkContourFilter>::New();
	skinExtractor->SetInputData(converter->GetOutput());
	skinExtractor->SetValue(0, 50000);
	skinExtractor->Update();
	
	//cast input image to float
	//typedef itk::CastImageFilter<ImageType, KernelImageType> CasterType;
	//CasterType::Pointer caster = CasterType::New();
	//caster->SetInput(img);
	
	//typedef itk::SimpleContourExtractorImageFilter<ImageType, ImageType> ContourFilterType; 
	//typename ContourFilterType::Pointer contourFilter = ContourFilterType::New(); 
	//contourFilter->SetInput(img); 
	//contourFilter->SetInputBackgroundValue(12000); 
	//contourFilter->SetInputForegroundValue(50000); 
	//contourFilter->Update(); 

	//typedef itk::FFTNormalizedCorrelationImageFilter<KernelImageType, KernelImageType> CorrelationFilterType;
	//typename CorrelationFilterType::Pointer filter = CorrelationFilterType::New();
	//filter->SetInput(caster->GetOutput());
	////filter->SetFixedImage(img);
	//filter->SetMovingImage(template_img);
	//filter->Modified();

	//p->Observe(skinExtractor);
	//skinExtractor->Update();

	image->SetImage(img);
	image->Modified();

	pData->ShallowCopy(skinExtractor->GetOutput());
	pData->Modified(); 

	std::cout << "numb of points: " << pData->GetNumberOfPoints() << std::endl; 

	//vtkSmartPointer<vtkPolyDataMapper> mapper =
	//vtkSmartPointer<vtkPolyDataMapper>::New();
	//mapper->SetInputData(pData);

	//vtkSmartPointer<vtkActor> actor =
	//vtkSmartPointer<vtkActor>::New();
	//actor->SetMapper(mapper);
	//actor->GetProperty()->SetPointSize(5);

	vtkSmartPointer<vtkPlane> plane =
		vtkSmartPointer<vtkPlane>::New();
	plane->SetNormal(0, 0, 1);

	vtkSmartPointer<vtkCutter> cutter =
		vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(pData);
	cutter->SetCutFunction(plane);
	cutter->GenerateValues(1, 0.0, 0.0);

	vtkSmartPointer<vtkPolyDataMapper> modelMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	modelMapper->SetInputData(pData);

	vtkSmartPointer<vtkActor> model =
		vtkSmartPointer<vtkActor>::New();
	model->SetMapper(modelMapper);

	vtkSmartPointer<vtkStripper> stripper =
		vtkSmartPointer<vtkStripper>::New();
	stripper->SetInputConnection(cutter->GetOutputPort());

	vtkSmartPointer<vtkKochanekSpline> spline =
		vtkSmartPointer<vtkKochanekSpline>::New();
	spline->SetDefaultTension(.1);

	vtkSmartPointer<vtkSplineFilter> sf =
		vtkSmartPointer<vtkSplineFilter>::New();

	sf->SetInputConnection(stripper->GetOutputPort());
	sf->SetSubdivideToSpecified();
	sf->SetNumberOfSubdivisions(50);
	sf->SetSpline(spline);
	sf->GetSpline()->ClosedOn();

	vtkSmartPointer<vtkTubeFilter> tubes =
		vtkSmartPointer<vtkTubeFilter>::New();
	tubes->SetInputConnection(sf->GetOutputPort());
	tubes->SetNumberOfSides(2);
	tubes->SetRadius(0.2);

	vtkSmartPointer<vtkPolyDataMapper> linesMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	linesMapper->SetInputConnection(sf->GetOutputPort());
	linesMapper->ScalarVisibilityOff();

	vtkSmartPointer<vtkActor> lines =
		vtkSmartPointer<vtkActor>::New();
	lines->SetMapper(linesMapper);

	vtkSmartPointer<vtkRenderer> ren =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin =
		vtkSmartPointer<vtkRenderWindow>::New();

	renWin->AddRenderer(ren);

	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	// Add the actors to the renderer
	ren->AddActor(lines);
	ren->SetBackground(0.1, 0.2, 0.4);

	// This starts the event loop and as a side effect causes an initial
	// render.
	renWin->Render();
	iren->Start();

	// Extract the lines from the polydata
	vtkIdType numberOfLines = cutter->GetOutput()->GetNumberOfLines();


	std::cout << "-----------Lines without using vtkStripper" << std::endl;
	std::cout << "There are "
		<< numberOfLines
		<< " lines in the polydata" << std::endl;
	/*
	numberOfLines = stripper->GetOutput()->GetNumberOfLines();
	vtkPoints *points = stripper->GetOutput()->GetPoints();
	vtkCellArray *cells = stripper->GetOutput()->GetLines();

	std::cout << "-----------Lines using vtkStripper" << std::endl;
	std::cout << "There are "
		<< numberOfLines
		<< " lines in the polydata" << std::endl;

	vtkIdType *indices;
	vtkIdType numberOfPoints;
	unsigned int lineCount = 0;
	for (cells->InitTraversal();
		cells->GetNextCell(numberOfPoints, indices);
		lineCount++)
	{
		std::cout << "Line " << lineCount << ": " << std::endl;
		for (vtkIdType i = 0; i < numberOfPoints; i++)
		{
			double point[3];
			points->GetPoint(indices[i], point);
			std::cout << "\t("
				<< point[0] << ", "
				<< point[1] << ", "
				<< point[2] << ")" << std::endl;
		}
	}
	*/

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetPointSize(5);

	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);

	renderWindow->Render();
	renderWindowInteractor->Start();
	
	return EXIT_SUCCESS;
}

//NCC calculation using fft and pure c++
template<class T> int fft_cpp_correlationFilterTemplate(iAConnector* image, iAProgress* p, std::string templateFileName)
{
	/*
	typedef itk::Image<T, DIM> ImageType;
	typedef itk::Image<float, DIM> FloatImageType;

	typename ImageType::Pointer img = dynamic_cast<ImageType *>(image->GetITKImage());

	typename ImageType::SpacingType inImgSpace = img->GetSpacing();
	typename ImageType::SizeType inputSize = img->GetLargestPossibleRegion().GetSize();
	const double origin[3] = { 0, 0, 0 };

	typedef itk::ImageFileReader<FloatImageType> ReaderType;
	typename ReaderType::Pointer reader = ReaderType::New();

	reader->SetFileName(templateFileName);
	reader->Update();

	typename FloatImageType::Pointer template_img = dynamic_cast<FloatImageType*>(reader->GetOutput());

	typedef itk::ImportImageFilter<float, DIM> ImportFilterType;
	typename ImportFilterType::Pointer importer = ImportFilterType::New();

	typedef float PixelType;
	PixelType * outputBuffer = NULL;

	itk::Size<3> sizeTemplateImg = template_img->GetLargestPossibleRegion().GetSize();
	emxArray_real32_T m_template;
	int m_size[3] = { sizeTemplateImg[0], sizeTemplateImg[1], sizeTemplateImg[2] };
	m_template.size = m_size;
	m_template.canFreeData = false;
	m_template.numDimensions = 3;

	m_template.data = reinterpret_cast<float*> (template_img->GetBufferPointer());

	unsigned int m_memAllocSize = m_size[0] * m_size[1] * m_size[2];
	m_template.allocatedSize = m_memAllocSize;

	//***********----------I--------**************
	itk::Size<3> sizeImg = img->GetLargestPossibleRegion().GetSize();
	emxArray_uint16_T m_image;
	int m_sizeI[3] = { sizeImg[0], sizeImg[1], sizeImg[2] };
	m_image.size = m_sizeI;
	m_image.canFreeData = false;
	m_image.numDimensions = 3;

	const unsigned int m_memAllocSizeI = sizeImg[0] * sizeImg[1] * sizeImg[2];
	m_image.allocatedSize = m_memAllocSizeI;
	m_image.data = reinterpret_cast<unsigned short*> (img->GetBufferPointer());

	//***********---------NCC---------**************
	emxArray_real_T m_NCC;
	m_NCC.size = m_sizeI;
	m_NCC.canFreeData = false;
	m_NCC.numDimensions = 3;

	m_NCC.allocatedSize = m_memAllocSizeI;

	std::vector<double> m_dataN(m_memAllocSizeI);
	m_NCC.data = &m_dataN[0];
	//***********------------------**************

	template_matching_gray(&m_template, &m_image, &m_NCC);

	FloatImageType::SizeType nccSize;
	nccSize[0] = sizeImg[0];
	nccSize[1] = sizeImg[1];
	nccSize[2] = sizeImg[2];

	std::cout << "ncc size: " << nccSize[0] << ", " << nccSize[1] << ", " << nccSize[2] << std::endl;

	FloatImageType::IndexType start;
	start.Fill(0);

	FloatImageType::RegionType region;
	region.SetIndex(start);
	region.SetSize(nccSize);

	double NCCorigin[DIM];
	NCCorigin[0] = 0.0;
	NCCorigin[1] = 0.0;
	NCCorigin[2] = 0.0;

	double spacing[DIM];
	spacing[0] = inImgSpace[0];    // along X direction
	spacing[1] = inImgSpace[1];    // along Y direction
	spacing[2] = inImgSpace[2];    // along Z direction

	importer->SetRegion(region);
	importer->SetOrigin(NCCorigin);
	importer->SetSpacing(spacing);

	outputBuffer = new PixelType[m_memAllocSizeI];

	float * it = outputBuffer;
	for (size_t z = 0; z < nccSize[2]; z++)
	{
		for (size_t y = 0; y < nccSize[1]; y++)
		{
			for (size_t x = 0; x < nccSize[0]; x++)
				*it++ = m_NCC.data[x + nccSize[0] * (y + nccSize[1] * z)];
		}
	}

	const bool importImageFilterOwnsBuffer = false;
	importer->SetImportPointer(outputBuffer, m_memAllocSizeI, importImageFilterOwnsBuffer);
	importer->Update();


	image->SetImage(importer->GetOutput());
	image->Modified();

	outputBuffer = NULL;
	*/

	return EXIT_SUCCESS;
}

void iAConvolutionFilter::convolutionFilter()
{
	addMsg(tr("%1  %2 started.").arg(QLocale().toString(Start(), QLocale::ShortFormat))
		.arg(getFilterName()));


	getConnector()->SetImage(getVtkImageData()); getConnector()->Modified();

	try
	{
		iAConnector::ITKScalarPixelType itkType = getConnector()->GetITKScalarPixelType();
		ITK_TYPED_CALL(convolutionFilterTemplate, itkType,
			getConnector(), getItkProgress(), templFileName);
	}
	catch (itk::ExceptionObject &excep)
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

void iAConvolutionFilter::fft_convolutionFilter()
{
	addMsg(tr("%1  %2 started.").arg(QLocale().toString(Start(), QLocale::ShortFormat))
		.arg(getFilterName()));


	getConnector()->SetImage(getVtkImageData()); getConnector()->Modified();

	try
	{
		iAConnector::ITKScalarPixelType itkType = getConnector()->GetITKScalarPixelType();
		ITK_TYPED_CALL(fft_convolutionFilterTemplate, itkType,
			getConnector(), getItkProgress(), templFileName);
	}
	catch (itk::ExceptionObject &excep)
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

void iAConvolutionFilter::correlationFilter()
{
	addMsg(tr("%1  %2 started.").arg(QLocale().toString(Start(), QLocale::ShortFormat))
		.arg(getFilterName()));

	getConnector()->SetImage(getVtkImageData()); 
	getConnector()->Modified();

	try
	{
		iAConnector::ITKScalarPixelType itkType = getConnector()->GetITKScalarPixelType();
		ITK_TYPED_CALL(correlationFilterTemplate, itkType,
			getConnector(), getItkProgress(), templFileName);
	}
	catch (itk::ExceptionObject &excep)
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

void iAConvolutionFilter::fft_correlationFilter()
{
	addMsg(tr("%1  %2 started.").arg(QLocale().toString(Start(), QLocale::ShortFormat))
		.arg(getFilterName()));


	getConnector()->SetImage(getVtkImageData()); getConnector()->Modified();

	try
	{
		switch (getConnector()->GetITKScalarPixelType())
		{
		case itk::ImageIOBase::USHORT:
			streamed_fft_correlationFilterTemplate<unsigned short>(getConnector(), getItkProgress(), templFileName, pData); break;
		case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
		default:
			addMsg(tr(" unknown component type"));
			return;
		}
	}
	catch (itk::ExceptionObject &excep)
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

void iAConvolutionFilter::fft_cpp_correlationFilter()
{
	addMsg(tr("%1  %2 started.").arg(QLocale().toString(Start(), QLocale::ShortFormat))
		.arg(getFilterName()));


	getConnector()->SetImage(getVtkImageData()); getConnector()->Modified();

	try
	{
		iAConnector::ITKScalarPixelType itkType = getConnector()->GetITKScalarPixelType();
		ITK_TYPED_CALL(fft_cpp_correlationFilterTemplate, itkType,
			getConnector(), getItkProgress(), templFileName);
	}
	catch (itk::ExceptionObject &excep)
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

void iAConvolutionFilter::run()
{
	switch (getFilterID())
	{
	case CONVOLUTION_FILTER: 
		convolutionFilter(); break;
	case FFT_CONVOLUTION_FILTER:
		fft_convolutionFilter(); break;
	case CORRELATION_FILTER: 
		correlationFilter(); break; 
	case FFT_CORRELATION_FILTER: 
		fft_correlationFilter(); break; 
	case FFT_NCC_CPP_FILTER: 
		fft_cpp_correlationFilter(); break; 
	default:
		addMsg(tr("unknown filter type"));
	}

}
