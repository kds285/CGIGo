name: "train"
op {
  input: "data"
  input: "conv1_w"
  input: "conv1_b"
  output: "conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "conv1"
  input: "conv_bn1_s"
  input: "conv_bn1_b"
  input: "conv_bn1_rm"
  input: "conv_bn1_riv"
  output: "conv_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "conv_bn1"
  output: "relu_bn1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "relu_bn1"
  input: "block1_conv1_w"
  input: "block1_conv1_b"
  output: "block1_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block1_conv1"
  input: "block1_bn1_s"
  input: "block1_bn1_b"
  input: "block1_bn1_rm"
  input: "block1_bn1_riv"
  output: "block1_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block1_bn1"
  output: "block1_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block1_relu1"
  input: "block1_conv2_w"
  input: "block1_conv2_b"
  output: "block1_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block1_conv2"
  input: "block1_bn2_s"
  input: "block1_bn2_b"
  input: "block1_bn2_rm"
  input: "block1_bn2_riv"
  output: "block1_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "relu_bn1"
  input: "block1_bn2"
  output: "block1_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block1_sum"
  output: "block1_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block1_relu"
  input: "block2_conv1_w"
  input: "block2_conv1_b"
  output: "block2_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block2_conv1"
  input: "block2_bn1_s"
  input: "block2_bn1_b"
  input: "block2_bn1_rm"
  input: "block2_bn1_riv"
  output: "block2_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block2_bn1"
  output: "block2_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block2_relu1"
  input: "block2_conv2_w"
  input: "block2_conv2_b"
  output: "block2_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block2_conv2"
  input: "block2_bn2_s"
  input: "block2_bn2_b"
  input: "block2_bn2_rm"
  input: "block2_bn2_riv"
  output: "block2_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block1_relu"
  input: "block2_bn2"
  output: "block2_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block2_sum"
  output: "block2_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block2_relu"
  input: "block3_conv1_w"
  input: "block3_conv1_b"
  output: "block3_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block3_conv1"
  input: "block3_bn1_s"
  input: "block3_bn1_b"
  input: "block3_bn1_rm"
  input: "block3_bn1_riv"
  output: "block3_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block3_bn1"
  output: "block3_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block3_relu1"
  input: "block3_conv2_w"
  input: "block3_conv2_b"
  output: "block3_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block3_conv2"
  input: "block3_bn2_s"
  input: "block3_bn2_b"
  input: "block3_bn2_rm"
  input: "block3_bn2_riv"
  output: "block3_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block2_relu"
  input: "block3_bn2"
  output: "block3_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block3_sum"
  output: "block3_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block3_relu"
  input: "block4_conv1_w"
  input: "block4_conv1_b"
  output: "block4_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block4_conv1"
  input: "block4_bn1_s"
  input: "block4_bn1_b"
  input: "block4_bn1_rm"
  input: "block4_bn1_riv"
  output: "block4_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block4_bn1"
  output: "block4_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block4_relu1"
  input: "block4_conv2_w"
  input: "block4_conv2_b"
  output: "block4_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block4_conv2"
  input: "block4_bn2_s"
  input: "block4_bn2_b"
  input: "block4_bn2_rm"
  input: "block4_bn2_riv"
  output: "block4_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block3_relu"
  input: "block4_bn2"
  output: "block4_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block4_sum"
  output: "block4_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block4_relu"
  input: "block5_conv1_w"
  input: "block5_conv1_b"
  output: "block5_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block5_conv1"
  input: "block5_bn1_s"
  input: "block5_bn1_b"
  input: "block5_bn1_rm"
  input: "block5_bn1_riv"
  output: "block5_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block5_bn1"
  output: "block5_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block5_relu1"
  input: "block5_conv2_w"
  input: "block5_conv2_b"
  output: "block5_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block5_conv2"
  input: "block5_bn2_s"
  input: "block5_bn2_b"
  input: "block5_bn2_rm"
  input: "block5_bn2_riv"
  output: "block5_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block4_relu"
  input: "block5_bn2"
  output: "block5_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block5_sum"
  output: "block5_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block5_relu"
  input: "block6_conv1_w"
  input: "block6_conv1_b"
  output: "block6_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block6_conv1"
  input: "block6_bn1_s"
  input: "block6_bn1_b"
  input: "block6_bn1_rm"
  input: "block6_bn1_riv"
  output: "block6_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block6_bn1"
  output: "block6_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block6_relu1"
  input: "block6_conv2_w"
  input: "block6_conv2_b"
  output: "block6_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block6_conv2"
  input: "block6_bn2_s"
  input: "block6_bn2_b"
  input: "block6_bn2_rm"
  input: "block6_bn2_riv"
  output: "block6_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block5_relu"
  input: "block6_bn2"
  output: "block6_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block6_sum"
  output: "block6_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block6_relu"
  input: "block7_conv1_w"
  input: "block7_conv1_b"
  output: "block7_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block7_conv1"
  input: "block7_bn1_s"
  input: "block7_bn1_b"
  input: "block7_bn1_rm"
  input: "block7_bn1_riv"
  output: "block7_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block7_bn1"
  output: "block7_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block7_relu1"
  input: "block7_conv2_w"
  input: "block7_conv2_b"
  output: "block7_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block7_conv2"
  input: "block7_bn2_s"
  input: "block7_bn2_b"
  input: "block7_bn2_rm"
  input: "block7_bn2_riv"
  output: "block7_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block6_relu"
  input: "block7_bn2"
  output: "block7_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block7_sum"
  output: "block7_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block7_relu"
  input: "block8_conv1_w"
  input: "block8_conv1_b"
  output: "block8_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block8_conv1"
  input: "block8_bn1_s"
  input: "block8_bn1_b"
  input: "block8_bn1_rm"
  input: "block8_bn1_riv"
  output: "block8_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block8_bn1"
  output: "block8_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block8_relu1"
  input: "block8_conv2_w"
  input: "block8_conv2_b"
  output: "block8_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block8_conv2"
  input: "block8_bn2_s"
  input: "block8_bn2_b"
  input: "block8_bn2_rm"
  input: "block8_bn2_riv"
  output: "block8_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block7_relu"
  input: "block8_bn2"
  output: "block8_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block8_sum"
  output: "block8_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block8_relu"
  input: "block9_conv1_w"
  input: "block9_conv1_b"
  output: "block9_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block9_conv1"
  input: "block9_bn1_s"
  input: "block9_bn1_b"
  input: "block9_bn1_rm"
  input: "block9_bn1_riv"
  output: "block9_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block9_bn1"
  output: "block9_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block9_relu1"
  input: "block9_conv2_w"
  input: "block9_conv2_b"
  output: "block9_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block9_conv2"
  input: "block9_bn2_s"
  input: "block9_bn2_b"
  input: "block9_bn2_rm"
  input: "block9_bn2_riv"
  output: "block9_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block8_relu"
  input: "block9_bn2"
  output: "block9_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block9_sum"
  output: "block9_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block9_relu"
  input: "block10_conv1_w"
  input: "block10_conv1_b"
  output: "block10_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block10_conv1"
  input: "block10_bn1_s"
  input: "block10_bn1_b"
  input: "block10_bn1_rm"
  input: "block10_bn1_riv"
  output: "block10_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block10_bn1"
  output: "block10_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block10_relu1"
  input: "block10_conv2_w"
  input: "block10_conv2_b"
  output: "block10_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block10_conv2"
  input: "block10_bn2_s"
  input: "block10_bn2_b"
  input: "block10_bn2_rm"
  input: "block10_bn2_riv"
  output: "block10_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block9_relu"
  input: "block10_bn2"
  output: "block10_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block10_sum"
  output: "block10_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block10_relu"
  input: "block11_conv1_w"
  input: "block11_conv1_b"
  output: "block11_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block11_conv1"
  input: "block11_bn1_s"
  input: "block11_bn1_b"
  input: "block11_bn1_rm"
  input: "block11_bn1_riv"
  output: "block11_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block11_bn1"
  output: "block11_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block11_relu1"
  input: "block11_conv2_w"
  input: "block11_conv2_b"
  output: "block11_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block11_conv2"
  input: "block11_bn2_s"
  input: "block11_bn2_b"
  input: "block11_bn2_rm"
  input: "block11_bn2_riv"
  output: "block11_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block10_relu"
  input: "block11_bn2"
  output: "block11_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block11_sum"
  output: "block11_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block11_relu"
  input: "block12_conv1_w"
  input: "block12_conv1_b"
  output: "block12_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block12_conv1"
  input: "block12_bn1_s"
  input: "block12_bn1_b"
  input: "block12_bn1_rm"
  input: "block12_bn1_riv"
  output: "block12_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block12_bn1"
  output: "block12_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block12_relu1"
  input: "block12_conv2_w"
  input: "block12_conv2_b"
  output: "block12_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block12_conv2"
  input: "block12_bn2_s"
  input: "block12_bn2_b"
  input: "block12_bn2_rm"
  input: "block12_bn2_riv"
  output: "block12_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block11_relu"
  input: "block12_bn2"
  output: "block12_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block12_sum"
  output: "block12_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block12_relu"
  input: "block13_conv1_w"
  input: "block13_conv1_b"
  output: "block13_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block13_conv1"
  input: "block13_bn1_s"
  input: "block13_bn1_b"
  input: "block13_bn1_rm"
  input: "block13_bn1_riv"
  output: "block13_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block13_bn1"
  output: "block13_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block13_relu1"
  input: "block13_conv2_w"
  input: "block13_conv2_b"
  output: "block13_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block13_conv2"
  input: "block13_bn2_s"
  input: "block13_bn2_b"
  input: "block13_bn2_rm"
  input: "block13_bn2_riv"
  output: "block13_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block12_relu"
  input: "block13_bn2"
  output: "block13_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block13_sum"
  output: "block13_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block13_relu"
  input: "block14_conv1_w"
  input: "block14_conv1_b"
  output: "block14_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block14_conv1"
  input: "block14_bn1_s"
  input: "block14_bn1_b"
  input: "block14_bn1_rm"
  input: "block14_bn1_riv"
  output: "block14_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block14_bn1"
  output: "block14_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block14_relu1"
  input: "block14_conv2_w"
  input: "block14_conv2_b"
  output: "block14_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block14_conv2"
  input: "block14_bn2_s"
  input: "block14_bn2_b"
  input: "block14_bn2_rm"
  input: "block14_bn2_riv"
  output: "block14_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block13_relu"
  input: "block14_bn2"
  output: "block14_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block14_sum"
  output: "block14_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block14_relu"
  input: "block15_conv1_w"
  input: "block15_conv1_b"
  output: "block15_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block15_conv1"
  input: "block15_bn1_s"
  input: "block15_bn1_b"
  input: "block15_bn1_rm"
  input: "block15_bn1_riv"
  output: "block15_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block15_bn1"
  output: "block15_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block15_relu1"
  input: "block15_conv2_w"
  input: "block15_conv2_b"
  output: "block15_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block15_conv2"
  input: "block15_bn2_s"
  input: "block15_bn2_b"
  input: "block15_bn2_rm"
  input: "block15_bn2_riv"
  output: "block15_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block14_relu"
  input: "block15_bn2"
  output: "block15_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block15_sum"
  output: "block15_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block15_relu"
  input: "block16_conv1_w"
  input: "block16_conv1_b"
  output: "block16_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block16_conv1"
  input: "block16_bn1_s"
  input: "block16_bn1_b"
  input: "block16_bn1_rm"
  input: "block16_bn1_riv"
  output: "block16_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block16_bn1"
  output: "block16_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block16_relu1"
  input: "block16_conv2_w"
  input: "block16_conv2_b"
  output: "block16_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block16_conv2"
  input: "block16_bn2_s"
  input: "block16_bn2_b"
  input: "block16_bn2_rm"
  input: "block16_bn2_riv"
  output: "block16_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block15_relu"
  input: "block16_bn2"
  output: "block16_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block16_sum"
  output: "block16_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block16_relu"
  input: "block17_conv1_w"
  input: "block17_conv1_b"
  output: "block17_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block17_conv1"
  input: "block17_bn1_s"
  input: "block17_bn1_b"
  input: "block17_bn1_rm"
  input: "block17_bn1_riv"
  output: "block17_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block17_bn1"
  output: "block17_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block17_relu1"
  input: "block17_conv2_w"
  input: "block17_conv2_b"
  output: "block17_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block17_conv2"
  input: "block17_bn2_s"
  input: "block17_bn2_b"
  input: "block17_bn2_rm"
  input: "block17_bn2_riv"
  output: "block17_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block16_relu"
  input: "block17_bn2"
  output: "block17_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block17_sum"
  output: "block17_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block17_relu"
  input: "block18_conv1_w"
  input: "block18_conv1_b"
  output: "block18_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block18_conv1"
  input: "block18_bn1_s"
  input: "block18_bn1_b"
  input: "block18_bn1_rm"
  input: "block18_bn1_riv"
  output: "block18_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block18_bn1"
  output: "block18_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block18_relu1"
  input: "block18_conv2_w"
  input: "block18_conv2_b"
  output: "block18_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block18_conv2"
  input: "block18_bn2_s"
  input: "block18_bn2_b"
  input: "block18_bn2_rm"
  input: "block18_bn2_riv"
  output: "block18_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block17_relu"
  input: "block18_bn2"
  output: "block18_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block18_sum"
  output: "block18_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block18_relu"
  input: "block19_conv1_w"
  input: "block19_conv1_b"
  output: "block19_conv1"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block19_conv1"
  input: "block19_bn1_s"
  input: "block19_bn1_b"
  input: "block19_bn1_rm"
  input: "block19_bn1_riv"
  output: "block19_bn1"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block19_bn1"
  output: "block19_relu1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block19_relu1"
  input: "block19_conv2_w"
  input: "block19_conv2_b"
  output: "block19_conv2"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 3
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "block19_conv2"
  input: "block19_bn2_s"
  input: "block19_bn2_b"
  input: "block19_bn2_rm"
  input: "block19_bn2_riv"
  output: "block19_bn2"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block18_relu"
  input: "block19_bn2"
  output: "block19_sum"
  name: ""
  type: "Sum"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
}
op {
  input: "block19_sum"
  output: "block19_relu"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "block19_relu"
  input: "sl_move_conv_w"
  input: "sl_move_conv_b"
  output: "sl_move_conv"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 1
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 0
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "sl_move_conv"
  input: "sl_move_bn_s"
  input: "sl_move_bn_b"
  input: "sl_move_bn_rm"
  input: "sl_move_bn_riv"
  output: "sl_move_bn"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "block19_relu"
  input: "vn_conv_w"
  input: "vn_conv_b"
  output: "vn_conv"
  name: ""
  type: "Conv"
  arg {
    name: "kernel"
    i: 1
  }
  arg {
    name: "exhaustive_search"
    i: 1
  }
  arg {
    name: "pad"
    i: 0
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  engine: "CUDNN"
}
op {
  input: "vn_conv"
  input: "vn_bn_s"
  input: "vn_bn_b"
  input: "vn_bn_rm"
  input: "vn_bn_riv"
  output: "vn_bn"
  name: ""
  type: "SpatialBN"
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "epsilon"
    f: 9.99999974738e-06
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "is_test"
    i: 1
  }
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "momentum"
    f: 0.899999976158
  }
}
op {
  input: "vn_bn"
  output: "vn_relu_1"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "vn_relu_1"
  input: "vn_fc_1_w"
  input: "vn_fc_1_b"
  output: "vn_fc_1"
  name: ""
  type: "FC"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
}
op {
  input: "vn_fc_1"
  output: "vn_relu_2"
  name: ""
  type: "Relu"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "vn_relu_2"
  input: "vn_fc_2_w"
  input: "vn_fc_2_b"
  output: "vn_fc_2"
  name: ""
  type: "FC"
  arg {
    name: "use_cudnn"
    i: 1
  }
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
}
op {
  input: "vn_fc_2"
  output: "tanh"
  name: ""
  type: "Tanh"
  arg {
    name: "order"
    s: "NCHW"
  }
  arg {
    name: "cudnn_exhaustive_search"
    i: 1
  }
  arg {
    name: "ws_nbytes_limit"
    i: 67108864
  }
  engine: "CUDNN"
}
op {
  input: "sl_move_bn"
  output: "softmax"
  name: ""
  type: "Flatten"
}
external_input: "data"
external_input: "conv1_w"
external_input: "conv1_b"
external_input: "conv_bn1_s"
external_input: "conv_bn1_b"
external_input: "conv_bn1_rm"
external_input: "conv_bn1_riv"
external_input: "block1_conv1_w"
external_input: "block1_conv1_b"
external_input: "block1_bn1_s"
external_input: "block1_bn1_b"
external_input: "block1_bn1_rm"
external_input: "block1_bn1_riv"
external_input: "block1_conv2_w"
external_input: "block1_conv2_b"
external_input: "block1_bn2_s"
external_input: "block1_bn2_b"
external_input: "block1_bn2_rm"
external_input: "block1_bn2_riv"
external_input: "block2_conv1_w"
external_input: "block2_conv1_b"
external_input: "block2_bn1_s"
external_input: "block2_bn1_b"
external_input: "block2_bn1_rm"
external_input: "block2_bn1_riv"
external_input: "block2_conv2_w"
external_input: "block2_conv2_b"
external_input: "block2_bn2_s"
external_input: "block2_bn2_b"
external_input: "block2_bn2_rm"
external_input: "block2_bn2_riv"
external_input: "block3_conv1_w"
external_input: "block3_conv1_b"
external_input: "block3_bn1_s"
external_input: "block3_bn1_b"
external_input: "block3_bn1_rm"
external_input: "block3_bn1_riv"
external_input: "block3_conv2_w"
external_input: "block3_conv2_b"
external_input: "block3_bn2_s"
external_input: "block3_bn2_b"
external_input: "block3_bn2_rm"
external_input: "block3_bn2_riv"
external_input: "block4_conv1_w"
external_input: "block4_conv1_b"
external_input: "block4_bn1_s"
external_input: "block4_bn1_b"
external_input: "block4_bn1_rm"
external_input: "block4_bn1_riv"
external_input: "block4_conv2_w"
external_input: "block4_conv2_b"
external_input: "block4_bn2_s"
external_input: "block4_bn2_b"
external_input: "block4_bn2_rm"
external_input: "block4_bn2_riv"
external_input: "block5_conv1_w"
external_input: "block5_conv1_b"
external_input: "block5_bn1_s"
external_input: "block5_bn1_b"
external_input: "block5_bn1_rm"
external_input: "block5_bn1_riv"
external_input: "block5_conv2_w"
external_input: "block5_conv2_b"
external_input: "block5_bn2_s"
external_input: "block5_bn2_b"
external_input: "block5_bn2_rm"
external_input: "block5_bn2_riv"
external_input: "block6_conv1_w"
external_input: "block6_conv1_b"
external_input: "block6_bn1_s"
external_input: "block6_bn1_b"
external_input: "block6_bn1_rm"
external_input: "block6_bn1_riv"
external_input: "block6_conv2_w"
external_input: "block6_conv2_b"
external_input: "block6_bn2_s"
external_input: "block6_bn2_b"
external_input: "block6_bn2_rm"
external_input: "block6_bn2_riv"
external_input: "block7_conv1_w"
external_input: "block7_conv1_b"
external_input: "block7_bn1_s"
external_input: "block7_bn1_b"
external_input: "block7_bn1_rm"
external_input: "block7_bn1_riv"
external_input: "block7_conv2_w"
external_input: "block7_conv2_b"
external_input: "block7_bn2_s"
external_input: "block7_bn2_b"
external_input: "block7_bn2_rm"
external_input: "block7_bn2_riv"
external_input: "block8_conv1_w"
external_input: "block8_conv1_b"
external_input: "block8_bn1_s"
external_input: "block8_bn1_b"
external_input: "block8_bn1_rm"
external_input: "block8_bn1_riv"
external_input: "block8_conv2_w"
external_input: "block8_conv2_b"
external_input: "block8_bn2_s"
external_input: "block8_bn2_b"
external_input: "block8_bn2_rm"
external_input: "block8_bn2_riv"
external_input: "block9_conv1_w"
external_input: "block9_conv1_b"
external_input: "block9_bn1_s"
external_input: "block9_bn1_b"
external_input: "block9_bn1_rm"
external_input: "block9_bn1_riv"
external_input: "block9_conv2_w"
external_input: "block9_conv2_b"
external_input: "block9_bn2_s"
external_input: "block9_bn2_b"
external_input: "block9_bn2_rm"
external_input: "block9_bn2_riv"
external_input: "block10_conv1_w"
external_input: "block10_conv1_b"
external_input: "block10_bn1_s"
external_input: "block10_bn1_b"
external_input: "block10_bn1_rm"
external_input: "block10_bn1_riv"
external_input: "block10_conv2_w"
external_input: "block10_conv2_b"
external_input: "block10_bn2_s"
external_input: "block10_bn2_b"
external_input: "block10_bn2_rm"
external_input: "block10_bn2_riv"
external_input: "block11_conv1_w"
external_input: "block11_conv1_b"
external_input: "block11_bn1_s"
external_input: "block11_bn1_b"
external_input: "block11_bn1_rm"
external_input: "block11_bn1_riv"
external_input: "block11_conv2_w"
external_input: "block11_conv2_b"
external_input: "block11_bn2_s"
external_input: "block11_bn2_b"
external_input: "block11_bn2_rm"
external_input: "block11_bn2_riv"
external_input: "block12_conv1_w"
external_input: "block12_conv1_b"
external_input: "block12_bn1_s"
external_input: "block12_bn1_b"
external_input: "block12_bn1_rm"
external_input: "block12_bn1_riv"
external_input: "block12_conv2_w"
external_input: "block12_conv2_b"
external_input: "block12_bn2_s"
external_input: "block12_bn2_b"
external_input: "block12_bn2_rm"
external_input: "block12_bn2_riv"
external_input: "block13_conv1_w"
external_input: "block13_conv1_b"
external_input: "block13_bn1_s"
external_input: "block13_bn1_b"
external_input: "block13_bn1_rm"
external_input: "block13_bn1_riv"
external_input: "block13_conv2_w"
external_input: "block13_conv2_b"
external_input: "block13_bn2_s"
external_input: "block13_bn2_b"
external_input: "block13_bn2_rm"
external_input: "block13_bn2_riv"
external_input: "block14_conv1_w"
external_input: "block14_conv1_b"
external_input: "block14_bn1_s"
external_input: "block14_bn1_b"
external_input: "block14_bn1_rm"
external_input: "block14_bn1_riv"
external_input: "block14_conv2_w"
external_input: "block14_conv2_b"
external_input: "block14_bn2_s"
external_input: "block14_bn2_b"
external_input: "block14_bn2_rm"
external_input: "block14_bn2_riv"
external_input: "block15_conv1_w"
external_input: "block15_conv1_b"
external_input: "block15_bn1_s"
external_input: "block15_bn1_b"
external_input: "block15_bn1_rm"
external_input: "block15_bn1_riv"
external_input: "block15_conv2_w"
external_input: "block15_conv2_b"
external_input: "block15_bn2_s"
external_input: "block15_bn2_b"
external_input: "block15_bn2_rm"
external_input: "block15_bn2_riv"
external_input: "block16_conv1_w"
external_input: "block16_conv1_b"
external_input: "block16_bn1_s"
external_input: "block16_bn1_b"
external_input: "block16_bn1_rm"
external_input: "block16_bn1_riv"
external_input: "block16_conv2_w"
external_input: "block16_conv2_b"
external_input: "block16_bn2_s"
external_input: "block16_bn2_b"
external_input: "block16_bn2_rm"
external_input: "block16_bn2_riv"
external_input: "block17_conv1_w"
external_input: "block17_conv1_b"
external_input: "block17_bn1_s"
external_input: "block17_bn1_b"
external_input: "block17_bn1_rm"
external_input: "block17_bn1_riv"
external_input: "block17_conv2_w"
external_input: "block17_conv2_b"
external_input: "block17_bn2_s"
external_input: "block17_bn2_b"
external_input: "block17_bn2_rm"
external_input: "block17_bn2_riv"
external_input: "block18_conv1_w"
external_input: "block18_conv1_b"
external_input: "block18_bn1_s"
external_input: "block18_bn1_b"
external_input: "block18_bn1_rm"
external_input: "block18_bn1_riv"
external_input: "block18_conv2_w"
external_input: "block18_conv2_b"
external_input: "block18_bn2_s"
external_input: "block18_bn2_b"
external_input: "block18_bn2_rm"
external_input: "block18_bn2_riv"
external_input: "block19_conv1_w"
external_input: "block19_conv1_b"
external_input: "block19_bn1_s"
external_input: "block19_bn1_b"
external_input: "block19_bn1_rm"
external_input: "block19_bn1_riv"
external_input: "block19_conv2_w"
external_input: "block19_conv2_b"
external_input: "block19_bn2_s"
external_input: "block19_bn2_b"
external_input: "block19_bn2_rm"
external_input: "block19_bn2_riv"
external_input: "sl_move_conv_w"
external_input: "sl_move_conv_b"
external_input: "sl_move_bn_s"
external_input: "sl_move_bn_b"
external_input: "sl_move_bn_rm"
external_input: "sl_move_bn_riv"
external_input: "vn_conv_w"
external_input: "vn_conv_b"
external_input: "vn_bn_s"
external_input: "vn_bn_b"
external_input: "vn_bn_rm"
external_input: "vn_bn_riv"
external_input: "vn_fc_1_w"
external_input: "vn_fc_1_b"
external_input: "vn_fc_2_w"
external_input: "vn_fc_2_b"
