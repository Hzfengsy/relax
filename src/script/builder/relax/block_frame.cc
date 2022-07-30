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

#include "block_frame.h"

#include <tvm/runtime/registry.h>

#include "./utils.h"

namespace tvm {
namespace script {
namespace builder {
namespace relax {

using tvm::relax::BlockBuilder;

BlockFrame Dataflow() {
  ObjectPtr<BlockFrameNode> n = make_object<BlockFrameNode>();
  n->is_dataflow = true;
  return BlockFrame(n);
}

BlockFrame BindingBlock() {
  ObjectPtr<BlockFrameNode> n = make_object<BlockFrameNode>();
  n->is_dataflow = false;
  return BlockFrame(n);
}

void BlockFrameNode::EnterWithScope() {
  RelaxFrameNode::EnterWithScope();
  if (is_dataflow) {
    BlockBuilder block_builder = GetBlockBuilder("R.dataflow");
    block_builder->BeginDataflowBlock();
  } else {
    BlockBuilder block_builder = GetBlockBuilder("R.block_binding");
    block_builder->BeginBindingBlock();
  }
}

void BlockFrameNode::ExitWithScope() {
  RelaxFrameNode::ExitWithScope();
  // We've checked that the scope when EnterWithScope, no need to check again.
  Optional<FunctionFrame> func_frame = Builder::Current()->FindFrame<FunctionFrame>();
  ICHECK(func_frame.defined());
  BlockBuilder block_builder = func_frame.value()->block_builder;
  tvm::relax::BindingBlock block = block_builder->EndBlock();
  if (!block->bindings.empty()) {
    func_frame.value()->binding_blocks.push_back(block);
  };
}

TVM_REGISTER_NODE_TYPE(BlockFrameNode);
TVM_REGISTER_GLOBAL("script.builder.tir.Dataflow").set_body_typed(Dataflow);
// BindingBlock is not registered in the global registry

}  // namespace relax
}  // namespace builder
}  // namespace script
}  // namespace tvm
