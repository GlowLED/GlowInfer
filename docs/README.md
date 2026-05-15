# GlowInfer 文档入口

本目录面向冷启动 agent。目标是让接手者不依赖历史对话，也能理解 GlowInfer 一期的工程边界、关键决策和下一步实现顺序。

## 阅读顺序

1. `agent-brief.md`：先读，用来快速建立当前状态和约束。
2. `architecture.md`：理解 Tensor、kernel、op、graph/executor、ONNX importer 的边界。
3. `implementation-plan.md`：按里程碑推进实现。
4. `onnx-subset.md`：实现或修改 ONNX importer 前必须阅读。
5. `testing-strategy.md`：新增功能时按这里补测试和验收用例。

## 一期目标

GlowInfer 一期目标是在 CPU 上完成传统 CV 模型推理，优先跑通：

1. VGG16
2. ResNet18

一期只覆盖静态图 float32 推理，不覆盖训练、自动求导、动态图、量化或 GPU。

## 核心约束

- 语言和构建：C++17 + CMake。
- 运行时依赖：核心推理库只依赖 C++ 标准库。
- 不使用 OpenBLAS、Protobuf、ONNXRuntime、GTest 作为一期运行时或核心实现依赖。
- kernel 自研，先以正确性优先的标量实现落地。
- ONNX 加载自研，只支持 VGG16 和 ResNet18 所需固定子集。
- Tensor 一期只支持 CPU、float32、NCHW、contiguous 输出。

## 对后续 agent 的要求

- 不要把本项目改造成通用深度学习框架。
- 不要在未确认前引入第三方依赖。
- 不要为了性能提前牺牲接口清晰度和测试覆盖。
- 每次实现都应保持可验证，优先让小模型、小图、小 kernel 先通过测试。
