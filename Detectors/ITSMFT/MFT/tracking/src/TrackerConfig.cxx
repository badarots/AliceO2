// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file GeometryTGeo.cxx
/// \brief Implementation of the GeometryTGeo class
/// \author bogdan.vulpescu@clermont.in2p3.fr - adapted from ITS, 21.09.2017

#include "MFTTracking/TrackerConfig.h"

#include <fairlogger/Logger.h>

//__________________________________________________________________________
o2::mft::TrackerConfig::TrackerConfig()
  : mMinTrackPointsLTF{5},
    mMinTrackPointsCA{4},
    mMinTrackStationsLTF{4},
    mMinTrackStationsCA{4},
    mLTFclsRCut{0.0100},
    mLTFclsR2Cut{0.0100 * 0.0100},
    mROADclsRCut{0.0400},
    mROADclsR2Cut{0.0400 * 0.0400},
    mLTFseed2BinWin{3},
    mLTFinterBinWin{3},
    mRBins{50},
    mPhiBins{50},
    mRPhiBins{50 * 50},
    mRBinSize{(constants::index_table::RMax - constants::index_table::RMin) / 50.},
    mPhiBinSize{(constants::index_table::PhiMax - constants::index_table::PhiMin) / 50.},
    mInverseRBinSize{50. / (constants::index_table::RMax - constants::index_table::RMin)},
    mInversePhiBinSize{50. / (constants::index_table::PhiMax - constants::index_table::PhiMin)}
{
  /// default constructor
}

//__________________________________________________________________________
void o2::mft::TrackerConfig::initialize(const MFTTrackingParam& trkParam)
{
  /// initialize from MFTTrackingParam (command line configuration parameters)

  mMinTrackPointsLTF = trkParam.MinTrackPointsLTF;
  mMinTrackPointsCA = trkParam.MinTrackPointsCA;
  mMinTrackStationsLTF = trkParam.MinTrackStationsLTF;
  mMinTrackStationsCA = trkParam.MinTrackStationsCA;
  mLTFclsRCut = trkParam.LTFclsRCut;
  mLTFclsR2Cut = mLTFclsRCut * mLTFclsRCut;
  mROADclsRCut = trkParam.ROADclsRCut;
  mROADclsR2Cut = mROADclsRCut * mROADclsRCut;
  mLTFseed2BinWin = trkParam.LTFseed2BinWin;
  mLTFinterBinWin = trkParam.LTFinterBinWin;

  mRBins = trkParam.RBins;
  mPhiBins = trkParam.PhiBins;
  mRPhiBins = trkParam.RBins * trkParam.PhiBins;
  if (mRPhiBins > constants::index_table::MaxRPhiBins) {
    LOG(warn) << "To many RPhiBins for this configuration!";
    mRPhiBins = constants::index_table::MaxRPhiBins;
    mRBins = sqrt(constants::index_table::MaxRPhiBins);
    mPhiBins = sqrt(constants::index_table::MaxRPhiBins);
    LOG(warn) << "Using instead RBins " << mRBins << " and PhiBins " << mPhiBins;
  }
  mRBinSize = (constants::index_table::RMax - constants::index_table::RMin) / mRBins;
  mPhiBinSize = (constants::index_table::PhiMax - constants::index_table::PhiMin) / mPhiBins;
}
