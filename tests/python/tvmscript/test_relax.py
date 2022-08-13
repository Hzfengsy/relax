from tvm.script.builder import ir as I, tir as T, relax as R
import tvm.testing
from tvm import IRModule


def _check(mod: IRModule):
    print(mod.script())


def test_simple_module():
    @I.ir_module
    class TestModule:
        @T.prim_func
        def tir_func(x: T.Buffer((128, 128), "float32"), y: T.Buffer((128, 128), "float32")):
            T.evaluate(0)

        @R.function
        def main(x: R.Tensor((128, 128), "float32")) -> R.Tensor(None, "float32", ndim=2):
            gv0 = R.call_tir(tir_func, x, (128, 128), dtype="float32")
            return gv0

    _check(TestModule)


def test_simple_func():
    @R.function
    def test_func(x: R.Tensor((128, 128), "float32")) -> R.Tensor(None, "float32", ndim=2):
        gv0 = R.call_tir("extern_func", x, (128, 128), dtype="float32")
        return gv0

    _check(test_func)


def test_symbolic_shape():
    @R.function
    def test_func(x: R.Tensor(("m", "n"), "float32")) -> R.Tensor(None, "float32", ndim=2):
        m = T.var("int32", "m")
        n = T.var("int32", "n")
        gv0 = R.call_tir("extern_func", x, (m, n), dtype="float32")
        return gv0

    _check(test_func)


if __name__ == "__main__":
    tvm.testing.main()
