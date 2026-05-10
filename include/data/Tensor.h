#include <armadillo>

namespace glow_infer {

template<typename T>
class Tensor {
public:
    // construct a new empty tensor
    explicit Tensor() = default;

    // construct a tensor with existing data
    explicit Tensor(T* raw_ptr, uint32_t size);
    explicit Tensor(T* raw_ptr, uint32_t rows, uint32_t cols);
    explicit Tensor(T* raw_ptr, uint32_t channels, uint32_t rows, uint32_t cols);
    explicit Tensor(T* raw_ptr, const std::vector<uint32_t>& shapes);

    // construct a tensor with only shapes(but no data)
    explicit Tensor(uint32_t size);
    explicit Tensor(uint32_t rows, uint32_t cols);
    explicit Tensor(uint32_t channels, uint32_t rows, uint32_t cols);
    explicit Tensor(const std::vector<uint32_t>& shapes);

    // get basic attrs of a tensor
    uint32_t channels() const;
    uint32_t rows() const;
    uint32_t cols() const;
    uint32_t size() const;
    std::vector<uint32_t> shapes() const;
    const std::vector<uint32_t>& raw_shapes();
    T* raw_ptr();
    const T* raw_ptr() const;

    bool empty() const;

    // about data
    arma::Cube<T>& data() const;
    const arma::Cube<T>& data() const;
    void set_data(const arma::Cube<T>& data);
    
    const T at(uint32_t channels, uint32_t rows, uint32_t cols) const;
    T& at(uint32_t channels, uint32_t rows, uint32_t cols);


private:
    std::vector<uint32_t> _raw_shapes;
    arma::Cube<T> _data;
};

template <>
class Tensor<float> {
    
};


}