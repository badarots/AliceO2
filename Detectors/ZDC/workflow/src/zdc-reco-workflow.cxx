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

#include "ZDCWorkflow/RecoWorkflow.h"
#include "CommonUtils/ConfigurableParam.h"
#include "DetectorsRaw/HBFUtilsInitializer.h"
#include "Framework/CallbacksPolicy.h"

using namespace o2::framework;

// ------------------------------------------------------------------
void customize(std::vector<o2::framework::CallbacksPolicy>& policies)
{
  o2::raw::HBFUtilsInitializer::addNewTimeSliceCallback(policies);
}

// we need to add workflow options before including Framework/runDataProcessing
void customize(std::vector<o2::framework::ConfigParamSpec>& workflowOptions)
{
  // option allowing to set parameters
  workflowOptions.push_back(ConfigParamSpec{"disable-mc", o2::framework::VariantType::Bool, false, {"disable MC propagation even if available"}});
  workflowOptions.push_back(ConfigParamSpec{"disable-root-input", o2::framework::VariantType::Bool, false, {"disable root-files input readers"}});
  workflowOptions.push_back(ConfigParamSpec{"disable-root-output", o2::framework::VariantType::Bool, false, {"disable root-files output writers"}});
  workflowOptions.push_back(ConfigParamSpec{"reco-verbosity", VariantType::Int, 0, {"reco verbosity level"}});
  workflowOptions.push_back(ConfigParamSpec{"enable-debug-output", VariantType::Bool, false, {"enable debug tree output"}});
  std::string keyvaluehelp("Semicolon separated key=value strings ...");
  workflowOptions.push_back(ConfigParamSpec{"configKeyValues", VariantType::String, "", {keyvaluehelp}});
  o2::raw::HBFUtilsInitializer::addConfigOption(workflowOptions);
}

// ------------------------------------------------------------------

#include "Framework/runDataProcessing.h"

WorkflowSpec defineDataProcessing(ConfigContext const& configcontext)
{
  LOG(info) << "WorkflowSpec defineDataProcessing";
  // Update the (declared) parameters if changed from the command line
  o2::conf::ConfigurableParam::updateFromString(configcontext.options().get<std::string>("configKeyValues"));
  // write the configuration used for the digitizer workflow
  //o2::conf::ConfigurableParam::writeINI("o2tpcits-match-recoflow_configuration.ini");

  auto useMC = !configcontext.options().get<bool>("disable-mc");
  auto disableRootInp = configcontext.options().get<bool>("disable-root-input");
  auto disableRootOut = configcontext.options().get<bool>("disable-root-output");
  auto verbosity = configcontext.options().get<int>("reco-verbosity");
  auto enableDebugOut = configcontext.options().get<bool>("enable-debug-output");

  LOG(info) << "WorkflowSpec getRecoWorkflow useMC " << useMC;
  auto wf = o2::zdc::getRecoWorkflow(useMC, disableRootInp, disableRootOut, verbosity, enableDebugOut);

  // configure dpl timer to inject correct firstTFOrbit: start from the 1st orbit of TF containing 1st sampled orbit
  o2::raw::HBFUtilsInitializer hbfIni(configcontext, wf);

  return std::move(wf);
}
