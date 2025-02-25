/*
 * Copyright (c) 2014 Steffen Kieß
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Image.hpp"

#include <Core/Error.hpp>

#include <fstream>
#include <sstream>

namespace Core {
void writeImagePGM(std::ostream& stream, const uint8_t* data, size_t width,
                   size_t height, const ImageFileOptions& options) {
  uint32_t maxValue = options.maxValue_;
  if (maxValue == 0) maxValue = 255;
  ASSERT(maxValue <= 255);

  // TODO: Check that values are <= maxValue?

  stream << "P5\n"
         << width << " " << height << (options.oneline_ ? " " : "\n")
         << maxValue << "\n";
  stream.write((const char*)data, width * height * sizeof(uint8_t));
}
void writeImagePGM(const Core::Path& filename, const uint8_t* data,
                   size_t width, size_t height,
                   const ImageFileOptions& options) {
  errno = 0;
  std::ofstream stream(filename.string().c_str(), std::ios_base::binary);
  Core::Error::check("open", stream);
  errno = 0;
  writeImagePGM(stream, data, width, height, options);
  Core::Error::check("write", stream);
}
void writeImagePGM(const Core::Path& filename, const std::vector<uint8_t>& data,
                   size_t width, size_t height,
                   const ImageFileOptions& options) {
  ASSERT(data.size() == width * height);
  writeImagePGM(filename, data.data(), width, height, options);
}
void imageFloatToByte8(const std::vector<float>& input,
                       std::vector<uint8_t>& output, uint8_t maxValue) {
  output.resize(input.size());
  for (size_t i = 0; i < input.size(); i++) {
    float val = std::max(0.0f, std::min(1.0f, input[i]));
    int32_t vali = (int32_t)(val * maxValue + 0.5);
    if (vali < 0)
      vali = 0;
    else if (vali > maxValue)
      vali = maxValue;
    output[i] = (uint8_t)vali;
  }
}
void writeImagePGM8(const Core::Path& filename, const std::vector<float>& data,
                    size_t width, size_t height,
                    const ImageFileOptions& options) {
  ImageFileOptions options2 = options;
  if (options2.maxValue_ == 0) options2.maxValue(255);

  ASSERT(options2.maxValue_ <= 255);

  std::vector<uint8_t> buf;
  imageFloatToByte8(data, buf, options2.maxValue_);
  writeImagePGM(filename, buf, width, height, options2);
}

void writeImagePGM(std::ostream& stream, const uint16_t* data, size_t width,
                   size_t height, const ImageFileOptions& options) {
  uint32_t maxValue = options.maxValue_;
  if (maxValue == 0) maxValue = 65535;
  // TODO: Create a 8-bit image in this case?
  ASSERT(maxValue > 255);
  ASSERT(maxValue <= 65535);

  stream << "P5\n"
         << width << " " << height << (options.oneline_ ? " " : "\n")
         << maxValue << "\n";

  std::vector<uint8_t> swappedData(width * height * sizeof(uint16_t));
  size_t count = width * height;
  for (size_t i = 0; i < count; i++) {
    uint16_t value = data[i];
    ASSERT(value <= maxValue);
    swappedData[2 * i] = value >> 8;
    swappedData[2 * i + 1] = value & 0xff;
  }
  stream.write((const char*)swappedData.data(), count * 2);
}
void writeImagePGM(const Core::Path& filename, const uint16_t* data,
                   size_t width, size_t height,
                   const ImageFileOptions& options) {
  errno = 0;
  std::ofstream stream(filename.string().c_str(), std::ios_base::binary);
  Core::Error::check("open", stream);
  errno = 0;
  writeImagePGM(stream, data, width, height, options);
  Core::Error::check("write", stream);
}
void writeImagePGM(const Core::Path& filename, const std::vector<uint16_t>& data,
                   size_t width, size_t height,
                   const ImageFileOptions& options) {
  ASSERT(data.size() == width * height);
  writeImagePGM(filename, data.data(), width, height, options);
}
void imageFloatToByte16(const std::vector<float>& input,
                       std::vector<uint16_t>& output, uint16_t maxValue) {
  output.resize(input.size());
  for (size_t i = 0; i < input.size(); i++) {
    float val = std::max(0.0f, std::min(1.0f, input[i]));
    int32_t vali = (int32_t)(val * maxValue + 0.5);
    if (vali < 0)
      vali = 0;
    else if (vali > maxValue)
      vali = maxValue;
    output[i] = (uint16_t)vali;
  }
}
void writeImagePGM16(const Core::Path& filename, const std::vector<float>& data,
                    size_t width, size_t height,
                    const ImageFileOptions& options) {
  ImageFileOptions options2 = options;
  if (options2.maxValue_ == 0) options2.maxValue(65535);

  ASSERT(options2.maxValue_ > 255);
  ASSERT(options2.maxValue_ <= 65535);

  std::vector<uint16_t> buf;
  imageFloatToByte16(data, buf, options2.maxValue_);
  writeImagePGM(filename, buf, width, height, options2);
}

void writeImagePGM(const Core::Path& filename, const std::vector<float>& data,
                   size_t width, size_t height,
                   const ImageFileOptions& options) {
  if (options.maxValue_ <= 255)
    // This branch will also be taken if options.maxValue_ is not set
    writeImagePGM8(filename, data, width, height, options);
  else
    writeImagePGM16(filename, data, width, height, options);
}

void writeImagePPM(std::ostream& stream, const uint8_t* data, size_t width,
                   size_t height) {
  stream << "P6\n" << width << " " << height << "\n255\n";
  stream.write((const char*)data, width * height * 3);
}
void writeImagePPM(const Core::Path& filename, const uint8_t* data,
                   size_t width, size_t height) {
  std::ofstream stream(filename.string().c_str(), std::ios_base::binary);
  Core::Error::check("open", stream);
  writeImagePPM(stream, data, width, height);
  Core::Error::check("write", stream);
}
void writeImagePPM(const Core::Path& filename, const std::vector<uint8_t>& data,
                   size_t width, size_t height) {
  ASSERT(data.size() == width * height * 3);
  writeImagePPM(filename, data.data(), width, height);
}

void writeImagePPM(std::ostream& stream, const uint16_t* data, size_t width,
                   size_t height, int bpp) {
  ASSERT(bpp > 8 && bpp <= 16);
  stream << "P6\n"
         << width << " " << height << "\n"
         << ((1 << bpp) - 1) << "\n";
  // stream.write ((const char*) data, width * height * 3);
  stream.write((const char*)data, width * height * 3 * 2);
}
void writeImagePPM(const Core::Path& filename, const uint16_t* data,
                   size_t width, size_t height, int bpp) {
  std::ofstream stream(filename.string().c_str(), std::ios_base::binary);
  Core::Error::check("open", stream);
  writeImagePPM(stream, data, width, height, bpp);
  Core::Error::check("write", stream);
}
void writeImagePPM(const Core::Path& filename,
                   const std::vector<uint16_t>& data, size_t width,
                   size_t height, int bpp) {
  ASSERT(data.size() == width * height * 3);
  writeImagePPM(filename, data.data(), width, height, bpp);
}

void imageFloatToByteCol(const std::vector<float>& input,
                         std::vector<uint8_t>& output) {
  output.resize(input.size() * 3);
  for (size_t i = 0; i < input.size(); i++) {
    float val = std::max(0.0f, std::min(1.0f, input[i]));
    int32_t vali = (int32_t)(val * 767 + 0.5);
    //int32_t vali = (int32_t) (val * 511 + 255.5);
    if (vali < 0)
      vali = 0;
    else if (vali > 767)
      vali = 767;
    uint8_t r, g, b;
    if (vali < 256) {
      r = (uint8_t)vali;
      g = b = 0;
    } else if (vali < 512) {
      r = (uint8_t)(255 - (vali - 256));
      g = (uint8_t)(vali - 256);
      b = 0;
    } else {
      r = 0;
      g = (uint8_t)(255 - (vali - 512));
      b = (uint8_t)(vali - 512);
    }
    output[3 * i + 0] = r;
    output[3 * i + 1] = g;
    output[3 * i + 2] = b;
  }
}
void writeImagePPM(const Core::Path& filename, const std::vector<float>& data,
                   size_t width, size_t height) {
  std::vector<uint8_t> buf;
  imageFloatToByteCol(data, buf);
  writeImagePPM(filename, buf, width, height);
}

void readImagePGM(std::istream& stream, std::vector<float>& data, size_t& width,
                  size_t& height) {
  std::string line;
  std::getline(stream, line);
  Core::Error::check("read", stream);
  ASSERT_MSG(line == "P5", "Not a binary pgm image");

  std::size_t val[3];
  int component = 0;
  while (component < 3) {
    std::getline(stream, line);
    Core::Error::check("read", stream);
    if (line.size() > 0 && line[0] == '#') continue;
    std::stringstream str(line);
    while (component < 3) {
      val[component] = 42;
      str >> val[component];
      if (str.eof() && str.fail()) break;
      component++;
      Core::Error::check("read", str);
    }
  }

  width = val[0];
  height = val[1];
  std::size_t count = val[0] * val[1];
  if (val[0]) ASSERT(count / val[0] == val[1]);

  std::vector<uint8_t> bytes(count);
  stream.read((char*)bytes.data(), count);
  Core::Error::check("read", stream);
  int chr = stream.peek();
  Core::Error::check("peek", stream);
  ASSERT_MSG(chr == -1, "Expected EOF");

  data.resize(count);
  for (std::size_t i = 0; i < count; i++) data[i] = bytes[i] / (float)val[2];
}
void readImagePGM(const Core::Path& filename, std::vector<float>& data,
                  size_t& width, size_t& height) {
  errno = 0;
  std::ifstream stream(filename.string().c_str(), std::ios_base::binary);
  Core::Error::check("open", stream);
  errno = 0;
  readImagePGM(stream, data, width, height);
  Core::Error::check("read", stream);
}
}  // namespace Core
