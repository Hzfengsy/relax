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

#include "var.h"

#include "../builder.h"

namespace tvm {
namespace script {
namespace builder {
namespace relax {

TVM_STATIC_IR_FUNCTOR(Namer, vtable)
    .set_dispatch<tvm::relax::VarNode>([](const ObjectRef& node, String name) -> void {
      using tvm::relax::VarNode;
      VarNode* var = const_cast<VarNode*>(node.as<VarNode>());
      var->vid = tvm::relax::Id(name);
    });

tvm::relax::Var Tensor(Optional<Array<PrimExpr>> shape, DataType dtype, Optional<Integer> ndim) {
  using namespace tvm::relax;
  int n_dim = -1;
  if (shape.defined() && ndim.defined()) {
    CHECK_EQ(shape.value().size(), ndim.value()->value)
        << "The dimension of the given shape is mismatched with the given `ndim`";
    n_dim = shape.value().size();
  } else if (shape.defined()) {
    n_dim = shape.value().size();
  } else if (ndim.defined()) {
    n_dim = ndim.value()->value;
  } else {
    LOG(FATAL) << "The `ndim` must be specified when the shape is None";
  }
  Type dyn_tensor_type = DynTensorType(n_dim, dtype);
  Optional<Expr> shape_expr = NullOpt;
  if (shape.defined()) {
    shape_expr = ShapeExpr(shape.value());
  }
  return Var("", shape_expr, dyn_tensor_type);
}

TVM_REGISTER_GLOBAL("script.builder.relax.Tensor").set_body_typed(Tensor);

}  // namespace relax
}  // namespace builder
}  // namespace script
}  // namespace tvm
