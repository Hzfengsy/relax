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
"""TVM Script TIR Buffer"""
from numbers import Integral

from tvm.ir import Array, PrimExpr, Range, PrimType
from tvm.runtime import DataType, Object
from tvm.relax.expr import Var

from . import _ffi_api


class Tensor_:
    def __call__(
        self,
        shape,
        dtype,
    ) -> Var:
        # TODO(@siyuan): support runtime dep shape.
        return _ffi_api.Tensor(shape, dtype)  # pylint: disable=no-member # type: ignore

    def __getitem__(self, keys) -> Var:
        return self(*keys)  # pylint: disable=no-member # type: ignore


Tensor = Tensor_()
