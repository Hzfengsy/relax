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

#include "function_frame.h"

#include "../ir/ir.h"

namespace tvm {
namespace script {
namespace builder {
namespace relax {

FunctionFrame FindFunctionFrame(const String& method) {
  Builder builder = Builder::Current();
  if (Optional<FunctionFrame> relax_func_frame = builder->FindFrame<FunctionFrame>()) {
    if (Optional<BlockFrame> block_frame = builder->GetLastFrame<BlockFrame>()) {
      if (relax_func_frame.value()->default_binding_block_frame == block_frame.value()) {
        return relax_func_frame.value();
      }
    }
  } else {
    LOG(FATAL) << "ValueError: Relax Function frame not find. Please ensure '" << method
               << "' is called under R.function()";
  }
  LOG(FATAL) << "ValueError: '" << method << "' must be called immediately under R.function()";
  throw;
}

void FunctionFrameNode::EnterWithScope() {
  RelaxFrameNode::EnterWithScope();
  // Start a default binding block scope;
  default_binding_block_frame->EnterWithScope();
}

void FunctionFrameNode::ExitWithScope() {
  using tvm::relax::Expr;
  default_binding_block_frame->ExitWithScope();
  RelaxFrameNode::ExitWithScope();
  Builder builder = Builder::Current();
  // Step 1: Create the function.
  Expr output = outputs.size() == 1 ? outputs[0] : tvm::relax::Tuple(outputs);
  output = this->block_builder->Normalize(output);
  Expr body = this->block_builder->Normalize(tvm::relax::SeqExpr(binding_blocks, output));
  CHECK(ret_type.defined()) << "ValueError: Function return type must be defined";
  tvm::relax::Function func(/*params=*/params,
                            /*body=*/body,
                            /*ret_type=*/ret_type.value(),
                            /*attrs=*/DictAttrs(attrs));
  // TODO: remove this line
  func = WithAttr(func, "global_symbol", name.value());
  // Step 2: Update IRModule.
  if (builder->frames.empty()) {
    // Case 0. If there is no output module frame.
    ICHECK(!builder->result.defined()) << "ValueError: Builder.result has already been set";
    builder->result = func;
  } else if (Optional<ir::IRModuleFrame> opt_frame = builder->FindFrame<ir::IRModuleFrame>()) {
    ir::IRModuleFrame frame = opt_frame.value();
    String func_name = name.value_or("");
    if (frame->global_var_map.count(func_name)) {
      ir::UpdateFunction(func_name, func);
    } else {
      ir::AddFunction(func_name, func, /*allow_rename=*/false);
    }
  } else {
    LOG(FATAL) << "ValueError: Cannot find where to insert Relax.Function";
  }
}

FunctionFrame Function() {
  ObjectPtr<FunctionFrameNode> n = make_object<FunctionFrameNode>();
  n->block_builder = tvm::relax::BlockBuilder::Create(/*mod=*/NullOpt);
  n->default_binding_block_frame = BindingBlock();
  return FunctionFrame(n);
}

tvm::relax::Var Arg(const String& name, const tvm::relax::Var& var) {
  FunctionFrame frame = FindFunctionFrame("R.Arg");
  Namer::Name(var, name);
  frame->params.push_back(var);
  return var;
}

void FuncName(const String& name) {
  FunctionFrame frame = FindFunctionFrame("R.func_name");
  if (frame->name.defined()) {
    LOG(FATAL) << "ValueError: Duplicate function name, previous one is: \"" << frame->name.value()
               << "\"";
  }
  frame->name = name;
}

void FuncAttrs(Map<String, ObjectRef> attrs) {
  FunctionFrame frame = FindFunctionFrame("R.func_attr");
  if (!frame->attrs.empty()) {
    LOG(FATAL) << "ValueError: Duplicate function attrs, previous one is:\n" << frame->attrs;
  }
  frame->attrs = attrs;
}

tvm::Type RetType(tvm::Type ret_type) {
  FunctionFrame frame = FindFunctionFrame("R.ret_type");
  if (frame->ret_type.defined()) {
    LOG(FATAL) << "ValueError: Duplicate function return type, previous one is:\n "
               << frame->ret_type.value();
  }
  frame->ret_type = ret_type;
  return ret_type;
}

void FuncReturn(const tvm::relax::Expr& value) {
  FunctionFrame frame = FindFunctionFrame("return");
  frame->outputs.push_back(value);
}

TVM_REGISTER_NODE_TYPE(FunctionFrameNode);
TVM_REGISTER_GLOBAL("script.builder.relax.Function").set_body_typed(Function);
TVM_REGISTER_GLOBAL("script.builder.relax.Arg").set_body_typed(Arg);
TVM_REGISTER_GLOBAL("script.builder.relax.FuncName").set_body_typed(FuncName);
TVM_REGISTER_GLOBAL("script.builder.relax.FuncAttrs").set_body_typed(FuncAttrs);
TVM_REGISTER_GLOBAL("script.builder.relax.RetType").set_body_typed(RetType);
TVM_REGISTER_GLOBAL("script.builder.relax.FuncReturn").set_body_typed(FuncReturn);

}  // namespace relax
}  // namespace builder
}  // namespace script
}  // namespace tvm
