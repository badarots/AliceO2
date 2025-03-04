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

#ifndef O2_TPC_INTERPOLATION_SPEC_H
#define O2_TPC_INTERPOLATION_SPEC_H

/// @file   TPCInterpolationSpec.h

#include "DataFormatsTPC/Constants.h"
#include "SpacePoints/TrackInterpolation.h"
#include "SpacePoints/TrackResiduals.h"
#include "Framework/DataProcessorSpec.h"
#include "Framework/Task.h"
#include "TStopwatch.h"
#include "ReconstructionDataFormats/GlobalTrackID.h"

using namespace o2::framework;

namespace o2::globaltracking
{
struct DataRequest;
} // namespace o2::globaltracking

namespace o2
{
namespace tpc
{
class TPCInterpolationDPL : public Task
{
 public:
  TPCInterpolationDPL(std::shared_ptr<o2::globaltracking::DataRequest> dr, bool useMC, bool processITSTPConly, bool writeResiduals) : mDataRequest(dr), mUseMC(useMC), mProcessITSTPConly(processITSTPConly), mWriteResiduals(writeResiduals) {}
  ~TPCInterpolationDPL() override = default;
  void init(InitContext& ic) final;
  void run(ProcessingContext& pc) final;
  void endOfStream(EndOfStreamContext& ec) final;

 private:
  o2::tpc::TrackInterpolation mInterpolation;                    ///< track interpolation engine
  o2::tpc::TrackResiduals mResidualProcessor;                    ///< conversion and avg. distortion map creation engine
  std::shared_ptr<o2::globaltracking::DataRequest> mDataRequest; ///< steers the input
  bool mUseMC{false}; ///< MC flag
  bool mProcessITSTPConly{false}; ///< should also tracks without outer point (ITS-TPC only) be processed?
  bool mWriteResiduals{false};    ///< whether or not the unbinned unfiltered residuals should be sent out
  TStopwatch mTimer;
};

/// create a processor spec
framework::DataProcessorSpec getTPCInterpolationSpec(o2::dataformats::GlobalTrackID::mask_t src, bool useMC, bool processITSTPConly, bool writeResiduals);

} // namespace tpc
} // namespace o2

#endif
