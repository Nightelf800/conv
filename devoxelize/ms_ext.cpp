#include <string.h>
#include <torch/extension.h> // 头文件引用部分

int8_t GetDtype(const std::string &dtypes) {
    int8_t type = 6;
    std::unordered_map<std::string, int8_t> m {
        {"uint8", 0}, {"int8", 1}, {"int16", 2}, {"int32", 3}, {"int64", 4}, {"float16", 5}, {"float32", 6}, {"float64", 7}};
    if (m.count(dtypes)) {
        type = m[dtypes];
    }
    return type;
}

std::vector<at::Tensor> get_torch_tensors(int nparam, void** params, int* ndims, int64_t** shapes, const char** dtypes, c10::Device device) {
    std::vector<at::Tensor> tensors;
    for (int i = 0; i < nparam; i++) {
        std::vector<int64_t> size;
        for (int j = 0; j < ndims[i]; j++) {
            size.push_back(shapes[i][j]);
        }
        int8_t type = GetDtype(dtypes[i]);
        // 注意：这里device设置为kCPU时跑的CPU算子，设置为kCUDA时跑的GPU算子，其他device不在使用范围内
        auto option = at::TensorOptions().dtype(static_cast<c10::ScalarType>(type)).device(device);
        tensors.emplace_back(at::from_blob(params[i], size, option));
    }
    return tensors;
}

void output_memcpy(void* output, const torch::Tensor &t) {
    memcpy(output, t.data_ptr(), t.element_size() * t.numel());
}
