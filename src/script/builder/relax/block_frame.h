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
#ifndef TVM_SCRIPT_BUILDER_RELAX_BLOCK_FRAME_H_
#define TVM_SCRIPT_BUILDER_RELAX_BLOCK_FRAME_H_

#include <tvm/relax/expr.h>

#include "./base.h"

namespace tvm {
namespace script {
namespace builder {
namespace relax {

class BlockFrameNode : public RelaxFrameNode {
 public:
  Array<tvm::relax::Binding> bindings;
  bool is_dataflow;

  void VisitAttrs(tvm::AttrVisitor* v) {
    RelaxFrameNode::VisitAttrs(v);
    v->Visit("bindings", &bindings);
    v->Visit("is_dataflow", &is_dataflow);
  }

  static constexpr const char* _type_key = "script.builder.relax.BlockFrame";
  TVM_DECLARE_FINAL_OBJECT_INFO(BlockFrameNode, RelaxFrameNode);

 public:
  void EnterWithScope() final;
  void ExitWithScope() final;
};

class BlockFrame : public RelaxFrame {
 public:
  TVM_DEFINE_MUTABLE_NOTNULLABLE_OBJECT_REF_METHODS(BlockFrame, RelaxFrame, BlockFrameNode);
};

BlockFrame BindingBlock();
BlockFrame Dataflow();

}  // namespace relax
}  // namespace builder
}  // namespace script
}  // namespace tvm

#endif  // TVM_SCRIPT_BUILDER_TIR_BLOCK_FRAME_H_
