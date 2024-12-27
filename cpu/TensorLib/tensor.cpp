#include <iostream>
#include <unsupported/Eigen/CXX11/Tensor>
#include <opencv2/opencv.hpp>

class Tensor4D {

public:
    Eigen::Tensor<float, 4> tensor;
    int n; // dimensions (batch size)
    int c; // channels
    int h; // height
    int w; // width

    //4D Tensor Initialization(N,C,H,W)
    Tensor4D(int N, int C, int H, int W)
        : n(N), c(C), h(H), w(W), tensor(N, C, H, W) {
        tensor.setRandom();
    }

    //3D Tensor Initialization(1,C,H,W)
    Tensor4D(int C, int H, int W) : Tensor4D(1, C, H, W) {}

    //2D Tensor Initialization(1,1,H,W)
    Tensor4D(int H, int W) : Tensor4D(1, 1, H, W) {}

    // Accessor for getting a matrix of shape (H, W)
    Eigen::Tensor<float, 2> get_matrix(int n, int c) {
        return tensor.chip(n, 0).chip(c, 0);
    }

    float operator()(int i, int j,int k, int l) {
        return tensor(i,j,k,l);
    }
        // Padding function
    Tensor4D pad(int padH, int padW) {
        Eigen::array<std::pair<int, int>, 4> paddings;
        paddings[0] = std::make_pair(0, 0);  // No padding for batch dimension
        paddings[1] = std::make_pair(0, 0);  // No padding for channel dimension
        paddings[2] = std::make_pair(padH, padH);  // Padding for height dimension
        paddings[3] = std::make_pair(padW, padW);  // Padding for width dimension

        Eigen::Tensor<float, 4> padded_tensor = tensor.pad(paddings);
        return Tensor4D(padded_tensor.dimension(0), padded_tensor.dimension(1), padded_tensor.dimension(2), padded_tensor.dimension(3), padded_tensor);
    }

    void addPadding(int padH, int padW) {
        // Define the padding for each dimension
        Eigen::array<std::pair<int, int>, 4> paddings;
        paddings[0] = std::make_pair(0, 0);  // No padding for batch dimension
        paddings[1] = std::make_pair(0, 0);  // No padding for channel dimension
        paddings[2] = std::make_pair(padH, padH);  // Padding for height dimension
        paddings[3] = std::make_pair(padW, padW);  // Padding for width dimension

        // Apply padding
        Eigen::Tensor<float, 4> padded_tensor = tensor.pad(paddings);

        // Update the original tensor with the padded tensor
        tensor = padded_tensor;

        // Update dimensions
        h = tensor.dimension(2);
        w = tensor.dimension(3);
    }

    void addBias(Tensor4D& bias) {
        // Check if the bias tensor has the correct shape (1, c, 1, 1)
        if (bias.n != 1 || bias.c != c || bias.h != 1 || bias.w != 1) {
            throw std::invalid_argument("Bias tensor must have shape (1, 1, 1, Len), where Len matches the number of channels.");
        }

        // Broadcast the bias to match the dimensions of the main tensor
        Eigen::array<int, 4> broadcast_dims = {n, 1, h, w};
        tensor = tensor + bias.tensor.broadcast(broadcast_dims);
    }
    // Constructor for creating Tensor4D from existing Eigen::Tensor
    Tensor4D(int N, int C, int H, int W, Eigen::Tensor<float, 4> existing_tensor)
        : n(N), c(C), h(H), w(W), tensor(std::move(existing_tensor)) {}

    // Print shape of the tensor
    void print_shape() const {
        std::cout << "Shape: (" 
                  << tensor.dimension(0) << ", " 
                  << tensor.dimension(1) << ", " 
                  << tensor.dimension(2) << ", " 
                  << tensor.dimension(3) << ")" << std::endl;
    }

    // Get each dimension of the tensor
    int dimension(int index) const {
        return tensor.dimension(index);
    }

    // Element-wise tensor operations
    Tensor4D operator+(const Tensor4D& other) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor + other.tensor;
        return result;
    }

    Tensor4D operator-(const Tensor4D& other) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor - other.tensor;
        return result;
    }

    Tensor4D operator*(const Tensor4D& other) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor * other.tensor;
        return result;
    }

    Tensor4D operator/(const Tensor4D& other) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor / other.tensor;
        return result;
    }

    // Scalar operations on tensor
    Tensor4D operator+(float scalar) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor + scalar;
        return result;
    }

    Tensor4D operator-(float scalar) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor - scalar;
        return result;
    }

    Tensor4D operator*(float scalar) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor * scalar;
        return result;
    }

    Tensor4D operator/(float scalar) const {
        Tensor4D result(n,c,h,w);
        result.tensor = tensor / scalar;
        return result;
    }

    // ReLU activation function
    void relu() {
        tensor = tensor.cwiseMax(0.0f);
    }

    // Print each channel of the tensor as a 2D matrix
    void printTensor() const {
        for (int n = 0; n < tensor.dimension(0); ++n) {
            for (int c = 0; c < tensor.dimension(1); ++c) {
                std::cout << "Batch " << n << ", Channel " << c << ":\n";
                for (int h = 0; h < tensor.dimension(2); ++h) {
                    for (int w = 0; w < tensor.dimension(3); ++w) {
                        std::cout << tensor(n, c, h, w) << " ";
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }

    //Concatenation function of 2 tensor with same N,H,W 
    static Tensor4D concatenate(const Tensor4D& t1, const Tensor4D& t2) {
        // Check if N, H, W are equal
        if (t1.n != t2.n || t1.h != t2.h || t1.w != t2.w) {
            throw std::invalid_argument("Batch size (N), height (H), and width (W) must be equal.");
        }

        // New channel dimension
        int new_channels = t1.c + t2.c;

        // Create new Tensor4D with concatenated channels
        Tensor4D result(t1.n, new_channels, t1.h, t1.w);
        result.tensor.slice(Eigen::array<int, 4>({0, 0, 0, 0}), t1.tensor.dimensions()) = t1.tensor;
        result.tensor.slice(Eigen::array<int, 4>({0, t1.c, 0, 0}), t2.tensor.dimensions()) = t2.tensor;

        return result;
    }

    //concat with the original tensor
    Tensor4D concatenate(const Tensor4D& other) const {
        // Check if N, H, W are equal
        if (n != other.n || h != other.h || w != other.w) {
            throw std::invalid_argument("Batch size (N), height (H), and width (W) must be equal.");
        }

        // New channel dimension
        int new_channels = c + other.c;

        // Create new Tensor4D with concatenated channels
        Tensor4D result(n, new_channels, h, w);

        // Define the slices for assignment
        Eigen::array<int, 4> offset1 = {0, 0, 0, 0};
        Eigen::array<int, 4> size1 = {n, c, h, w};
        Eigen::array<int, 4> offset2 = {0, c, 0, 0};
        Eigen::array<int, 4> size2 = {n, other.c, h, w};

        // Assign the values to the result tensor
        result.tensor.slice(offset1, size1) = tensor;
        result.tensor.slice(offset2, size2) = other.tensor;

        return result;
    }

    // Function to get a block (H, W) from given (r, c)
    Eigen::Tensor<float, 2> block(int n, int c, int r, int col, int height, int width) {
        return tensor.chip(n, 0).chip(c, 0).slice(Eigen::array<int, 2>({r, col}), Eigen::array<int, 2>({height, width}));
    }
    
    void saveAsImage(const std::string& filePath) const {
        if (n != 1) {
            throw std::runtime_error("Batch size must be 1 to save as an image.");
        }
        if (c != 1 && c != 3) {
            throw std::runtime_error("Only grayscale (1 channel) or RGB (3 channels) images are supported.");
        }

        // Create an OpenCV matrix
        cv::Mat image(h, w, (c == 1) ? CV_8UC1 : CV_8UC3);

        // Copy data from tensor to the OpenCV matrix
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                if (c == 1) {
                    // Grayscale image
                    image.at<uchar>(i, j) = static_cast<uchar>(tensor(0, 0, i, j) * 255.0f); // Denormalize
                } else if (c == 3) {
                    // RGB image
                    cv::Vec3b& pixel = image.at<cv::Vec3b>(i, j);
                    for (int k = 0; k < 3; ++k) {
                        pixel[k] = static_cast<uchar>(tensor(0, k, i, j) * 255.0f); // Denormalize
                    }
                }
            }
        }

        // Save the image
        if (!cv::imwrite(filePath, image)) {
            throw std::runtime_error("Failed to save image to file: " + filePath);
        }

        std::cout << "Image saved successfully to " << filePath << std::endl;
    }
    
    Tensor4D batchNorm(float epsilon = 1e-5, float gamma = 1.0f, float beta = 0.0f) const {
        // Create output tensor with same dimensions
        Tensor4D output(n, c, h, w);
        
        // For each channel
        for (int channel = 0; channel < c; ++channel) {
            // Calculate mean for this channel
            float channel_mean = 0.0f;
            for (int batch = 0; batch < n; ++batch) {
                for (int row = 0; row < h; ++row) {
                    for (int col = 0; col < w; ++col) {
                        channel_mean += tensor(batch, channel, row, col);
                    }
                }
            }
            channel_mean /= (n * h * w);

            // Calculate variance for this channel
            float channel_variance = 0.0f;
            for (int batch = 0; batch < n; ++batch) {
                for (int row = 0; row < h; ++row) {
                    for (int col = 0; col < w; ++col) {
                        float diff = tensor(batch, channel, row, col) - channel_mean;
                        channel_variance += diff * diff;
                    }
                }
            }
            channel_variance /= (n * h * w);

            // Calculate standard deviation with epsilon for numerical stability
            float std_dev = std::sqrt(channel_variance + epsilon);

            // Normalize, scale, and shift
            for (int batch = 0; batch < n; ++batch) {
                for (int row = 0; row < h; ++row) {
                    for (int col = 0; col < w; ++col) {
                        // Normalize
                        float normalized = (tensor(batch, channel, row, col) - channel_mean) / std_dev;
                        // Scale and shift
                        output.tensor(batch, channel, row, col) = gamma * normalized + beta;
                    }
                }
            }
        }
        
        return output;
    }

    static Tensor4D fromImage(const std::string& imagePath) {
        // Read the image using OpenCV
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (image.empty()) {
            throw std::runtime_error("Failed to read image: " + imagePath);
        }

        // Get image dimensions
        int height = image.rows;
        int width = image.cols;
        int channels = image.channels();

        // Create a Tensor4D with a batch size of 1
        Tensor4D tensor(1, channels, height, width);

        // Normalize and copy image data to the tensor
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                cv::Vec3b pixel = image.at<cv::Vec3b>(h, w);
                for (int c = 0; c < channels; ++c) {
                    tensor.tensor(0, c, h, w) = pixel[c] / 255.0f; // Normalize to [0, 1]
                }
            }
        }
        return tensor;
    }
};

// int main() {
//     // Example usage
//     Tensor4D tensor1(1, 1, 4, 4);
//     Tensor4D tensor3(1, 2, 4, 4);
    
//     // Print shape of tensor
//     tensor1.print_shape();
//     tensor1.printTensor();
//     Tensor4D tensor2 = tensor1 + 1.0f;
//     tensor2.printTensor();
//     Tensor4D padded = tensor2.pad(1,1);
//     padded.printTensor();
//     Tensor4D tensor4 = tensor1.concatenate(tensor3);
//     tensor4.print_shape();


//     return 0;
// }