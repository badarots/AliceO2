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
///
/// \file IOUtils.h
/// \brief
///

#ifndef TRACKINGITSU_INCLUDE_EVENTLOADER_H_
#define TRACKINGITSU_INCLUDE_EVENTLOADER_H_

#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataFormatsITSMFT/ROFRecord.h"
#include "ITStracking/Configuration.h"
#include "ITStracking/ROframe.h"
#include "ITStracking/Label.h"
#include "ITStracking/Road.h"
#include "ITStracking/TrackingConfigParam.h"
#include "ITSMFTBase/SegmentationAlpide.h"
#include "ReconstructionDataFormats/BaseCluster.h"
#include "ITSMFTReconstruction/ChipMappingITS.h"

namespace o2
{

class MCCompLabel;

namespace dataformats
{
template <typename T>
class MCTruthContainer;
}

namespace itsmft
{
class CompClusterExt;
class TopologyDictionary;
}

namespace its
{

namespace ioutils
{
constexpr float DefClusErrorRow = o2::itsmft::SegmentationAlpide::PitchRow * 0.5;
constexpr float DefClusErrorCol = o2::itsmft::SegmentationAlpide::PitchCol * 0.5;
constexpr float DefClusError2Row = DefClusErrorRow * DefClusErrorRow;
constexpr float DefClusError2Col = DefClusErrorCol * DefClusErrorCol;

void loadEventData(ROframe& events, gsl::span<const itsmft::CompClusterExt> clusters,
                   gsl::span<const unsigned char>::iterator& pattIt, const itsmft::TopologyDictionary* dict,
                   const dataformats::MCTruthContainer<MCCompLabel>* clsLabels = nullptr);
int loadROFrameData(const o2::itsmft::ROFRecord& rof, ROframe& events, gsl::span<const itsmft::CompClusterExt> clusters,
                    gsl::span<const unsigned char>::iterator& pattIt, const itsmft::TopologyDictionary* dict,
                    const dataformats::MCTruthContainer<MCCompLabel>* mClsLabels = nullptr);

void convertCompactClusters(gsl::span<const itsmft::CompClusterExt> clusters,
                            gsl::span<const unsigned char>::iterator& pattIt,
                            std::vector<o2::BaseCluster<float>>& output,
                            const itsmft::TopologyDictionary* dict);

inline static const o2::itsmft::ChipMappingITS& getChipMappingITS()
{
  static const o2::itsmft::ChipMappingITS MP;
  return MP;
}

std::vector<std::unordered_map<int, Label>> loadLabels(const int, const std::string&);
void writeRoadsReport(std::ofstream&, std::ofstream&, std::ofstream&, const std::vector<std::vector<Road>>&,
                      const std::unordered_map<int, Label>&);
} // namespace ioutils
} // namespace its
} // namespace o2

#endif /* TRACKINGITSU_INCLUDE_EVENTLOADER_H_ */
