# 一期架构

GlowInfer 一期采用分层结构：Tensor 提供数据容器，kernel 提供底层数值实现，op 包装计算图节点，graph/executor 管理执行顺序，ONNX importer 负责把固定 ONNX 子集转换为内部图。

## 数据流

```text
ONNX 文件
  -> ONNX importer
  -> 内部 Graph
  -> Executor
  -> Op::forward
  -> kernel
  -> Tensor 输出
```

## Tensor 层

Tensor 是一期所有算子的基础数据结构。

一期默认能力：

- 设备：CPU only。
- dtype：float32 only。
- layout：NCHW 为主。
- 存储：owned contiguous storage 为默认输出形式。
- shape：静态 shape，维度使用非负整数。
- stride：支持记录 stride，用于 view 和 reshape 校验。

Tensor 应提供：

- shape、stride、rank、numel 查询。
- `float*` 和 `const float*` 数据访问。
- owned tensor 构造和 view tensor 构造。
- reshape/view，要求元素数量一致。
- clone、fill、基础 element access。
- contiguous 检查。

一期不要求 Tensor 支持：

- 多 dtype。
- GPU memory。
- 自动广播 view。
- 复杂 slicing。
- 引用计数以外的高级内存池。

## Kernel 层

kernel 是具体数值计算实现，不直接理解计算图。

一期 kernel：

- `relu`
- `add`
- `flatten`
- `gemm` / `linear`
- `conv2d_nchw`
- `maxpool2d_nchw`
- `global_average_pool2d_nchw`
- `batchnorm_inference_nchw`

实现策略：

- 第一版使用清晰的标量循环。
- 所有 shape 校验放在 kernel 入口或 op 的 shape inference 中。
- 错误信息应包含 op/kernel 名称和关键 shape。
- 后续优化可以替换内部实现，但不改变上层接口。

## Op 层

Op 是 graph 中的计算节点，负责把 Tensor 输入映射为 Tensor 输出。

建议接口能力：

- op name。
- op type。
- input value names。
- output value names。
- attribute storage。
- `infer_shape`。
- `forward`。

Op 层负责：

- 解析并保存 op 属性。
- 调用对应 kernel。
- 将 ONNX 语义转换为 GlowInfer 内部语义。

Op 层不负责：

- 文件解析。
- 全局执行调度。
- 内存池优化。

## Graph 和 Executor

Graph 保存模型结构：

- 输入 value。
- 输出 value。
- initializer。
- op 节点列表。
- value 到 Tensor 或 metadata 的映射。

Executor 按拓扑顺序执行：

1. 绑定输入 Tensor。
2. 加载 initializer。
3. 依次执行 op。
4. 收集 graph output。

一期可以假设 ONNX 节点顺序已经是可执行顺序，但 importer 应在缺少输入或重复输出时给出错误。

## ONNX Importer

ONNX importer 只服务一期模型，不做通用 ONNX 框架。

职责：

- 读取 ONNX protobuf wire format 中必要字段。
- 提取 graph input、output、initializer 和 node。
- 将支持的 ONNX op 映射为内部 op。
- 解析必要 attributes。
- 拒绝未知或不支持内容。

不支持内容必须显式失败，包括动态 shape、未知 dtype、未知 op、control flow 和 quantization。
