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
#include "iARenderer.h"

#include "defines.h"
#include "iAChannelID.h"
#include "iAChannelVisualizationData.h"
#include "iAObserverProgress.h"
#include "iARenderObserver.h"
#include "iAWrapperText.h"
#include "iAXmlFiberParser.h"

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkAxesActor.h>
#include <vtkBox.h>
#include <vtkCamera.h>
#include <vtkCameraPass.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkClearZPass.h>
#include <vtkClipVolume.h>
#include <vtkColorTransferFunction.h>
#include <vtkCornerAnnotation.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDecimatePro.h>
#include <vtkDefaultPass.h>
#include <vtkDelaunay2D.h>
#include <vtkElevationFilter.h>
#include <vtkGenericMovieWriter.h>
#include <vtkGeometryFilter.h>
#include <vtkGlyph3D.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageCast.h>
#include <vtkImageExtractComponents.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkLightsPass.h>
#include <vtkLogoRepresentation.h>
#include <vtkLogoWidget.h>
#include <vtkLookupTable.h>
#include <vtkLoopSubdivisionFilter.h> 
#include <vtkObjectFactory.h>
#include <vtkOpaquePass.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkOutlineFilter.h>
#include <vtkOverlayPass.h>
#include <vtkPicker.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPolygon.h>
#include <vtkQImageToImageSource.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderPassCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSequencePass.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkSphereSource.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTranslucentPass.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumetricPass.h>
#include <vtkWindowToImageFilter.h>

#include <QApplication>
#include <QDateTime>
#include <QImage>
#include <QLocale>
#include <QApplication>
#include <QImage>

#ifdef VTK_USE_MPEG2_ENCODER
#include <vtkMPEG2Writer.h>
#endif

#ifdef VTK_USE_OGGTHEORA_ENCODER
#include <vtkOggTheoraWriter.h>
#endif

#ifdef WIN32
#include <vtkAVIWriter.h>
#endif


iARenderer::iARenderer(QObject *par)  :  QThread( par ),
	interactor(0),
	volumeMapper(0),
	imageSampleDistance(-1.0),
	sampleDistance(-1.0),
	m_showMainVolumeWithChannels(true)
{
	multiChannelImageData = vtkImageData::New();

	parent = (QWidget*)par;

	ren = vtkOpenGLRenderer::New();
	labelRen = vtkOpenGLRenderer::New();

	renWin = vtkRenderWindow::New();
	renWin->AlphaBitPlanesOn();
	renWin->LineSmoothingOn();
	renWin->PointSmoothingOn();

	cam = vtkSmartPointer<vtkCamera>::New();

	interactorStyle = vtkInteractorStyleSwitch::New();
	outlineFilter = vtkOutlineFilter::New();
	outlineMapper = vtkPolyDataMapper::New();
	outlineActor = vtkActor::New();

	cSource = vtkCubeSource::New();
	cMapper = vtkPolyDataMapper::New();
	cActor = vtkActor::New();

	rep = vtkLogoRepresentation::New();
	logowidget = vtkLogoWidget::New();
	image1 = vtkQImageToImageSource::New();

	volumeProperty = vtkVolumeProperty::New();

	volume = vtkVolume::New();
	actor2D = vtkActor2D::New();
	textMapper = vtkTextMapper::New();

	polyMapper = vtkPolyDataMapper::New();
	polyActor = vtkActor::New();

	annotatedCubeActor = vtkAnnotatedCubeActor::New();
	axesActor = vtkAxesActor::New();
	moveableAxesActor = vtkAxesActor::New();
	textProperty = vtkTextProperty::New();
	orientationMarkerWidget = vtkOrientationMarkerWidget::New();

	textInfo = iAWrapperText::New();

	pointPicker = vtkPicker::New();
	renderObserver = NULL;
	observerFPProgress = iAObserverProgress::New();
	observerGPUProgress = iAObserverProgress::New();

	plane1 = vtkPlane::New();
	plane2 = vtkPlane::New();
	plane3 = vtkPlane::New();

	sphere = vtkSphereSource::New();
	pickerPGlyphs = vtkGlyph3D::New();
	pickerPPoints = vtkPoints::New();
	pickerPPolyMapper = vtkPolyDataMapper::New();
	pickerPPolyActor = vtkActor::New();
	pickerPPolyData = vtkPolyData::New();
	pickerVGlyphs = vtkGlyph3D::New();
	pickerVPoints = vtkPoints::New();
	pickerVPolyMapper = vtkPolyDataMapper::New();
	pickerVPolyActor = vtkActor::New();
	pickerVPolyData = vtkPolyData::New();

	helperGlyphs = vtkGlyph3D::New();
	helperPoints = vtkPoints::New();
	helperPolyMapper = vtkPolyDataMapper::New();
	helperPolyActor = vtkActor::New();
	helperPolyData = vtkPolyData::New();

	moveableHelperPolyActor = vtkActor::New();

	textInfo->AddToScene(ren);
	textInfo->SetText(" ");
	textInfo->SetPosition(iAWrapperText::POS_LOWER_LEFT);
	textInfo->Show(1);

	cellLocator = vtkCellLocator::New();

	// mobject image members initialize
	volumeHighlight = vtkVolume::New();
	volumePropertyHighlight = vtkVolumeProperty::New();
	volumeMapper = vtkSmartVolumeMapper::New();
	volumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::RayCastRenderMode);

	highlightMode = false;
	meanObjectSelected = false;
	meanObjectHighlighted = false;
}

iARenderer::~iARenderer(void)
{
	ren->RemoveAllObservers();
	renWin->RemoveAllObservers();
	multiChannelImageData->Delete();
	pointPicker->Delete();
	if (renderObserver) renderObserver->Delete();
	observerFPProgress->Delete();
	observerGPUProgress->Delete();

	textInfo->Delete();	

	plane1->Delete();
	plane2->Delete();
	plane3->Delete();

	cSource->Delete();
	cMapper->Delete();
	cActor->Delete();

	sphere->Delete();	
	pickerPGlyphs->Delete();	
	pickerPPoints->Delete();	
	pickerPPolyMapper->Delete();	
	pickerPPolyActor->Delete();	
	pickerPPolyData->Delete();	
	pickerVGlyphs->Delete();	
	pickerVPoints->Delete();	
	pickerVPolyMapper->Delete();	
	pickerVPolyActor->Delete();	
	pickerVPolyData->Delete();	

	helperGlyphs->Delete();	
	helperPoints->Delete();	
	helperPolyMapper->Delete();	
	helperPolyActor->Delete();	
	helperPolyData->Delete();

	moveableHelperPolyActor->Delete();

	orientationMarkerWidget	->Delete();
	textProperty->Delete();
	axesActor->Delete();
	moveableAxesActor->Delete();
	//annotatedCubeActor->Delete(); Load simulation crashes in Release mode but not in Debug mode

	textMapper->ReleaseDataFlagOn();
	textMapper->Delete();
	actor2D->Delete();
	volume->Delete();
	if (volumeMapper != NULL) {
		volumeMapper->ReleaseDataFlagOn();
		volumeMapper->Delete();
	}
	volumeProperty->Delete();

	outlineActor->Delete();
	outlineMapper->ReleaseDataFlagOn();
	outlineMapper->Delete();
	outlineFilter->ReleaseDataFlagOn();
	outlineFilter->Delete();

	interactorStyle->Delete();

	rep->Delete();
	logowidget->Delete();
	image1->Delete();

	renWin->Delete();

	ren->Delete();
	labelRen->Delete();

	//cam->Delete();	// now the camera is vtkSmartPointer
	cellLocator->Delete();
}

void iARenderer::initialize( vtkImageData* ds, vtkPolyData* pd, vtkPiecewiseFunction* otf, 
	vtkColorTransferFunction* ctf, int e )
{
	imageData = ds;
	polyData = pd;
	cellLocator->SetDataSet(polyData);
	if(polyData)
		if( polyData->GetNumberOfCells() )
			cellLocator->BuildLocator();
	piecewiseFunction = otf;
	colorTransferFunction = ctf;
	ext = e;

	double spacing[3];	imageData->GetSpacing(spacing);

	ren->SetLayer(0);
	labelRen->SetLayer(2);
	labelRen->InteractiveOff();
	labelRen->UseDepthPeelingOn();
	renWin->SetNumberOfLayers(3);
	renWin->AddRenderer(ren);
	renWin->AddRenderer(labelRen);
	renWin->LineSmoothingOn();
	pointPicker->SetTolerance(0.00005);//spacing[0]/150);
	interactor = renWin->GetInteractor();
	interactor->SetPicker(pointPicker);
	interactorStyle->SetCurrentStyleToTrackballCamera();
	interactor->SetInteractorStyle(interactorStyle);
	InitObserver();

	QImage img;
	if( QDate::currentDate().dayOfYear() >= 340 )img.load(":/images/Xmas.png");
	else img.load(":/images/fhlogo.png");

	image1->SetQImage(&img);
	image1->Update();
	rep->SetImage(image1->GetOutput( ));
	logowidget->SetInteractor(interactor);
	logowidget->SetRepresentation(rep);
	logowidget->SetResizable(false);
	logowidget->SetSelectable(false);
	logowidget->On();

	interactor->Initialize();

	// setup cube source
	cSource->SetXLength(ext * spacing[0]);
	cSource->SetYLength(ext * spacing[1]);
	cSource->SetZLength(ext * spacing[2]);
	cMapper->SetInputConnection(cSource->GetOutputPort());
	cActor->SetMapper(cMapper);
	cActor->GetProperty()->SetColor(1,0,0);

	setupCutter();
	setupTxt();
	setupHelper();
	setupCube();
	setupAxes(spacing);
	setupPickerGlyphs();
	setupOrientationMarker();
	setupRenderer();

	labelRen->SetActiveCamera(cam);
	ren->SetActiveCamera(cam);
	setCamPosition( 0,0,1, 1,1,1 ); // +Z
}

void iARenderer::reInitialize( vtkImageData* ds, vtkPolyData* pd, vtkPiecewiseFunction* otf, 
	vtkColorTransferFunction* ctf, int e )
{
	imageData = ds;
	polyData = pd;
	if( polyData )
	{
		cellLocator->SetDataSet( polyData );
		if( polyData->GetNumberOfCells() )
			cellLocator->BuildLocator();
	}
	piecewiseFunction = otf;
	colorTransferFunction = ctf;
	ext = e;

	volumeMapper->SetInputData(imageData);
	outlineFilter->SetInputData(imageData);
	polyMapper->SetInputData(polyData);

	volumeProperty->SetColor(0, colorTransferFunction);
	volumeProperty->SetScalarOpacity(0, piecewiseFunction);

	renderObserver->ReInitialize( ren, labelRen, interactor, pointPicker,
		axesTransform, imageData,
		plane1, plane2, plane3, cellLocator );
	interactor->ReInitialize();

	emit reInitialized();

	update();

	//setCamPosition( 0,0,1, 1,1,1 ); // +Z
}


void iARenderer::setTransferFunctions( vtkPiecewiseFunction* opacityTFHighlight, vtkColorTransferFunction* colorTFHighlight, vtkPiecewiseFunction* opacityTFTransparent, vtkColorTransferFunction* colorTFTransparent )
{
	piecewiseFunctionHighlight = opacityTFHighlight;
	colorTransferFunctionHighlight = colorTFHighlight;
	piecewiseFunctionTransparent = opacityTFTransparent;
	colorTransferFunctionTransparent = colorTFTransparent;

	update();
}



void iARenderer::initializeHighlight( vtkImageData* ds, vtkPiecewiseFunction* otfHighlight, vtkColorTransferFunction* ctfHighlight, vtkPiecewiseFunction* otf, vtkColorTransferFunction* ctf )
{
	imageDataHighlight = ds;
	piecewiseFunctionHighlight = otfHighlight;
	colorTransferFunctionHighlight = ctfHighlight;

	piecewiseFunction = otf;
	colorTransferFunction = ctf;

	highlightMode = true;

	volumeProperty->SetColor(0, colorTransferFunction);
	volumeProperty->SetScalarOpacity(0, piecewiseFunction);

	//volumePropertyHighlight->DeepCopy(volumeProperty);

	volumePropertyHighlight->SetColor(colorTransferFunctionHighlight);
	volumePropertyHighlight->SetScalarOpacity(piecewiseFunctionHighlight);
	volumePropertyHighlight->SetAmbient(volumeProperty->GetAmbient());
	volumePropertyHighlight->SetDiffuse(volumeProperty->GetDiffuse());
	volumePropertyHighlight->SetSpecular(volumeProperty->GetSpecular());
	volumePropertyHighlight->SetSpecularPower(volumeProperty->GetSpecularPower());
	volumePropertyHighlight->SetInterpolationType(volumeProperty->GetInterpolationType()); 
	volumePropertyHighlight->SetShade(volumeProperty->GetShade());

	volumeMapper->SetInputData(imageDataHighlight);
	volumeMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsProgressEvent, this->observerFPProgress);
	volumeHighlight->SetMapper(volumeMapper);
	volumeHighlight->SetProperty(volumePropertyHighlight);

	volumeHighlight->SetPosition((imageData->GetDimensions()[0]-imageDataHighlight->GetDimensions()[0])/2*10-10,
		(imageData->GetDimensions()[1]-imageDataHighlight->GetDimensions()[1])/2*10-10,
		(imageData->GetDimensions()[2]-imageDataHighlight->GetDimensions()[2])/2*10-10);

	volumeHighlight->Update();

	ren->AddVolume(volumeHighlight);

	update();
}

void iARenderer::reInitializeHighlight( vtkImageData* ds, vtkPiecewiseFunction* otf, vtkColorTransferFunction* ctf )
{
	highlightMode = true;

	imageDataHighlight = ds;
	piecewiseFunctionHighlight = otf;
	colorTransferFunctionHighlight = ctf;

	volumePropertyHighlight->SetColor(colorTransferFunctionHighlight);
	volumePropertyHighlight->SetScalarOpacity(piecewiseFunctionHighlight);

	volumeMapper->SetInputData(imageDataHighlight);
	volumeMapper->SetInputData(imageDataHighlight);
	volumeHighlight->SetMapper(volumeMapper);
	volumeHighlight->SetProperty(volumePropertyHighlight);

	ren->AddVolume(volumeHighlight);

	update();
}

void iARenderer::visualizeHighlight( bool enabled )
{
	if(enabled)
	{
		ren->AddVolume(volumeHighlight);
	}
	else
	{
		ren->RemoveVolume(volumeHighlight);
	}
}

namespace
{
	bool hasActiveChannel(std::set<iAChannelVisualizationData*> const & channels)
	{
		for (std::set<iAChannelVisualizationData* >::iterator it = channels.begin();
			it != channels.end();
			++it)
		{
			if ((*it)->IsEnabled())
			{
				return true;
			}
		}
		return false;
	}
}

void iARenderer::updateChannelImages() //add active channel images as additional components to the image data
{
	if (!hasActiveChannel(m_channels))
	{
		// in case we don't have any additional channels:
		setInputVolume(imageData);
		return;
	}

	vtkSmartPointer<vtkImageAppendComponents> append = vtkSmartPointer<vtkImageAppendComponents>::New();

	if(m_showMainVolumeWithChannels) {
		append->SetInputData(imageData);
	}

	for (std::set<iAChannelVisualizationData*>::iterator it = m_channels.begin();
		it != m_channels.end();
		++it)
	{
		if ((*it)->IsEnabled())
		{
			//we cast channel image to the scalar type of image data
			vtkSmartPointer<vtkImageCast> caster = vtkSmartPointer<vtkImageCast>::New();
			caster->SetInputData((*it)->GetActiveImage());
			caster->SetOutputScalarType(imageData->GetScalarType());
			//we upscale the channel image to the size of image data
			vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
			reslice->SetInterpolationModeToCubic();
			reslice->SetInformationInput(imageData);
			reslice->SetInputConnection(0, caster->GetOutputPort());
			// and append them to image data
			reslice->Update();
			append->AddInputData(reslice->GetOutput());
		}
	}
	append->Update();
	multiChannelImageData->DeepCopy(append->GetOutput());

	int renderedChannel = 0;
	for (std::set<iAChannelVisualizationData* >::iterator it = m_channels.begin();
		it != m_channels.end();
		++it)
	{
		if ((*it)->IsEnabled())
		{
			int renChan = renderedChannel + m_showMainVolumeWithChannels ? 0 : 1;
			vtkColorTransferFunction* ctf = dynamic_cast<vtkColorTransferFunction*> ( (*it)->GetCTF() );
			assert(ctf);
			volumeProperty->SetColor( renChan, ctf);
			volumeProperty->SetScalarOpacity( renChan, (*it)->GetOTF() );
			renderedChannel++;
		}
	}
	volume->SetProperty(volumeProperty);

	setInputVolume(multiChannelImageData);
}

void iARenderer::addChannel(iAChannelVisualizationData * chData)
{
	if (!chData ||
		m_channels.size() == iAChannelVisualizationData::Maximum3DChannels)
	{
		// show warning?
		return;
	}
	m_channels.insert(chData);
}


void iARenderer::removeChannel(iAChannelVisualizationData * chData)
{
	std::set<iAChannelVisualizationData*>::const_iterator it = m_channels.find(chData);
	if (it != m_channels.end())
	{
		m_channels.erase(it);
		updateChannelImages();
	}
}


void iARenderer::showMainVolumeWithChannels(bool show)
{
	m_showMainVolumeWithChannels = show;
	updateChannelImages();
}

void iARenderer::initializePasses()
{
	// initialize passes
	vtkCameraPass* cameraPass			= vtkCameraPass::New();
	vtkClearZPass* clearZPass			= vtkClearZPass::New();
	vtkDefaultPass* defaultPas			= vtkDefaultPass::New();
	vtkSequencePass* renderPass			= vtkSequencePass::New();
	vtkOpaquePass* opaquePass			= vtkOpaquePass::New();
	vtkTranslucentPass* translucentPass	= vtkTranslucentPass::New();
	vtkVolumetricPass* volumePass		= vtkVolumetricPass::New();
	vtkOverlayPass* overlayPass			= vtkOverlayPass::New();
	vtkLightsPass* lightsPass			= vtkLightsPass::New();
	vtkRenderPassCollection* passes		= vtkRenderPassCollection::New();

	passes->AddItem(clearZPass);
	passes->AddItem(lightsPass);
	passes->AddItem(defaultPas);
	passes->AddItem(volumePass);
	passes->AddItem(translucentPass);
	passes->AddItem(opaquePass);
	passes->AddItem(overlayPass);

	renderPass->SetPasses(passes);
	cameraPass->SetDelegatePass(renderPass);

	// attach pass to renderer
	ren->SetPass(cameraPass);

	// delete passes
	opaquePass->Delete();
	volumePass->Delete();
	overlayPass->Delete();
	renderPass->Delete();
	passes->Delete();
	cameraPass->Delete();
	lightsPass->Delete();
	clearZPass->Delete();
	defaultPas->Delete();
	translucentPass->Delete();
}

void iARenderer::run()
{
	qApp->processEvents();

	this->update();
}


void iARenderer::showSlicers( bool s ) 
{
	if (s) {
		volumeMapper->AddClippingPlane(plane1);
		volumeMapper->AddClippingPlane(plane2);
		volumeMapper->AddClippingPlane(plane3);
	} else {
		volumeMapper->RemoveAllClippingPlanes();
	}
}

void iARenderer::showSlicers( bool showPlane1, bool showPlane2, bool showPlane3 ) 
{
	volumeMapper->RemoveAllClippingPlanes();

	if (showPlane1) {
		volumeMapper->AddClippingPlane(plane1);
	} else {
		volumeMapper->RemoveClippingPlane(plane1);
	}

	if (showPlane2) {
		volumeMapper->AddClippingPlane(plane2);
	} else {
		volumeMapper->RemoveClippingPlane(plane2);
	}

	if (showPlane3) {
		volumeMapper->AddClippingPlane(plane3);
	} else {
		volumeMapper->RemoveClippingPlane(plane3);
	}
}

void iARenderer::setupCutter()
{
	plane1->SetNormal(1, 0, 0);
	plane2->SetNormal(0, 1, 0);
	plane3->SetNormal(0, 0, 1);
}

void iARenderer::setupTxt()
{
	textProperty->SetBold(0);
	textProperty->SetItalic(0);
	textProperty->SetFontSize(11);
	textProperty->SetFontFamily(VTK_COURIER);
	textProperty->SetJustification(VTK_TEXT_CENTERED);
	textProperty->SetVerticalJustification(VTK_TEXT_CENTERED);

	textMapper->SetTextProperty(textProperty);
	actor2D->SetMapper(textMapper);
}

void iARenderer::setupHelper()
{
	sphere->Update();
	helperGlyphs->SetSourceConnection( sphere->GetOutputPort() );
	helperGlyphs->SetColorModeToColorByScale();
	helperGlyphs->SetVectorModeToUseNormal();
	helperGlyphs->SetScaleFactor(1.5);
	helperGlyphs->SetScaleModeToDataScalingOff();

	helperPoints->InsertNextPoint(0,0,0);

	helperPolyData->SetPoints(helperPoints);
	helperGlyphs->SetInputData(helperPolyData);
	helperPolyMapper->SetInputConnection(helperGlyphs->GetOutputPort());
	helperPolyActor->SetMapper(helperPolyMapper);
	helperPolyActor->GetProperty()->SetColor(0.1,0.1,0.1);

	moveableHelperPolyActor->SetMapper(helperPolyMapper);
	moveableHelperPolyActor->GetProperty()->SetColor(0.1,0.1,0.1);
}

void iARenderer::setupCube()
{
	annotatedCubeActor->SetPickable(1);
	annotatedCubeActor->SetXPlusFaceText("+X");
	annotatedCubeActor->SetXMinusFaceText("-X");
	annotatedCubeActor->SetYPlusFaceText("+Y");
	annotatedCubeActor->SetYMinusFaceText("-Y");
	annotatedCubeActor->SetZPlusFaceText("+Z");
	annotatedCubeActor->SetZMinusFaceText("-Z");
	annotatedCubeActor->SetXFaceTextRotation(0);
	annotatedCubeActor->SetYFaceTextRotation(0);
	annotatedCubeActor->SetZFaceTextRotation(90);
	annotatedCubeActor->SetFaceTextScale(0.45);
	annotatedCubeActor->GetCubeProperty()->SetColor(0.7, 0.78, 1);
	annotatedCubeActor->GetTextEdgesProperty()->SetDiffuse(0);
	annotatedCubeActor->GetTextEdgesProperty()->SetAmbient(0);
	annotatedCubeActor->GetXPlusFaceProperty()->SetColor(1, 0, 0);
	annotatedCubeActor->GetXPlusFaceProperty()->SetInterpolationToFlat();
	annotatedCubeActor->GetXMinusFaceProperty()->SetColor(1, 0, 0);
	annotatedCubeActor->GetXMinusFaceProperty()->SetInterpolationToFlat();
	annotatedCubeActor->GetYPlusFaceProperty()->SetColor(0, 1, 0);
	annotatedCubeActor->GetYPlusFaceProperty()->SetInterpolationToFlat();
	annotatedCubeActor->GetYMinusFaceProperty()->SetColor(0, 1, 0);
	annotatedCubeActor->GetYMinusFaceProperty()->SetInterpolationToFlat();
	annotatedCubeActor->GetZPlusFaceProperty()->SetColor(0, 0, 1);
	annotatedCubeActor->GetZPlusFaceProperty()->SetInterpolationToFlat();
	annotatedCubeActor->GetZMinusFaceProperty()->SetColor(0, 0, 1);
	annotatedCubeActor->GetZMinusFaceProperty()->SetInterpolationToFlat();
}

void iARenderer::setupAxes(double spacing[3])
{
	axesActor->AxisLabelsOff();
	axesActor->SetShaftTypeToCylinder();
	axesActor->SetTotalLength(15, 15, 15);

	vtkTransform *transform = vtkTransform::New();
	transform->Scale(spacing[0]*3, spacing[1]*3, spacing[2]*3);

	axesActor->SetUserTransform(transform);
	helperPolyActor->SetUserTransform(transform);
	transform->Delete();

	moveableAxesActor->AxisLabelsOff();
	moveableAxesActor->SetShaftTypeToCylinder();
	moveableAxesActor->SetTotalLength(15, 15, 15);

	axesTransform->Scale(spacing[0]*3, spacing[1]*3, spacing[2]*3);

	moveableAxesActor->SetUserTransform(axesTransform);
	moveableHelperPolyActor->SetUserTransform(axesTransform);
}

void iARenderer::setupOrientationMarker()
{
	orientationMarkerWidget->SetOrientationMarker(annotatedCubeActor);
	orientationMarkerWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
	orientationMarkerWidget->SetInteractor(interactor);
	orientationMarkerWidget->SetEnabled( 1 );
	orientationMarkerWidget->InteractiveOff();
}

void iARenderer::hideOrientationMarker()
{
	orientationMarkerWidget->SetEnabled(false);
}

void iARenderer::setupPickerGlyphs()
{
	sphere->Update();
	pickerPGlyphs->SetSourceConnection( sphere->GetOutputPort() );
	pickerPGlyphs->SetColorModeToColorByScale();
	pickerPGlyphs->SetVectorModeToUseNormal();
	pickerPGlyphs->SetScaleFactor(1.5);
	pickerPGlyphs->SetScaleModeToDataScalingOff();

	sphere->Update();
	pickerVGlyphs->SetSourceConnection( sphere->GetOutputPort() );
	pickerVGlyphs->SetColorModeToColorByScale();
	pickerVGlyphs->SetVectorModeToUseNormal();
	pickerVGlyphs->SetScaleFactor(1.5);
	pickerVGlyphs->SetScaleModeToDataScalingOff();
}

void iARenderer::setupRenderer()
{
	outlineFilter->SetInputData(imageData);
	outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());
	//outlineActor->GetProperty()->SetLineWidth(0.5);
	outlineActor->GetProperty()->SetColor(0,0,0);
	outlineActor->PickableOff();
	outlineActor->SetMapper(outlineMapper);

	polyMapper->SetInputData(polyData);
	polyMapper->SelectColorArray("Colors");
	polyMapper->SetScalarModeToUsePointFieldData();
	polyActor->SetMapper(polyMapper);

	pickerPPolyData->SetPoints(pickerPPoints);
	pickerPGlyphs->SetInputData(pickerPPolyData);
	pickerPPolyMapper->SetInputConnection(pickerPGlyphs->GetOutputPort());
	pickerPPolyMapper->ScalarVisibilityOff();
	pickerPPolyActor->SetMapper(pickerPPolyMapper);
	pickerPPolyActor->GetProperty()->SetColor(0,0,1);

	pickerVPolyData->SetPoints(pickerVPoints);
	pickerVGlyphs->SetInputData(pickerVPolyData);
	pickerVPolyMapper->SetInputConnection(pickerVGlyphs->GetOutputPort());
	pickerVPolyMapper->ScalarVisibilityOff();
	pickerVPolyActor->SetMapper(pickerVPolyMapper);
	pickerVPolyActor->GetProperty()->SetColor(1,0,0);

	volumeProperty->SetColor(0, colorTransferFunction);
	volumeProperty->SetScalarOpacity(0, piecewiseFunction);

	getNewVolumeMapper(imageData);

	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);
	volume->SetVisibility(false);

	ren->GradientBackgroundOn();
	ren->AddVolume(volume);
	ren->AddActor(polyActor);
	ren->AddActor(pickerPPolyActor);
	ren->AddActor(pickerVPolyActor);
	ren->AddActor(cActor);
	ren->AddActor(actor2D);
	ren->AddActor(helperPolyActor);
	ren->AddActor(moveableHelperPolyActor);
	ren->AddActor(axesActor);
	ren->AddActor(moveableAxesActor);
	ren->AddActor(outlineActor);

	//ren->SetBackground(1.0, 1.0, 1.0);
	//ren->SetBackground2(0.5, 0.66666666666666666666666666666667, 1.0);
	emit onSetupRenderer();
}

void iARenderer::reset(double imageSampleDistance, double sampleDistance)
{
	this->imageSampleDistance = imageSampleDistance;
	this->sampleDistance = sampleDistance;
	recreateMapper(imageData);
	volume->SetProperty(volumeProperty);
	volume->Update();
}

void iARenderer::update()
{
	pickerPGlyphs->Update();
	pickerVGlyphs->Update();
	volume->Update();
	volumeMapper->Update();
	polyMapper->Update();

	renWin->Render();
}

void iARenderer::showHelpers(bool show)
{
	orientationMarkerWidget->SetEnabled(show);
	helperPolyActor->SetVisibility(show),
		axesActor->SetVisibility(show);
	moveableHelperPolyActor->SetVisibility(show);
	moveableAxesActor->SetVisibility(show);
	logowidget->SetEnabled(show);
	cActor->SetVisibility(show);
}

void iARenderer::showRPosition(bool s) 
{ 
	cActor->SetVisibility(s); 
}

void iARenderer::SetRenderMode(int mode)
{
	volumeMapper->SetRequestedRenderMode(mode);
	volumeMapper->InteractiveAdjustSampleDistancesOff();
}

void iARenderer::setPlaneNormals( vtkTransform *tr ) 
{ 
	double normal[4], temp[4];

	normal[0] = 1; normal[1] = 0; normal[2] = 0; normal[3] = 1;
	tr->GetMatrix()->MultiplyPoint(normal, temp); 
	plane1->SetNormal( temp[0], temp[1], temp[2] ); 

	normal[0] = 0; normal[1] = 1; normal[2] = 0; normal[3] = 1;
	tr->GetMatrix()->MultiplyPoint(normal, temp); 
	plane2->SetNormal( temp[0], temp[1], temp[2] ); 

	normal[0] = 0; normal[1] = 0; normal[2] = 1; normal[3] = 1;
	tr->GetMatrix()->MultiplyPoint(normal, temp); 
	plane3->SetNormal( temp[0], temp[1], temp[2] ); 

	renWin->Render();
	ren->Render();
};

void iARenderer::setCubeCenter( int x, int y, int z )
{
	if (interactor->GetEnabled()) {
		cSource->SetCenter( x * imageData->GetSpacing()[0], 
			y * imageData->GetSpacing()[1], 
			z * imageData->GetSpacing()[2] );
		update();
	}
};

void iARenderer::setCamPosition( int uvx, int uvy, int uvz, int px, int py, int pz )
{
	cam->SetViewUp ( uvx, uvy, uvz );
	cam->SetPosition ( px, py, pz );
	cam->SetFocalPoint( 0,0,0 );
	ren->ResetCamera();
	update();
}


void iARenderer::getCamPosition( double * camOptions )
{
	double pS = cam->GetParallelScale();
	double a[3] = {0};
	double b[3] = {0};
	double c[3] = {0};
	cam->GetViewUp(a);
	cam->GetPosition(b);
	cam->GetFocalPoint(c);

	camOptions[0] = a[0];
	camOptions[1] = a[1];
	camOptions[2] = a[2];
	camOptions[3] = b[0];
	camOptions[4] = b[1];
	camOptions[5] = b[2];
	camOptions[6] = c[0];
	camOptions[7] = c[1];
	camOptions[8] = c[2];
	camOptions[9] = pS;
	
	ren->ResetCamera();
	update();
}


void iARenderer::setCamPosition( double * camOptions, bool rsParallelProjection )
{
	cam->SetViewUp ( camOptions[0], camOptions[1], camOptions[2] );
	cam->SetPosition ( camOptions[3], camOptions[4], camOptions[5] );
	cam->SetFocalPoint( camOptions[6], camOptions[7], camOptions[8] );

	if(rsParallelProjection)
	cam->SetParallelScale( camOptions[9] );

	update();
}


void iARenderer::setCamera(vtkCamera* c)
{
	cam = c;
	labelRen->SetActiveCamera(cam);
	ren->SetActiveCamera(cam);
	emit onSetCamera();
}


vtkCamera* iARenderer::getCamera()
{
	return cam;
}


void iARenderer::saveMovie( const QString& fileName, int mode, int qual /*= 2*/ )
{
	vtkSmartPointer<vtkGenericMovieWriter> movieWriter;

	// Try to create proper video encoder based on given file name.

#ifdef VTK_USE_MPEG2_ENCODER
	if (fileName.endsWith(".mpeg")){
		vtkSmartPointer<vtkMPEG2Writer> mpegwriter;
		mpegwriter = vtkSmartPointer<vtkMPEG2Writer>::New();
		movieWriter = mpegwriter;
	}
#endif

#ifdef VTK_USE_OGGTHEORA_ENCODER
	if (fileName.endsWith(".ogv")) {
		vtkSmartPointer<vtkOggTheoraWriter> oggwriter;
		oggwriter = vtkSmartPointer<vtkOggTheoraWriter>::New();
		oggwriter->SetQuality(qual);
		movieWriter = oggwriter;
	}
#endif

#ifdef WIN32
	vtkSmartPointer<vtkAVIWriter> aviwriter;
	if (fileName.endsWith(".avi")){
		aviwriter = vtkSmartPointer<vtkAVIWriter>::New();
		aviwriter->SetCompressorFourCC("XVID");
		aviwriter->PromptCompressionOptionsOn();
		aviwriter->SetQuality(qual);
		movieWriter = aviwriter;
	}
#endif

	if (movieWriter.GetPointer() == NULL)
		return;

	interactor->Disable();

	vtkSmartPointer<vtkWindowToImageFilter> w2if = vtkSmartPointer<vtkWindowToImageFilter>::New();
	int* rws = renWin->GetSize();
	if (rws[0] % 2 != 0) rws[0]++;
	if (rws[1] % 2 != 0) rws[1]++;
	renWin->SetSize(rws);
	renWin->Render();

	w2if->SetInput(renWin);
	w2if->ReadFrontBufferOff();

	movieWriter->SetInputConnection(w2if->GetOutputPort());
	movieWriter->SetFileName(fileName.toLatin1().data());
	movieWriter->Start();

	int i;
	int* extent = imageData->GetExtent();

	emit msg(tr("%1  MOVIE export started. Output: %2").arg(QLocale().toString(QDateTime::currentDateTime(), QLocale::ShortFormat), fileName));

	int numRenderings = 360;//TODO
	vtkSmartPointer<vtkTransform> rot = vtkSmartPointer<vtkTransform>::New();
	cam->SetFocalPoint( 0,0,0 );
	double view[3];
	double point[3];
	if (mode == 0) { // YZ
		double _view[3]  = { 0 ,0, -1 };
		double _point[3] = { 1, 0, 0 };
		for (int ind=0; ind<3; ind++)
		{
			view[ind] = _view[ind];
			point[ind] = _point[ind];
		}
		rot->RotateZ(360/numRenderings);
	} else if (mode == 1) { // XY
		double _view[3]  = { 0, 0, -1 };
		double _point[3] = { 0, 1, 0 };
		for (int ind=0; ind<3; ind++)
		{
			view[ind] = _view[ind];
			point[ind] = _point[ind];
		}
		rot->RotateX(360/numRenderings);
	} else if (mode == 2) { // XZ
		double _view[3]  = { 0, 1, 0 };
		double _point[3] = { 0, 0, 1 };
		for (int ind=0; ind<3; ind++)
		{
			view[ind] = _view[ind];
			point[ind] = _point[ind];
		}
		rot->RotateY(360/numRenderings);
	}
	cam->SetViewUp ( view );
	cam->SetPosition ( point );
	for ( i =0; i < numRenderings; i++ ) {
		ren->ResetCamera();
		renWin->Render();

		w2if->Modified();
		movieWriter->Write();
		if (movieWriter->GetError()) { 
			emit msg(movieWriter->GetStringFromErrorCode(movieWriter->GetErrorCode())); 
			break;
		}
		emit progress( 100 * (i+1) / (extent[1]-extent[0]));
		cam->ApplyTransform(rot);
	}

	movieWriter->End(); 
	movieWriter->ReleaseDataFlagOn();
	w2if->ReleaseDataFlagOn();

	interactor->Enable();

	if (movieWriter->GetError()) emit msg(tr("  MOVIE export failed."));
	else emit msg(tr("  MOVIE export completed."));

	return;
}

void iARenderer::mouseRightButtonReleasedSlot()
{
	if (!interactor)
		return;
	interactor->InvokeEvent(vtkCommand::RightButtonReleaseEvent);
}

void iARenderer::mouseLeftButtonReleasedSlot()
{
	if (!interactor)
		return;
	interactor->InvokeEvent(vtkCommand::LeftButtonReleaseEvent);
}

void iARenderer::setInputVolume(vtkImageData* imageData)
{
	// workaround for vtk 6.1 problem (bug?)
	// where an access violation happens if input image to mapper is changed
	// to one with a higher number of components
	recreateMapper(imageData);
}


void iARenderer::getNewVolumeMapper(vtkImageData* imageData)
{
	volumeMapper = vtkSmartVolumeMapper::New();
	volumeMapper->SetBlendModeToComposite(); // composite first
	volumeMapper->SetInputData(imageData);
	volumeMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsProgressEvent, this->observerFPProgress);
}

void iARenderer::recreateMapper(vtkImageData* imageData)
{
	volumeMapper->ReleaseDataFlagOn();
	volumeMapper->Delete();
	getNewVolumeMapper(imageData);
	volume->SetMapper(volumeMapper);
	volumeMapper->Update();
}

void iARenderer::setImageSampleDistance(double imageSampleDistance)
{
	this->imageSampleDistance = imageSampleDistance;
	// SetImageSampleDistance function not available in vtkSmartVolumeMapper
	// volumeMapper->SetImageSampleDistance(sampleDistance);
}

void iARenderer::setSampleDistance(double sampleDistance)
{
	this->sampleDistance = sampleDistance;
	// SetSampleDistance function only exists in vtkSmartVolumeMapper when OpenGL2 backend is selected!
	 volumeMapper->SetSampleDistance(sampleDistance);
}

void iARenderer::InitObserver()
{
	renderObserver = RenderObserver::New(ren, labelRen, interactor, pointPicker,
		axesTransform, imageData,
		plane1, plane2, plane3, cellLocator);

	interactor->AddObserver(vtkCommand::KeyPressEvent, renderObserver);
	interactor->AddObserver(vtkCommand::LeftButtonPressEvent, renderObserver);
	//There is a VTK bug, observer does not catch mouse release events!
	// workaround using QVTKWidgetMouseReleaseWorkaround used
	interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, renderObserver);
	interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, renderObserver);
	interactor->AddObserver(vtkCommand::RightButtonReleaseEvent, renderObserver);
}

void iARenderer::setPolyData(vtkPolyData* pd)
{
	polyData = pd;
	if (polyData)
	{
		cellLocator->SetDataSet(polyData);
		if (polyData->GetNumberOfCells())
			cellLocator->BuildLocator();
	}
	polyMapper->SetInputData( polyData );
}

vtkPolyData* iARenderer::getPolyData()
{
	return polyData;
}

void iARenderer::parallelProjection(bool b)
{
	ren->GetActiveCamera()->SetParallelProjection(b);
}

void iARenderer::shade(bool b) { volumeProperty->SetShade(b); };
void iARenderer::interpolationType(int val) { volumeProperty->SetInterpolationType(val); };
void iARenderer::ambient(double val) { volumeProperty->SetAmbient(val); };
void iARenderer::diffuse(double val) { volumeProperty->SetDiffuse(val); };
void iARenderer::specular(double val) { volumeProperty->SetSpecular(val); };
void iARenderer::specularPower(double val) { volumeProperty->SetSpecularPower(val); };
void iARenderer::color(vtkColorTransferFunction* TF) { volumeProperty->SetColor(TF); };
void iARenderer::scalarOpacity(vtkPiecewiseFunction* TF) { volumeProperty->SetScalarOpacity(TF); };
void iARenderer::setTransferFunctionToHighlight() { volumeProperty->SetColor(colorTransferFunctionHighlight); volumeProperty->SetScalarOpacity(piecewiseFunctionHighlight); update(); };
void iARenderer::setTransferFunctionToTransparent() { volumeProperty->SetColor(colorTransferFunctionTransparent); volumeProperty->SetScalarOpacity(piecewiseFunctionTransparent); update(); };

void iARenderer::visibility(bool b) { volume->SetVisibility(b); };
void iARenderer::disableInteractor() { interactor->Disable(); disabled = true; }
void iARenderer::enableInteractor() { interactor->ReInitialize(); disabled = false; }

vtkTransform* iARenderer::getCoordinateSystemTransform() { axesTransform->Update(); return axesTransform; }
void iARenderer::GetImageDataBounds(double bounds[6]) { imageData->GetBounds(bounds); }


void iARenderer::AddRenderer(vtkRenderer* renderer)
{
	renWin->AddRenderer(renderer);
}
