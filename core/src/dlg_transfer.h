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
 
#ifndef DLG_TRANSFER_H
#define DLG_TRANSFER_H

#include "dlg_function.h"
#include "open_iA_Core_export.h"
#include "iATransferFunction.h"

#include <QLinearGradient>

class QColorDialog;
class QDomNode;

class vtkPiecewiseFunction;
class vtkColorTransferFunction;

class iAFunctionChangeListener;

class open_iA_Core_API dlg_transfer : public dlg_function, public TransferFunction
{
	int selectedPoint;

	QColor          color;
	QColorDialog    *dlg;
	QLinearGradient gradient;
		
	vtkPiecewiseFunction     *opacityTF;
	vtkColorTransferFunction *colorTF;

	iAFunctionChangeListener * m_changeListener;

public:
	dlg_transfer(iADiagramFctWidget *histogram, QColor color);
	~dlg_transfer();

	int getType() { return TRANSFER; }
	
	// abstract functions
	void draw(QPainter &painter);
	void draw(QPainter &painter, QColor color, int lineWidth);
	void drawOnTop(QPainter &painter);

	int selectPoint(QMouseEvent *event, int *x = NULL);
	int getSelectedPoint() { return selectedPoint; }
	int addPoint(int x, int y);
	void addColorPoint(int x, double red = -1.0, double green = -1.0, double blue = -1.0);
	void removePoint(int index);
	void moveSelectedPoint(int x, int y);
	void changeColor(QMouseEvent *event);
	void enableRangeSliderHandles( bool rangeSliderHandles );

	bool isColored() { return true; }
	bool isEndPoint(int index);
	bool isDeletable(int index);

	void reset();
	void update(double oldDataRange[2]);
	
	void mousePressEvent(QMouseEvent*)   {}
	void mouseMoveEvent(QMouseEvent*)    {}
	void mouseReleaseEvent(QMouseEvent*) {}
	void mouseReleaseEventAfterNewPoint(QMouseEvent *event);
	
	// additional public functions
	void setOpacityFunction(vtkPiecewiseFunction *opacityTF) { this->opacityTF = opacityTF; }
	void setColorFunction(vtkColorTransferFunction *colorTF) { this->colorTF = colorTF; }

	vtkPiecewiseFunction* getOpacityFunction() { return opacityTF; }
	vtkColorTransferFunction* getColorFunction() { return colorTF; }

	void setChangeListener(iAFunctionChangeListener* listener);

	// TODO: remov!
	void loadTransferFunction(QDomNode &functionsNode, double range[2]);
	
private:
	void setColorPoint(int selectedPoint, double x, double red, double green, double blue);
	void setColorPoint(int selectedPoint, int x, double red, double green, double blue);
	void setColorPoint(int selectedPoint, int x);
	void setPoint(int selectedPoint, int x, int y);
	void setPointX(int selectedPoint, int x);
	void setPointY(int selectedPoint, int y);
	
	// convert view to data
	double v2dX(int x);
	double v2dY(int y);

	// conver data to view
	int d2vX(double x, double oldDataRange0 = -1, double oldDataRange1 = -1);
	int d2vY(double y);

	//convert data to image
	int d2iX(double x);
	int d2iY(double y);

	void triggerOnChange();

	bool m_rangeSliderHandles;
};

#endif
