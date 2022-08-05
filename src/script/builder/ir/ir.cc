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
#include "./ir.h"

#include "../builder.h"

namespace tvm {
namespace script {
namespace builder {
namespace ir {

String GetUniqueName(const String& name) {
  // TODO: use tvm/ir/global_var_supply.h in PR #12066
  return name;
}

IRModuleFrame IRModule() {
  ObjectPtr<IRModuleFrameNode> n = make_object<IRModuleFrameNode>();
  n->functions.clear();
  return IRModuleFrame(n);
}

void IRModuleFrameNode::ExitWithScope() {
  Map<GlobalVar, BaseFunc> func_map;
  for (const auto& kv : functions) {
    const GlobalVar& gv = kv.first;
    const Optional<BaseFunc>& func = kv.second;
    CHECK(func.defined()) << "ValueError: function " << gv->name_hint << " is not defined";
    func_map.Set(gv, func.value());
  }
  Builder builder = Builder::Current();
  ICHECK(!builder->result.defined()) << "ValueError: Builder.result has already been set";
  builder->result = tvm::IRModule(func_map);
}

IRModuleFrame FindModuleFrame(const String& method) {
  Builder builder = Builder::Current();
  if (Optional<IRModuleFrame> frame = builder->FindFrame<IRModuleFrame>()) {
    if (builder->GetLastFrame<IRModuleFrame>().value() == frame) {
      return frame.value();
    }
  } else {
    LOG(FATAL) << "ValueError: IRModule frame not find. Please ensure '" << method
               << "' is called under I.ir_module()";
  }
  LOG(FATAL) << "ValueError: '" << method << "' must be called immediately under I.ir_module()";
  throw;
}

GlobalVar AddFunction(const String& func_name,         //
                      const Optional<BaseFunc>& func,  //
                      bool allow_rename) {
  IRModuleFrame frame = FindModuleFrame("I.AddFunction");
  if (!allow_rename) {
    CHECK(!frame->global_var_map.count(func_name))
        << "ValueError: function " << func_name << " already exists";
  }
  String name = GetUniqueName(func_name);
  GlobalVar gv = GlobalVar(name);
  frame->global_var_map.Set(name, gv);
  frame->functions.Set(gv, func);
  return gv;
}

void UpdateFunction(const String& func_name, const BaseFunc& func, bool require_first_define) {
  IRModuleFrame frame = FindModuleFrame("I.UpdateFunction");
  auto it = frame->global_var_map.find(func_name);
  CHECK(it != frame->global_var_map.end())
      << "ValueError: function " << func_name << " does not exist";
  const GlobalVar& gv = (*it).second;
  if (require_first_define) {
    CHECK(!frame->functions.at(gv).defined())
        << "ValueError: function " << func_name << " has already been defined";
  }
  frame->functions.Set(gv, func);
}

TVM_REGISTER_NODE_TYPE(IRModuleFrameNode);
TVM_REGISTER_GLOBAL("script.builder.ir.IRModule").set_body_typed(IRModule);
TVM_REGISTER_GLOBAL("script.builder.ir.AddFunction").set_body_typed(AddFunction);
TVM_REGISTER_GLOBAL("script.builder.ir.UpdateFunction").set_body_typed(UpdateFunction);

}  // namespace ir
}  // namespace builder
}  // namespace script
}  // namespace tvm
