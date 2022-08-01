/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#ifndef TVM_SCRIPT_BUILDER_RELAX_UTILS_H_
#define TVM_SCRIPT_BUILDER_RELAX_UTILS_H_

#include "./base.h"
#include "./function_frame.h"

namespace tvm {
namespace script {
namespace builder {
namespace relax {

inline tvm::relax::BlockBuilder GetBlockBuilder(const String& method) {
  Builder builder = Builder::Current();
  Optional<FunctionFrame> relax_func_frame = builder->FindFrame<FunctionFrame>();
  CHECK(relax_func_frame.defined()) << "ValueError: Relax Function frame not find. Please ensure '"
                                    << method << "' is called under R.function()";
  return relax_func_frame.value()->block_builder;
}

}  // namespace relax
}  // namespace builder
}  // namespace script
}  // namespace tvm

#endif  // TVM_SCRIPT_BUILDER_TIR_UTILS_H_
