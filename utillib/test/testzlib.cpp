/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include "util/zlibutil.h"
#include "util/cryptoutil.h"
#include "testzlib.h"

using namespace std::filesystem;
using namespace util::zlib;
using namespace util::crypto;

namespace {

constexpr std::string_view kSourceFile = "k:/test/mdf/ct/cyclelow.mf4";
constexpr std::string_view kTestDir =  "o:/test/util";
constexpr std::string_view kTestFileName = "source.bin";
constexpr std::string_view kDeflateFileName = "source.zip";
constexpr std::string_view kInflateFileName ="source.inf";

bool skip_test_ = false; // Set to true if this test shall be skipped
std::string kTestFile;
std::string kDeflateFile;
std::string kInflateFile;
}

namespace util::test {

void TestZlib::SetUpTestCase() {
  std::error_code er;
  remove_all(kTestDir, er);

  try {
    create_directories(kTestDir);
    if (!exists(kSourceFile)) {
      throw std::runtime_error("Source file missing. Skip test.");
    }
    path source(kSourceFile);

    path dest(kTestDir);
    dest.append(kTestFileName);

    copy_file(source,dest);

    kTestFile = dest.string();

    path deflate(kTestDir);
    deflate.append(kDeflateFileName);
    kDeflateFile = deflate.string();

    path inflate(kTestDir);
    inflate.append(kInflateFileName);
    kInflateFile = inflate.string();

  } catch (const std::exception& err) {
    std::cout << "Skipping Test. Error: " << err.what() << std::endl;
    skip_test_ = true;
  }

}

void TestZlib::TearDownTestCase()
{
  std::error_code err;
  remove_all(kTestDir, err);
}

TEST_F(TestZlib, FileCompress)
{
  if (skip_test_) {
    GTEST_SKIP() << "Skipped Test";
  }

  const auto md5_orig = CreateMd5FileString(kTestFile);

  auto* in = fopen(kTestFile.c_str(), "rb");
  EXPECT_TRUE(in != nullptr);
  auto* out = fopen(kDeflateFile.c_str(), "wb");
  EXPECT_TRUE(out != nullptr);

  EXPECT_TRUE(Deflate(in, out));

  fclose(in);
  fclose(out);

  auto* in1 = fopen(kDeflateFile.c_str(), "rb");
  EXPECT_TRUE(in1 != nullptr);
  auto* out1 = fopen(kInflateFile.c_str(), "wb");
  EXPECT_TRUE(out1 != nullptr);

  EXPECT_TRUE(Inflate(in1, out1));
  fclose(in1);
  fclose(out1);

  const auto md5_dest =  util::crypto::CreateMd5FileString(kInflateFile);

  EXPECT_EQ(md5_orig,md5_dest);
}

TEST_F(TestZlib, ArrayCompressLarge)
{
  ByteArray buf_in(4'000'000, 0);
  for (size_t ii = 0; ii < buf_in.size(); ++ii) {
    buf_in[ii] = ii % 256;
  }

  ByteArray buf_out(4'000'000, 0);
  EXPECT_TRUE(Deflate(buf_in,buf_out));

  ByteArray buf_dest(4'000'000, 0xFF);
  EXPECT_TRUE(Inflate(buf_out,buf_dest));

  bool match = true;
  for (size_t ii = 0; ii < buf_in.size(); ++ii) {
    if (buf_in[ii] != buf_dest[ii]) {
      match = false;
      break;
    }
  }
  EXPECT_TRUE(match);
}

TEST_F(TestZlib, ArrayCompressSmall)
{
  ByteArray buf_in(1,0);
  for (size_t ii = 0; ii < buf_in.size(); ++ii) {
    buf_in[ii] = ii % 256;
  }

  ByteArray buf_out(1, 0);
  EXPECT_TRUE(Deflate(buf_in,buf_out));

  ByteArray buf_dest(1, 0xFF);
  EXPECT_TRUE(Inflate(buf_out,buf_dest));

  bool match = true;
  for (size_t ii = 0; ii < buf_in.size(); ++ii) {
    if (buf_in[ii] != buf_dest[ii]) {
      match = false;
      break;
    }
  }
  EXPECT_TRUE(match);
}

TEST_F(TestZlib, Transpose)
{
  ByteArray data(1000,0); // 10 rows, 100 byte record size
  size_t index = 0;
  for ( size_t row = 0; row < 10; ++row) {
    for (size_t col = 0; col < 100; ++col) {
      data[index] = static_cast<uint8_t>(col);
      ++index;
    }
  }
  ByteArray orig(1000, 0);
  orig = data;

  Transpose( data, 100);

  EXPECT_FALSE(orig == data);

  InvTranspose(data, 100);

  EXPECT_TRUE(orig == data);

  index = 0;
  for ( size_t row = 0; row < 10; ++row) {
    for (size_t col = 0; col < 100; ++col) {
      ASSERT_EQ(data[index], col) << index;
      ++index;
    }
  }

}

} // namespace util::test

