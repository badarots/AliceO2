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

/// @file   EntropyEncoderSpec.cxx

#include <vector>

#include "Framework/ControlService.h"
#include "Framework/ConfigParamRegistry.h"
#include "CPVWorkflow/EntropyEncoderSpec.h"
#include "DetectorsCommonDataFormats/DetID.h"

using namespace o2::framework;

namespace o2
{
namespace cpv
{

EntropyEncoderSpec::EntropyEncoderSpec()
{
  mTimer.Stop();
  mTimer.Reset();
}

void EntropyEncoderSpec::init(o2::framework::InitContext& ic)
{
  std::string dictPath = ic.options().get<std::string>("ctf-dict");
  mCTFCoder.setMemMarginFactor(ic.options().get<float>("mem-factor"));
  if (!dictPath.empty() && dictPath != "none") {
    mCTFCoder.createCodersFromFile<CTF>(dictPath, o2::ctf::CTFCoderBase::OpType::Encoder);
  }
}

void EntropyEncoderSpec::run(ProcessingContext& pc)
{
  auto cput = mTimer.CpuTime();
  mTimer.Start(false);
  auto triggers = pc.inputs().get<gsl::span<TriggerRecord>>("triggers");
  auto clusters = pc.inputs().get<gsl::span<Cluster>>("clusters");

  auto& buffer = pc.outputs().make<std::vector<o2::ctf::BufferType>>(Output{"CPV", "CTFDATA", 0, Lifetime::Timeframe});
  mCTFCoder.encode(buffer, triggers, clusters);
  auto eeb = CTF::get(buffer.data()); // cast to container pointer
  eeb->compactify();                  // eliminate unnecessary padding
  buffer.resize(eeb->size());         // shrink buffer to strictly necessary size
  //  eeb->print();
  mTimer.Stop();
  LOG(info) << "Created encoded data of size " << eeb->size() << " for CPV in " << mTimer.CpuTime() - cput << " s";
}

void EntropyEncoderSpec::endOfStream(EndOfStreamContext& ec)
{
  LOGF(info, "CPV Entropy Encoding total timing: Cpu: %.3e Real: %.3e s in %d slots",
       mTimer.CpuTime(), mTimer.RealTime(), mTimer.Counter() - 1);
}

DataProcessorSpec getEntropyEncoderSpec()
{
  std::vector<InputSpec> inputs;
  inputs.emplace_back("triggers", "CPV", "CLUSTERTRIGRECS", 0, Lifetime::Timeframe);
  inputs.emplace_back("clusters", "CPV", "CLUSTERS", 0, Lifetime::Timeframe);

  return DataProcessorSpec{
    "cpv-entropy-encoder",
    inputs,
    Outputs{{"CPV", "CTFDATA", 0, Lifetime::Timeframe}},
    AlgorithmSpec{adaptFromTask<EntropyEncoderSpec>()},
    Options{{"ctf-dict", VariantType::String, o2::base::NameConf::getCTFDictFileName(), {"File of CTF encoding dictionary"}},
            {"mem-factor", VariantType::Float, 1.f, {"Memory allocation margin factor"}}}};
}

} // namespace cpv
} // namespace o2
