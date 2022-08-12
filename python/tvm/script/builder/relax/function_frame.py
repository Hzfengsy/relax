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
"""TVM Script Relax Function Frame"""
import inspect
from typing import Callable, Mapping, Optional, Union, TypeVar

from tvm._ffi import register_object as _register_object
from tvm.ir import Type
from tvm.relax.expr import Var
from tvm.runtime.object import Object

from . import _ffi_api
from .base import RelaxFrame
from ..ir import is_defined_in_module


@_register_object("script.builder.relax.FunctionFrame")
class FunctionFrame(RelaxFrame):
    ...


FType = TypeVar("FType", bound=Callable)


def function(f: Optional[FType] = None) -> Union[FunctionFrame, FType]:
    if f is None:
        # Case 0. used in IRBuilder
        # with T.prim_func():
        #    ...
        return _ffi_api.Function()  # pylint: disable=no-member # type: ignore

    # pylint: disable=import-outside-toplevel
    from tvm.script.parse import parse
    from tvm.script.parse.utils import inspect_function_capture

    # pylint: enable=import-outside-toplevel

    if not inspect.isfunction(f):
        raise TypeError(f"Expect a function, but got: {f}")
    if is_defined_in_module(inspect.stack()):
        return f
    return parse(f, inspect_function_capture(f))


setattr(function, "dispatch_token", "relax")


def arg(name: str, var: Var) -> Var:
    return _ffi_api.Arg(name, var)  # pylint: disable=no-member # type: ignore


def func_name(name: str) -> str:
    return _ffi_api.FuncName(name)  # pylint: disable=no-member # type: ignore


def func_attr(attrs: Mapping[str, Object]) -> None:
    return _ffi_api.FuncAttrs(attrs)  # pylint: disable=no-member # type: ignore


def ret_type(type: Type) -> Type:
    return _ffi_api.RetType(type)  # pylint: disable=no-member # type: ignore


def func_return(type: Type) -> Type:
    return _ffi_api.FuncReturn(type)  # pylint: disable=no-member # type: ignore
