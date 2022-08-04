# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

import contextlib
from functools import partial
from typing import Any

from ...builder import Frame, name
from ...builder import relax as R
from .. import dispatch, doc
from ..parser import Parser


def bind_value(self: Parser, name: str, value: Any) -> Any:
    if isinstance(value, Frame):
        value.add_callback(partial(value.__exit__, None, None, None))
        res = value.__enter__()
        def_(name, res)
        return res
    elif isinstance(value, (T.Buffer_, IterVar, Var, tuple, list)):
        def_(name, value)
        return value
    elif isinstance(value, PrimExpr):
        var = T.var(value.dtype)
        def_(name, var)
        frame = T.let(var, value)
        frame.add_callback(partial(frame.__exit__, None, None, None))
        frame.__enter__()
        return var
    else:
        self.report_error("Do not know how to bind type: " + str(type(value)))


@dispatch.register(token="relax", type_name="FunctionDef")
def visit_function_def(self: Parser, node: doc.FunctionDef) -> None:
    with self.var_table.with_frame():
        with R.function():
            R.func_name(node.name)
            with self.with_dispatch_token("relax"):
                # TODO: define the GlobalVar
                self.visit(node.args)
                # self.visit(node.returns)
                self.visit_body(node.body)


@dispatch.register(token="relax", type_name="arguments")
def visit_arguments(self: Parser, node: doc.arguments) -> None:
    arg: doc.arg
    for arg in node.args:
        if arg.annotation is None:
            self.report_error(arg, "Type annotation is required for function parameters.")
        param = R.arg(arg.arg, self.visit_tvm_annotation(arg.annotation))
        self.var_table.add(arg.arg, param)


@dispatch.register(token="relax", type_name="tvm_annotation")
def visit_tvm_annotation(self: Parser, node: doc.expr):
    annotation = self.eval_expr(node)
    if callable(annotation):
        annotation = annotation()
    return annotation


@dispatch.register(token="relax", type_name="Assign")
def visit_assign(self: Parser, node: doc.Assign) -> None:
    if len(node.targets) != 1:
        self.report_error(node, "Consequential assignments like 'a = b = c' are not supported.")
    lhs = node.targets[0]
    rhs = self.eval_expr(node.value)
    self.eval_assign(target=lhs, source=rhs, bind_value=bind_value)
