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
#include "op.h"

#include "./utils.h"

namespace tvm {
namespace script {
namespace builder {
namespace relax {

using tvm::relax::Expr;
tvm::relax::Var CallTIR(const Expr& func,              //
                        const Array<Expr>& args,       //
                        const Array<PrimExpr>& shape,  //
                        const DataType& dtype,         //
                        const Optional<Expr>& packed_ints) {
  using tvm::relax::Call;
  using tvm::relax::ShapeExpr;
  using tvm::relax::Tuple;

  tvm::relax::BlockBuilder block_builder = GetBlockBuilder("R.call_tir");
  static const Op& op = Op::Get("relax.call_tir");
  ShapeExpr shape_expr = ShapeExpr(shape);
  Tuple tuple_args = Tuple(args);
  Type type = tvm::relax::DynTensorType(shape.size(), dtype);

  Array<Expr> call_args = {func, tuple_args, shape_expr};
  if (packed_ints.defined()) {
    call_args.push_back(packed_ints.value());
  }
  Call call_tir = Call(op, call_args, /*attrs=*/{}, /*type_args=*/{type});
  return block_builder->Emit(call_tir);
}

tvm::relax::Var CallTIR(const String& func_name,       //
                        const Array<Expr>& args,       //
                        const Array<PrimExpr>& shape,  //
                        const DataType& dtype,         //
                        const Optional<Expr>& packed_ints) {
  Expr func = tvm::relax::ExternFunc(func_name);
  return CallTIR(func, args, shape, dtype, packed_ints);
}

TVM_REGISTER_GLOBAL("script.builder.relax.CallTIR").set_body([](TVMArgs args, TVMRetValue* rv) {
  CHECK_EQ(args.num_args, 5);
  const auto& arg = args[0];
  if (String::CanConvertFrom(arg)) {
    const String& name = arg.operator String();
    *rv = CallTIR(name, args[1], args[2], args[3], args[4]);
  } else if (arg.IsObjectRef<Expr>()) {
    *rv = CallTIR(arg.AsObjectRef<Expr>(), args[1], args[2], args[3], args[4]);
  } else {
    LOG(FATAL) << "call_tir expected a string or an expr as first argument, but got "
               << runtime::ArgTypeCode2Str(arg.type_code());
  }
});

}  // namespace relax
}  // namespace builder
}  // namespace script
}  // namespace tvm
