# 冷启动 Agent 交接说明

## 当前状态

仓库目前处于早期启动阶段。已存在的源码文件很少：

- `include/data/Tensor.h`
- `src/Tensor.cpp`

这些文件可能存在用户本地未提交修改。接手 agent 不应重置或覆盖它们，除非任务明确要求。

## 已确定决策

- 一期目标是 CPU 上推理 VGG16 和 ResNet18。
- 技术基线是 C++17 + CMake。
- 运行时零第三方依赖，核心库只依赖 C++ 标准库。
- kernel 不使用 OpenBLAS，全部自研。
- ONNX 加载不使用 Protobuf，采用自研固定子集 parser。
- 数据默认是 CPU float32 NCHW。
- 首个真实模型验收顺序是先 VGG16，再 ResNet18。
- 实现顺序以正确性为先，性能优化后置。

## 非目标

一期不做以下内容：

- GPU、CUDA、OpenCL、Vulkan 或其他加速后端。
- 训练、反向传播、自动求导。
- 动态图执行。
- 通用 ONNX 支持。
- 量化模型。
- 多 dtype 或多 layout。
- 自动内存规划和复杂图优化。
- 与 PyTorch、TensorFlow、ONNXRuntime 的运行时集成。

## 推荐下一步

1. 新增 CMake 工程骨架，生成 `glowinfer` 静态库和简单测试可执行文件。
2. 实现 Tensor 基础能力：shape、stride、size、owned storage、view、reshape、contiguous 校验。
3. 实现最小测试框架，使用自研断言宏或简单 test runner。
4. 实现第一批标量 kernel：ReLU、Add、Flatten、Gemm。
5. 实现 Conv/MaxPool/BatchNorm/GlobalAveragePool。
6. 实现 op 和 graph/executor。
7. 实现固定 ONNX 子集 importer。
8. 用 VGG16 和 ResNet18 静态 fixture 做验收。

## 工作原则

- 先让每个子系统有明确、窄小、可测的行为。
- 遇到未知 ONNX op、attr、dtype 或动态 shape 时直接报错，不静默忽略。
- 接口先保持简单，不为未来功能提前加入复杂泛型和抽象。
- 性能优化应建立在测试稳定之后。
- 如果某项改动会扩大一期范围，先更新文档或向用户确认。
