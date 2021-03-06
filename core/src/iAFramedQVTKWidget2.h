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
 
#ifndef IAFRAMEDQVTKWIDGET2_H
#define IAFRAMEDQVTKWIDGET2_H

#include <QVTKWidget2.h>

class iAFramedQVTKWidget2 : public QVTKWidget2
{
	Q_OBJECT
public: 
	enum FrameStyle {
		FRAMED,
		NO_FRAME,
		LEFT_SIDE,
	};
public:
	iAFramedQVTKWidget2(QWidget * parent = NULL, const QGLWidget * shareWidget=0, Qt::WindowFlags f = 0);

	void SetFrameStyle(FrameStyle frameStyle);
	FrameStyle GetFrameStyle() const;

	qreal GetFrameWidth() const;
	void SetFrameWidth(qreal newWidth);

	void SetCrossHair(bool enabled);
	
protected slots:
	//overloaded events of QVTKWidget2
	virtual void Frame();

protected:
	qreal m_penWidth;
	FrameStyle m_frameStyle;
	bool m_crossHair;
};
#endif // IAFRAMEDQVTKWIDGET2_H