# 测试策略

GlowInfer 一期保持运行时零依赖。测试也优先使用自研轻量 test runner，避免引入 GTest 等框架。

## 测试基础设施

建议提供一个简单测试可执行文件：

- 注册测试函数。
- 提供 `EXPECT_EQ`、`EXPECT_NEAR`、`EXPECT_TRUE` 之类的轻量断言宏。
- 测试失败时输出文件、行号、表达式和实际值。
- 支持按测试名称过滤，便于调试单个 kernel。

测试代码可以放在 `tests/`，不进入核心库。

## Tensor 测试

覆盖：

- shape、rank、numel。
- contiguous stride 计算。
- owned tensor 数据读写。
- view tensor 不拥有数据。
- reshape/view 元素数量一致性。
- clone 后数据独立。
- fill 和 element access。
- 非法 shape 和越界访问。

## Kernel 测试

每个 kernel 都应有小尺寸确定性输入和人工可检查输出。

覆盖：

- ReLU 正负值。
- Add 相同 shape 和 shape mismatch。
- Flatten axis。
- Gemm 的 transA、transB、alpha、beta。
- Conv 的 padding、stride、bias。
- MaxPool 的 padding、stride。
- GlobalAveragePool 的多 channel 输入。
- BatchNorm epsilon 和 scale/bias。

数值比较使用绝对误差和相对误差组合。float32 初期建议容差从 `1e-4` 开始，具体模型验收可单独设定。

## Graph 测试

构造小型手写图：

- `Relu -> Add`
- `Gemm`
- `Conv -> Relu -> MaxPool -> Flatten -> Gemm`

覆盖：

- 输入绑定。
- initializer 读取。
- op 顺序执行。
- 中间 shape。
- 缺失输入。
- 重复输出。
- unsupported op。

## ONNX Parser 测试

使用小型静态 fixture：

- 只含一个 initializer 的 ONNX。
- `Conv -> Relu` 小图。
- `Gemm` 小图。
- 带 unsupported op 的错误 fixture。
- 带 unsupported dtype 的错误 fixture。

fixture 应尽量小，便于人工定位 parser 问题。

## 模型验收测试

VGG16 和 ResNet18 验收使用静态输入和静态参考输出。

建议保存：

- 输入 tensor fixture。
- 输出 logits fixture。
- top-5 参考结果。
- 模型文件来源和导出参数说明。

核心库和运行时不依赖 Python、NumPy 或 ONNXRuntime。可以在仓库外使用这些工具生成参考 fixture，但生成工具不是 GlowInfer 一期运行时的一部分。

## 性能测试

一期不以性能为主要验收标准，但可以记录基础指标：

- 单次推理耗时。
- 每个 op 耗时。
- Tensor 分配次数。

性能测试不能替代数值正确性测试。
