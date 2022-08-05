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
"""TVMScript IR"""

import inspect
from typing import Optional, Type, Union

from tvm._ffi import register_object as _register_object
from tvm.ir import IRModule, BaseFunc

from ..frame import Frame
from . import _ffi_api


def is_defined_in_module(frames):
    if len(frames) > 2:
        maybe_class_frame = frames[2]
        statement_list = maybe_class_frame[4]
        first_statement = statement_list[0]
        line = first_statement.strip()
        if line.startswith("class "):
            return True
        if line.startswith("@") and "ir_module" in line:
            return True
    return False


@_register_object("script.builder.ir.IRModuleFrame")
class IRModuleFrame(Frame):
    ...


def ir_module(f: Optional[Type] = None) -> Union[IRModuleFrame, IRModule]:
    if f is not None:
        # pylint: disable=import-outside-toplevel
        from tvm.script.parse import parse
        from tvm.script.parse.utils import inspect_class_capture

        # pylint: enable=import-outside-toplevel

        if not inspect.isclass(f):
            raise TypeError(f"Expect a class, but got: {f}")

        return parse(f, inspect_class_capture(f))
    return _ffi_api.IRModule()  # pylint: disable=no-member # type: ignore


setattr(ir_module, "dispatch_token", "ir")


def add_function(name: str, func: Optional[BaseFunc] = None, allow_rename: bool = False) -> None:
    return _ffi_api.AddFunction(
        name, func, allow_rename
    )  # pylint: disable=no-member # type: ignore


def update_function(name: str, func: BaseFunc, require_first_define: bool=True) -> None:
    return _ffi_api.UpdateFunction(name, func, require_first_define)  # pylint: disable=no-member # type: ignore
