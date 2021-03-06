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
#include "iAFavoriteWidget.h"

#include "iAConsole.h"
#include "iAImagePreviewWidget.h"
#include "iAImageTree.h"
#include "iAPreviewWidgetPool.h"
#include "iAQtCaptionWidget.h"
#include "iAGEMSeConstants.h"

#include <QVBoxLayout>

typedef QVBoxLayout LikeLayoutType;

iAFavoriteWidget::iAFavoriteWidget(iAPreviewWidgetPool* previewPool) :
	m_previewPool(previewPool)
{
	QWidget* favListWdgt = new QWidget();
	QHBoxLayout* favListLayout = new QHBoxLayout();
	favListLayout->setSpacing(0);
	favListLayout->setMargin(0);
	favListLayout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
	favListWdgt->setLayout(favListLayout);
	favListWdgt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QWidget* likes = new QWidget();
	m_likeLayout =  new LikeLayoutType();
	m_likeLayout->setSpacing(ExampleViewSpacing);
	m_likeLayout->setContentsMargins(ExampleViewSpacing, ExampleViewSpacing, ExampleViewSpacing, ExampleViewSpacing);
	m_likeLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	//m_likeLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	likes->setLayout(m_likeLayout);
	likes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	likes->setStyleSheet("background-color: #DFD;");

	favListLayout->addWidget(likes);
	
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	SetCaptionedContent(this, "Favorites", favListWdgt);
	setFixedWidth(FavoriteBarWidth);
}

bool iAFavoriteWidget::HasAnyFavorite() const
{
	return m_favorites.size() > 0;
}

bool iAFavoriteWidget::ToggleLike(iAImageClusterNode * node)
{
	if (!node)
	{
		DEBUG_LOG("ERROR in favorites: ToggleLike called for NULL node.\n");
		return false;
	}
	if (node->GetAttitude() == iAImageClusterNode::Liked)
	{
		node->SetAttitude(iAImageClusterNode::NoPreference);
		Remove(node);
		return false;
	}
	else
	{
		node->SetAttitude(iAImageClusterNode::Liked);
		Add(node);
		return true;
	}
}

bool iAFavoriteWidget::ToggleHate(iAImageClusterNode * node)
{
	if (!node)
	{
		DEBUG_LOG("ERROR in favorites: ToggleHate called for NULL node.\n");
		return false;
	}
	if (node->GetAttitude() == iAImageClusterNode::Hated)
	{
		node->SetAttitude(iAImageClusterNode::NoPreference);
		return false;
	}
	else
	{
		if (node->GetAttitude() == iAImageClusterNode::Liked)
		{
			int idx = GetIndexForNode(node);
			if (idx == -1)
			{
				DEBUG_LOG("ERROR in favorites: node not found in favorite list.\n");
				return false;
			}
			iAImagePreviewWidget * widget = m_favorites[idx].widget;
			if (!widget)
			{
				DEBUG_LOG("ERROR in favorites: remove called for unset widget.\n");
				return false;
			}
			m_likeLayout->removeWidget(widget);
		}
		node->SetAttitude(iAImageClusterNode::Hated);
		return true;
	}
}

void iAFavoriteWidget::Add(iAImageClusterNode * node)
{
	if (!node || node->GetAttitude() != iAImageClusterNode::Liked)
	{
		return;
	}
	iAImagePreviewWidget * widget = m_previewPool->GetWidget(this);
	if (!widget)
	{
		DEBUG_LOG("FavoriteView: No more slicer widgets available.\n");
		return;
	}
	widget->setFixedSize(FavoriteWidth, FavoriteWidth);
	widget->SetImage(node->GetRepresentativeImage(iARepresentativeType::Difference), false, true);
	connect(widget, SIGNAL(Clicked()), this, SLOT(FavoriteClicked()));
	connect(widget, SIGNAL(Updated()), this, SIGNAL(ViewUpdated()));
	m_favorites.push_back(FavoriteData(node, widget));
	dynamic_cast<LikeLayoutType*>(m_likeLayout)->insertWidget(0, widget);
}


void iAFavoriteWidget::Remove(iAImageClusterNode const * node)
{
	if (!node)
	{
		DEBUG_LOG("ERROR in favorites: remove called for NULL node\n");
		return;
	}
	int idx = GetIndexForNode(node);
	if (idx == -1)
	{
		DEBUG_LOG("ERROR in favorites: node not found in favorite list\n");
		return;
	}
	iAImagePreviewWidget * widget = m_favorites[idx].widget;
	if (!widget)
	{
		DEBUG_LOG("ERROR in favorites: remove called for unset widget\n");
		return;
	}
	m_favorites[idx].node = 0;
	m_favorites[idx].widget = 0;
	m_favorites.remove(idx);
	disconnect(widget, SIGNAL(Clicked()), this, SLOT(FavoriteClicked()));
	disconnect(widget, SIGNAL(Updated()), this, SIGNAL(ViewUpdated()));
	m_previewPool->ReturnWidget(widget);
}

void iAFavoriteWidget::FavoriteClicked()
{
	iAImagePreviewWidget * widget = dynamic_cast<iAImagePreviewWidget*>(sender());
	if (!widget)
	{
		DEBUG_LOG("FavoriteClicked: Error: invalid sender!\n");
	}
	iAImageClusterNode * node = GetNodeForWidget(widget);
	if (!node)
	{
		DEBUG_LOG("FavoriteClicked: Error: node not found!\n");
	}
	emit Clicked(node);
}


int iAFavoriteWidget::GetIndexForNode(iAImageClusterNode const* node)
{
	for (int i=0; i<m_favorites.size(); ++i)
	{
		if (m_favorites[i].node == node)
		{
			return i;
		}
	}
	return -1;
}

iAImageClusterNode * iAFavoriteWidget::GetNodeForWidget(iAImagePreviewWidget* widget)
{
	for (FavoriteData const & data: m_favorites)
	{
		if (data.widget == widget)
		{
			return data.node;
		}
	}
	return 0;
}


QVector<iAImageClusterNode const *> iAFavoriteWidget::GetFavorites(iAImageClusterNode::Attitude att) const
{
	QVector<iAImageClusterNode const *> result;
	for (FavoriteData const & data : m_favorites)
	{
		if (data.node->GetAttitude() == att)
		{
			result.push_back(data.node);
		}
	}
	return result;
}
