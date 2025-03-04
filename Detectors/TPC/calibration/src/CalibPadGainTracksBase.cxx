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

/// @file   CalibPadGainTracksBase.cxx
/// @author Matthias Kleiner, mkleiner@ikf.uni-frankfurt.de

#include "TPCCalibration/CalibPadGainTracksBase.h"
#include "TPCCalibration/IDCDrawHelper.h"
#include "TPCBase/ROC.h"
#include "TPCBase/Painter.h"

// root includes
#include "TFile.h"
#include "TCanvas.h"

using namespace o2::tpc;

CalibPadGainTracksBase::CalibPadGainTracksBase(const bool initCalPad) : mPadHistosDet(std::make_unique<DataTHistos>("Histo"))
{
  if (initCalPad) {
    initCalPadMemory();
    initCalPadStdDevMemory();
  }
};

void CalibPadGainTracksBase::init(const unsigned int nBins, const float xmin, const float xmax, const bool useUnderflow, const bool useOverflow)
{
  DataTHisto hist(nBins, xmin, xmax, useUnderflow, useOverflow);
  for (auto& calArray : mPadHistosDet->getData()) {
    for (auto& tHist : calArray.getData()) {
      tHist = hist;
    }
  }
}

void CalibPadGainTracksBase::dumpGainMap(const char* fileName) const
{
  TFile f(fileName, "RECREATE");
  f.WriteObject(mGainMap.get(), "GainMap");
}

void CalibPadGainTracksBase::drawExtractedGainMapHelper(const bool type, const int typeMap, const Sector sector, const std::string filename, const float minZ, const float maxZ, const bool norm) const
{
  const auto map = (typeMap == 0) ? std::make_unique<CalPad>(*mGainMap) : std::make_unique<CalPad>(*mSigmaMap);
  if (!map) {
    LOGP(error, "Map not set");
    return;
  }

  if (norm) {
    *map /= *mGainMap;
  }

  std::function<float(const unsigned int, const unsigned int, const unsigned int, const unsigned int)> idcFunc = [mapTmp = map.get()](const unsigned int sector, const unsigned int region, const unsigned int lrow, const unsigned int pad) {
    return mapTmp->getValue(sector, Mapper::getGlobalPadNumber(lrow, pad, region));
  };

  IDCDrawHelper::IDCDraw drawFun;
  drawFun.mIDCFunc = idcFunc;
  const std::string zAxisTitle = (typeMap == 0) ? "rel. gain" : (norm ? "sigma / rel. gain" : "sigma");
  type ? IDCDrawHelper::drawSide(drawFun, sector.side(), zAxisTitle, filename, minZ, maxZ) : IDCDrawHelper::drawSector(drawFun, 0, Mapper::NREGIONS, sector, zAxisTitle, filename, minZ, maxZ);
}

void CalibPadGainTracksBase::divideGainMap(const char* inpFile, const char* mapName)
{
  TFile f(inpFile, "READ");
  o2::tpc::CalPad* gainMap = nullptr;
  f.GetObject(mapName, gainMap);

  if (!gainMap) {
    LOGP(info, "GainMap {} not found returning", mapName);
    return;
  }

  *mGainMap /= *gainMap;
  delete gainMap;
}

void CalibPadGainTracksBase::setGainMap(const char* inpFile, const char* mapName)
{
  TFile f(inpFile, "READ");
  o2::tpc::CalPad* gainMap = nullptr;
  f.GetObject(mapName, gainMap);

  if (!gainMap) {
    LOGP(info, "GainMap {} not found returning", mapName);
    return;
  }

  mGainMap = std::make_unique<CalPad>(*gainMap);
  delete gainMap;
}

TCanvas* CalibPadGainTracksBase::drawExtractedGainMapPainter() const
{
  return painter::draw(*mGainMap);
}

void CalibPadGainTracksBase::resetHistos()
{
  for (auto& calArray : mPadHistosDet->getData()) {
    for (auto& tHist : calArray.getData()) {
      tHist.reset();
    }
  }
}

void CalibPadGainTracksBase::dumpToFile(const char* outFileName, const char* outName) const
{
  TFile fOut(outFileName, "RECREATE");
  fOut.WriteObject(this, outName);
  fOut.Close();
}

void CalibPadGainTracksBase::fill(const gsl::span<const DataTHistos>& caldets)
{
  for (const auto& caldet : caldets) {
    fill(caldet);
  }
}

void CalibPadGainTracksBase::print() const
{
  unsigned int totEntries = 0;
  int minEntries = -1;
  for (auto& calArray : mPadHistosDet->getData()) {
    for (auto& tHist : calArray.getData()) {
      const auto entries = tHist.getEntries();
      totEntries += entries;
      if (entries < minEntries || minEntries == -1) {
        minEntries = entries;
      }
    }
  }
  LOGP(info, "Total number of entries: {}", totEntries);
  LOGP(info, "Minimum number of entries: {}", minEntries);
}

bool CalibPadGainTracksBase::hasEnoughData(const int minEntries) const
{
  for (auto& calArray : mPadHistosDet->getData()) {
    for (auto& tHist : calArray.getData()) {
      const auto entries = tHist.getEntries();
      if (entries > 0 && entries < minEntries) {
        return false;
      }
    }
  }
  return true;
}

void CalibPadGainTracksBase::finalize(const float low, const float high)
{
  for (int roc = 0; roc < ROC::MaxROC; ++roc) {
    const auto padsInRoc = ROC(roc).isIROC() ? Mapper::getPadsInIROC() : Mapper::getPadsInOROC();
    for (int pad = 0; pad < padsInRoc; ++pad) {
      const auto stat = mPadHistosDet->getCalArray(roc).getData()[pad].getStatisticsData(low, high);
      mGainMap->getCalArray(roc).getData()[pad] = stat.mCOG;
      mSigmaMap->getCalArray(roc).getData()[pad] = stat.mStdDev;
    }
  }
}
