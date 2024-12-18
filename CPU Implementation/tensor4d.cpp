#include<iostream>
#include<vector>
#include<Eigen/Dense>

class Tensor4D {
private:
    std::vector<std::vector<Eigen::MatrixXf>> tensor4d;
    int n; // dimensions (batch size)
    int c; // channels
    int h; // height
    int w; // width
    void checkDimensionsMatch(const Tensor4D& other) const {
        if (n != other.n || c != other.c || h != other.h || w != other.w) {
            throw std::invalid_argument("Tensor dimensions do not match for the operation");
        }
    }

public:
    // Constructor for 4D Tensor (N, C, H, W)
    Tensor4D(int dimensions, int channels, int height, int width)
        : n(dimensions), c(channels), h(height), w(width) {
        tensor4d.resize(n);  // Resize outer vector to hold 'n' batches
        for (int i = 0; i < n; ++i) {
            tensor4d[i].resize(c);  // Resize each batch to hold 'c' channels
            for (int j = 0; j < c; ++j) {
                tensor4d[i][j] = Eigen::MatrixXf::Zero(h, w);  // Initialize each matrix to zeros
            }
        }
    }

    void resizeMatrices(int new_height, int new_width) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                tensor4d[i][j].conservativeResize(new_height, new_width);  // Resize while preserving existing data
                tensor4d[i][j]= Eigen::MatrixXf::Zero(new_height, new_width);
            }
        }
    }

    void resizeTensor(int new_dim, int new_channel) {
        for (int i = 0; i < new_dim; ++i) {
            for (int j = 0; j < new_channel; ++j) {
                tensor4d[i][j].conservativeResize(h, w);  // Resize while preserving existing data
            }
        }
    }

    // Constructor for 3D Tensor (C, H, W)
    Tensor4D(int channels, int height, int width)
        : Tensor4D(1, channels, height, width) {}

    // Accessor for the tensor data
    Eigen::MatrixXf& operator()(int i, int j) {
        return tensor4d[i][j];
    }

    const Eigen::MatrixXf& operator()(int i, int j) const {
        return tensor4d[i][j];
    }

    // Display function to print the contents of the tensor
    void print() const {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                std::cout << "Tensor4D[" << i << "][" << j << "]:\n";
                std::cout << tensor4d[i][j] << "\n\n";
            }
        }
    }

     size_t dimension(size_t index) const {
    switch(index) {
        case 0: return n;
        case 1: return c;
        case 2: return h;
        case 3: return w;
        default: throw std::out_of_range("Invalid dimension index");
         }
    }

    // Overload the addition operator (Tensor4D + Tensor4D)
    Tensor4D operator+(const Tensor4D& other) const {
        checkDimensionsMatch(other);
        Tensor4D result(n, c, h, w);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                result.tensor4d[i][j] = tensor4d[i][j] + other.tensor4d[i][j];
            }
        }
        return result;
    }

    // Overload the subtraction operator (Tensor4D - Tensor4D)
    Tensor4D operator-(const Tensor4D& other) const {
        checkDimensionsMatch(other);
        Tensor4D result(n, c, h, w);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                result.tensor4d[i][j] = tensor4d[i][j] - other.tensor4d[i][j];
            }
        }
        return result;
    }

    // Overload the elementwise multiplication operator (Tensor4D * Tensor4D)
    Tensor4D operator*(const Tensor4D& other) const {
        checkDimensionsMatch(other);
        Tensor4D result(n, c, h, w);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                result.tensor4d[i][j] = tensor4d[i][j].cwiseProduct(other.tensor4d[i][j]);
            }
        }
        return result;
    }

    // Overload the scalar addition operator (Tensor4D + scalar)
    Tensor4D operator+(float scalar) const {
        Tensor4D result(n, c, h, w);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                result.tensor4d[i][j] = tensor4d[i][j] * scalar;
            }
        }
        return result;
    }

    // Overload the scalar subtraction operator (Tensor4D - scalar)
    Tensor4D operator-(float scalar) const {
        Tensor4D result(n, c, h, w);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                result.tensor4d[i][j] = tensor4d[i][j] * scalar;
            }
        }
        return result;
    }

    // Overload the scalar multiplication operator (Tensor4D * scalar)
    Tensor4D operator*(float scalar) const {
        Tensor4D result(n, c, h, w);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                result.tensor4d[i][j] = tensor4d[i][j] * scalar;
            }
        }
        return result;
    }

    // Overload the scalar division operator (Tensor4D / scalar)
    Tensor4D operator/(float scalar) const {
        Tensor4D result(n, c, h, w);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < c; ++j) {
                result.tensor4d[i][j] = tensor4d[i][j] / scalar;
            }
        }
        return result;
    }



};

// int main() {
//     int N = 1;  // Batch size
//     int C = 1;  // Channels
//     int H = 3;  // Height
//     int W = 3;  // Width

//     Tensor4D input_tensor1(N, C, H, W);
//     Tensor4D input_tensor2(N, C, H, W);

//     // Initialize one of the matrices with random values
//     input_tensor1(0, 0) = Eigen::MatrixXf::Random(H, W);
//     input_tensor2(0, 0) = Eigen::MatrixXf::Random(H, W);

//     input_tensor2(0, 0)(0,0) = 100;
//     input_tensor2(0, 0)(0,1) = 200;

//     Tensor4D add  = input_tensor1 + input_tensor2;

//     std::cout << "Initialized Tensor4D:\n";
//     std::cout << add.dimension(0)<<"\n";
//     std::cout << add.dimension(1)<<"\n";
//     std::cout << add.dimension(2)<<"\n";
//     std::cout << add.dimension(3)<<"\n";
//     add.print();

//     return 0;
// }