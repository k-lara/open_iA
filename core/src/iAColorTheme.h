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
 
#ifndef IA_COLOR_THEME_H
#define IA_COLOR_THEME_H

#include "open_iA_Core_export.h"

#include <QColor>
#include <QList>
#include <QMap>

#include <vector>

class QString;
class QStringList;

//! A simple color theme. Holds a number of colors (which can be distinguished easily)
class open_iA_Core_API iAColorTheme
{
public:
	//! returns the number of colors in this theme
	size_t size() const;
	//! returns the color with the given index in this theme
	QColor const & GetColor(int idx) const;
	//! add a color to the theme (typically only necessary for theme creators)
	void AddColor(QColor const &);

	static iAColorTheme const * NullTheme();
private:
	std::vector<QColor> m_colors;
	static QColor ErrorColor;
};

//! Manager for color themes. Internally creates the qualitative color themes from
//! Color Brewer (http://mkweb.bcgsc.ca/brewer/swatches/brewer.txt) and provides
//! access to their names as well as the single themes.
class open_iA_Core_API iAColorThemeManager
{
public:
	//! only every need one of those
	static iAColorThemeManager const & GetInstance();
	//! Get the list of all available themes
	QList<QString> GetAvailableThemes() const;
	//! Get a theme by name
	iAColorTheme const * GetTheme(QString const & name) const;
private:
	iAColorThemeManager();
	~iAColorThemeManager();

	QMap<QString, iAColorTheme*> m_themes;
};

#endif // IA_COLOR_THEME_H