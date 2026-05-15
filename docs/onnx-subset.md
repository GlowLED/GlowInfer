# 一期 ONNX 子集

GlowInfer 一期 ONNX importer 是固定子集 loader，不是通用 ONNX 实现。目标是支持常见 VGG16 和 ResNet18 float32 静态图推理。

## 支持的模型特征

- 静态 shape。
- float32 tensor。
- initializer 存储权重和 bias。
- 节点按拓扑可执行顺序排列。
- opset 11 或 13 的常见 CV 导出形式。

## 支持的 TensorProto 数据

一期支持：

- `data_type = FLOAT`。
- `dims` 为静态非负整数。
- 数据来自 `raw_data`。
- 数据来自 repeated float 字段。

一期不支持：

- float16、bfloat16、int8、uint8、int64 等其他 dtype。
- external data。
- sparse tensor。
- string tensor。
- 动态维度。

## 支持的 Op

### Conv

支持：

- NCHW 输入。
- weight 格式 `[out_channels, in_channels, kernel_h, kernel_w]`。
- 可选 bias。
- `strides`。
- `pads`。
- `dilations = [1, 1]`。
- `group = 1`。

不支持：

- grouped conv。
- depthwise conv。
- dilation 大于 1。
- auto_pad。

### Relu

支持标准逐元素 ReLU。

### MaxPool

支持：

- NCHW 输入。
- `kernel_shape`。
- `strides`。
- `pads`。

不支持：

- indices 输出。
- ceil mode。
- dilation。

### GlobalAveragePool

支持 NCHW 输入，输出 shape 为 `[N, C, 1, 1]`。

### BatchNormalization

支持推理模式：

- input。
- scale。
- bias。
- mean。
- var。
- epsilon。

不支持训练模式和多输出统计值。

### Add

一期只支持相同 shape 的逐元素 Add。

暂不支持通用广播。如果 ResNet18 导出中出现可安全处理的简单广播，需要先更新本文档再实现。

### Flatten

支持 `axis`，用于将卷积输出转换为 Gemm 输入。

### Gemm

支持：

- A、B、可选 C。
- `alpha`。
- `beta`。
- `transA`。
- `transB`。

一期只实现 VGG16/ResNet18 常见导出所需组合。

## 必要 Protobuf 字段

自研 parser 只需要解析 ONNX 中必要 message 和字段：

- ModelProto：graph、opset_import。
- GraphProto：node、input、output、initializer。
- NodeProto：input、output、op_type、name、attribute。
- TensorProto：dims、data_type、name、raw_data、float_data。
- AttributeProto：name、type、ints、floats、i、f。
- ValueInfoProto：name、type、tensor_type、shape。

未知字段可以跳过。已知字段如果类型不符合预期，应报错。

## 错误策略

importer 必须显式拒绝：

- 未知 op。
- 未知或不支持 dtype。
- 动态 shape。
- control flow op。
- quantization op。
- training op。
- external data。
- unsupported attribute。

不要静默忽略未知语义。静默忽略会让数值错误更难定位。
