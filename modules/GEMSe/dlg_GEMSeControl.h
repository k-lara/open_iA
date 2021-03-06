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
 
#ifndef DLG_GEMSECONTROL_H
#define DLG_GEMSECONTROL_H

#include "ui_GEMSeControl.h"

#include <iAQTtoUIConnector.h>
typedef iAQTtoUIConnector<QDockWidget, Ui_GEMSeControl>   dlg_GEMSeControlUI;

class iAImageClusterer;
class iAImageSampler;

class dlg_labels;
class dlg_modalities;
class dlg_modalitySPLOM;
class dlg_samplingSettings;
class dlg_progress;
class dlg_GEMSe;
class iASamplingResults;

#include <vtkSmartPointer.h>

class vtkImageData;

class dlg_GEMSeControl: public dlg_GEMSeControlUI
{
	Q_OBJECT
public:
	dlg_GEMSeControl(QWidget *parentWidget,
		dlg_GEMSe* dlgGEMSe,
		dlg_modalities* dlgModalities,
		dlg_labels* dlgLabels,
		QString const & defaultThemeName
	);
	bool LoadSampling(QString const & fileName);
	bool LoadClustering(QString const & fileName);
	void ExportAttributeRangeRanking();
	void ExportRankings();
	void ImportRankings();
public slots:
	void ExportIDs();
private slots:
	void StartSampling();
	void SamplingFinished();
	void ClusteringFinished();
	void LoadSampling();
	void LoadClustering();
	void CalculateClustering();
	void StoreClustering();
	void StoreSampling();
	void CalcCharacteristics();
	void CalcRefImgComp();
	void StoreAll();
	void DataAvailable();
	void ShowImage(vtkSmartPointer<vtkImageData> imgData);
	void ModalitySPLOM();
	void Help();
	void ResetFilters();
	void SetMagicLensOpacity(int newValue);
	void SetIconSize(int newSize);
	void SetColorTheme(const QString &);
	void SetRepresentative(const QString &);
private:
	void OpenGEMSe();
	
	dlg_modalities*                      m_dlgModalities;
	dlg_samplingSettings*                m_dlgSamplingSettings;
	dlg_progress*						 m_dlgProgress;
	dlg_GEMSe*                           m_dlgGEMSe;
	dlg_modalitySPLOM*					 m_dlgModalitySPLOM;
	dlg_labels*                          m_dlgLabels;

	QSharedPointer<iAImageSampler>       m_sampler;
	QSharedPointer<iASamplingResults>    m_samplingResults;

	QSharedPointer<iAImageClusterer>     m_clusterer;

	QString								 m_outputFolder;
	
	QString                              m_cltFile;
	QString                              m_m_metaFileName;
};

#endif // DLG_GEMSECONTROL_H
