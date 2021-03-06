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
 
#ifndef IACHANNELID_H
#define IACHANNELID_H

// TODO: remove completely. instead, return ID when creating channel
enum iAChannelID
{
	ch_None = -1,
	// don't change the order of the ch_MetaN elements!
	ch_Meta0,
	ch_Meta1,
	ch_Meta2,

	ch_XRF,
	ch_SpectrumSelection,
	ch_DistVisLabels,
	ch_DistVisSampledPoints,
	ch_DistVisDistancePairs,
	ch_DistVisEntropy,

	ch_VolumePlayer0,
	ch_VolumePlayer1,

	ch_fixed,
	ch_moving,
	
	ch_Microscopy1,
	ch_Microscopy2,
	ch_Microscopy3,
	ch_Microscopy4,

	ch_ModalityLens,

	ch_ModSPLOMSelection,

	ch_DefectView0,
	ch_DefectView1,
	ch_DefectView2,
	ch_DefectView3,

	ch_DensityMap,

	ch_LabelOverlay,

	ch_SE_DetailView,

	// needs to be the last one, as there can be an arbitrary number of such concentration layers:
	ch_Concentration0,
	ch_Concentration1,
	ch_Concentration2,
	ch_Concentration3,
	ch_Concentration4,
	ch_Concentration5,
	ch_Concentration6,
	ch_Concentration7,
	ch_Concentration8,
	ch_Concentration9,
};

#endif /* IACHANNELID_H */