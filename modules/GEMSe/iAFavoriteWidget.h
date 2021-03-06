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
 
#ifndef IA_FAVORITE_WIDGET_H
#define IA_FAVORITE_WIDGET_H

#include "iASlicerMode.h"
#include "iAImageTree.h"

#include <QVector>
//#include <QSharedPointer>
#include <QWidget>

class iAImagePreviewWidget;
class iAPreviewWidgetPool;

class vtkCamera;

struct FavoriteData
{
	iAImageClusterNode * node;
	iAImagePreviewWidget* widget;
	FavoriteData():
		node(0), widget(0) {}
	FavoriteData(iAImageClusterNode *l, iAImagePreviewWidget* w):
		node(l), widget(w) {}
};

class iAFavoriteWidget : public QWidget
{
	Q_OBJECT
public:
	iAFavoriteWidget(iAPreviewWidgetPool* previewWidgetPool);
	bool ToggleLike(iAImageClusterNode * leaf);
	bool ToggleHate(iAImageClusterNode * leaf);
	bool HasAnyFavorite() const;
	QVector<iAImageClusterNode const *> GetFavorites(iAImageClusterNode::Attitude att) const;
signals:
	void ViewUpdated();
	void Clicked(iAImageClusterNode * leaf);
private slots:
	void FavoriteClicked();
private:
	void Add(iAImageClusterNode * leaf);
	void Remove(iAImageClusterNode const * leaf);
	int GetIndexForNode(iAImageClusterNode const* leaf);
	iAImageClusterNode * GetNodeForWidget(iAImagePreviewWidget* widget);
	QVector<FavoriteData> m_favorites;
	iAPreviewWidgetPool* m_previewPool;
	QLayout *m_likeLayout, *m_hateLayout;
};

#endif // IA_FAVORITE_WIDGET_H
