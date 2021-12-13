/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <filesystem>
#include <array>
#include <cstdio>
#include <openssl/md5.h>
#include "util/cryptoutil.h"
#include "util/logstream.h"

namespace util::crypto {

bool CreateMd5FileChecksum(const std::string &file, std::vector<uint8_t> &md5) {
  bool ok = false;
  if (md5.size() != MD5_DIGEST_LENGTH) {
    md5.resize(MD5_DIGEST_LENGTH);
  }

  try {
    std::filesystem::path p(file);
    if (std::filesystem::exists(p)) {

      std::FILE *f = fopen(file.c_str(), "rb");
      if (f != nullptr) {
        std::array<uint8_t, 10000> temp{};
        MD5_CTX ctx{};
        auto md_ok = MD5_Init(&ctx);
        for (auto bytes = std::fread(temp.data(), sizeof(uint8_t), temp.size(), f);
             bytes > 0 && md_ok;
             bytes = std::fread(temp.data(), sizeof(uint8_t), temp.size(), f)) {
          md_ok = MD5_Update(&ctx, temp.data(), bytes);
        }
        fclose(f);
        if (md_ok) {
          md_ok = MD5_Final(md5.data(), &ctx);
          ok = md_ok;
        } else {
          log::LOG_ERROR() << "Failed to create MD5 file checksum. File: " << file
                           << ". Error: OpenSSL failure";
        }
      } else {
        log::LOG_ERROR() << "Failed to create MD5 file checksum. File: " << file
                         << ". Error: Failed to open the file";
      }

    } else {
      log::LOG_ERROR() << "Failed to create MD5 file checksum. File: " << file
                       << ". Error: The file doesn't exist";
    }

  } catch (const std::exception &error) {
    log::LOG_ERROR() << "Failed to create MD5 file checksum. File: " << file
                     << ". Error: " << error.what();
    ok = false;
  }
  return ok;
}

bool CreateMd5FileString(const std::string& file, std::string& md5) {
  std::vector<uint8_t> checksum(MD5_DIGEST_LENGTH,0);
  const auto ok = CreateMd5FileChecksum(file, checksum);

  if (ok) {
    std::ostringstream s;
    for (auto byte : checksum ) {
      s << std::uppercase <<  std::setfill('0')
        << std::setw(2) << std::hex << static_cast<uint16_t>(byte);
    }
    md5 = s.str();
  }
  return ok;
}

}

