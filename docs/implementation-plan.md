# 一期实施计划

本计划按可验证里程碑推进。每个里程碑都应能单独构建和测试。

## 里程碑 1：工程骨架

目标：

- 建立 C++17 + CMake 工程。
- 生成 `glowinfer` 静态库。
- 生成一个自研测试可执行文件。

建议目录：

- `include/`：公开头文件。
- `src/`：库实现。
- `tests/`：自研测试 runner 和测试用例。
- `examples/`：后续模型推理示例。

完成标准：

- `cmake -S . -B build` 成功。
- `cmake --build build` 成功。
- 测试可执行文件能运行并报告通过或失败。

## 里程碑 2：Tensor

目标：

- 实现 CPU float32 Tensor。
- 支持 owned contiguous tensor 和 view tensor。
- 支持 shape、stride、numel、rank、data access、reshape/view、clone、fill。

完成标准：

- Tensor 基础测试通过。
- 非法 shape、元素数量不匹配、空数据访问等错误可诊断。
- 后续 kernel 可以直接使用 Tensor 作为输入输出容器。

## 里程碑 3：基础 Kernel

先实现简单且容易验证的 kernel：

- ReLU
- Add
- Flatten
- Gemm / Linear

完成标准：

- 每个 kernel 有小尺寸确定性测试。
- shape mismatch 会失败并给出明确错误。
- Gemm 支持一期 ONNX `Gemm` 所需的 alpha、beta、transA、transB 子集。

## 里程碑 4：CV Kernel

实现 VGG16 和 ResNet18 必需的图像算子：

- Conv2D NCHW
- MaxPool2D NCHW
- GlobalAveragePool2D NCHW
- BatchNorm inference NCHW

完成标准：

- Conv 支持 stride、padding、dilation=1、group=1、可选 bias。
- MaxPool 支持 kernel、stride、padding。
- BatchNorm 使用推理公式：`y = scale * (x - mean) / sqrt(var + epsilon) + bias`。
- 每个 kernel 有覆盖 padding、stride 和边界尺寸的测试。

## 里程碑 5：Op、Graph 和 Executor

目标：

- 将 kernel 包装为内部 op。
- 实现 Graph value 管理和 Executor 顺序执行。
- 支持构造小型手写图并执行。

完成标准：

- 手写 `Conv -> Relu -> MaxPool -> Flatten -> Gemm` 图能运行。
- 中间 Tensor shape 符合预期。
- 缺失输入、重复输出和不支持 op 会报错。

## 里程碑 6：固定 ONNX 子集 Importer

目标：

- 自研最小 protobuf wire reader。
- 解析一期所需 ONNX message 字段。
- 将 ONNX node 转换为内部 op。

完成标准：

- 能读取包含 initializer 和少量节点的手工 ONNX fixture。
- 能识别支持的 op 和 attributes。
- 遇到未知 op、dtype、动态 shape 或不支持 attr 时失败。

## 里程碑 7：VGG16 验收

目标：

- 加载 VGG16 ONNX。
- 对单张静态输入执行推理。
- 输出与参考 fixture 在容差内一致。

完成标准：

- top-5 类别顺序与参考一致，或 logits 最大误差在预设容差内。
- 推理过程中所有中间 shape 可追踪。
- 错误路径能定位到具体 op。

## 里程碑 8：ResNet18 验收

目标：

- 加载 ResNet18 ONNX。
- 覆盖 BatchNorm、Add、GlobalAveragePool 和残差路径。

完成标准：

- 输出与参考 fixture 在容差内一致。
- 残差 Add 的输入 shape 校验稳定。
- VGG16 验收仍然通过。

## 后续优化入口

一期正确性稳定后再考虑：

- 自研 tiled GEMM。
- Conv im2col + GEMM。
- SIMD。
- 线程池。
- 简单内存复用。
- graph level shape inference 和常量折叠。
