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

#include "Framework/TableBuilder.h"
#include <memory>
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <arrow/builder.h>
#include <arrow/memory_pool.h>
#include <arrow/record_batch.h>
#include <arrow/table.h>
#include <arrow/type_traits.h>
#include <arrow/status.h>
#include <arrow/util/key_value_metadata.h>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

using namespace arrow;

namespace
{

// FIXME: Dummy schema, to compile.
template <typename TYPE, typename C_TYPE>
void ArrayFromVector(const std::vector<C_TYPE>& values, std::shared_ptr<arrow::Array>* out)
{
  typename arrow::TypeTraits<TYPE>::BuilderType builder;
  for (size_t i = 0; i < values.size(); ++i) {
    auto status = builder.Append(values[i]);
    assert(status.ok());
  }
  auto status = builder.Finish(out);
  assert(status.ok());
}

} // namespace

namespace o2::framework
{
void addLabelToSchema(std::shared_ptr<arrow::Schema>& schema, const char* label)
{
  schema = schema->WithMetadata(
    std::make_shared<arrow::KeyValueMetadata>(
      std::vector{std::string{"label"}},
      std::vector{std::string{label}}));
}

std::shared_ptr<arrow::Table>
  TableBuilder::finalize()
{
  bool status = mFinalizer(mSchema, mArrays, mHolders);
  if (status == false) {
    throwError(runtime_error("Unable to finalize"));
  }
  assert(mSchema->num_fields() > 0 && "Schema needs to be non-empty");
  return arrow::Table::Make(mSchema, mArrays);
}

void TableBuilder::throwError(RuntimeErrorRef const& ref)
{
  throw ref;
}

void TableBuilder::validate(const int nColumns, std::vector<std::string> const& columnNames) const
{
  if (nColumns != columnNames.size()) {
    throwError(runtime_error("Mismatching number of column types and names"));
  }
  if (mHolders != nullptr) {
    throwError(runtime_error("TableBuilder::persist can only be invoked once per instance"));
  }
}

void TableBuilder::setLabel(const char* label)
{
  mSchema = mSchema->WithMetadata(std::make_shared<arrow::KeyValueMetadata>(std::vector{std::string{"label"}}, std::vector{std::string{label}}));
}

} // namespace o2::framework
