/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <cstdint>
#include <cstring>
#include <zlib.h>
#include "util/zlibutil.h"
namespace
{
constexpr size_t kZlibChunk = 16384;
}

namespace util::zlib {


bool Deflate(FILE *in, FILE *out) {
  if (in == nullptr || out == nullptr) {
    return false;
  }

  z_stream s {};
  std::vector<uint8_t> buf_in(kZlibChunk, 0);
  std::vector<uint8_t> buf_out(kZlibChunk,0);

  auto ret = deflateInit(&s,  Z_DEFAULT_COMPRESSION);
  if (ret != Z_OK) {
    return false;
  }

  /* compress until end of file */
  int flush;
  do {
    s.avail_in = static_cast<uInt>(fread(buf_in.data(), 1, kZlibChunk, in));
    if (ferror(in)) {
      deflateEnd(&s);
      return false;
    }

    flush = feof(in) ? Z_FINISH : Z_NO_FLUSH;
    s.next_in = buf_in.data();

    /* run deflate() on input until output buffer not full, finish
       compression if all the sources has been read in */
    do {
      s.avail_out = kZlibChunk;
      s.next_out = buf_out.data();
      ret = deflate(&s, flush);    /* no bad return value */
      if (ret == Z_STREAM_ERROR) {  /* state not clobbered */
        return false;
      }
      const auto have = kZlibChunk - s.avail_out;
      if (fwrite(buf_out.data(), 1, have, out) != have || ferror(out)) {
        deflateEnd(&s);
        return false;
      }
    } while (s.avail_out == 0);
    if (s.avail_in != 0) {     /* all input will be used */
      return false;
    }
    /* done when last data in file processed */
  } while (flush != Z_FINISH);
  /* stream will be complete */
  if (ret != Z_STREAM_END) {     /* all input will be used */
    return false;
  }

  /* clean up and return */
  deflateEnd(&s);
  return true;
}


bool Deflate(const ByteArray& buf_in, ByteArray& buf_out) {
  if (buf_in.empty() || buf_out.empty()) {
    return false;
  }
  if (buf_out.size() < 100) {
    buf_out.resize(100,0);
  }

  z_stream s {};
  auto ret = deflateInit(&s,  Z_DEFAULT_COMPRESSION);
  if (ret != Z_OK) {
    return false;
  }

  s.avail_in = static_cast<uInt>(buf_in.size());
  s.next_in = const_cast<Bytef*>(buf_in.data());

  s.avail_out = static_cast<uInt>(buf_out.size());
  s.next_out = const_cast<Bytef*>(buf_out.data());
  ret = deflate(&s, Z_FINISH);    /* no bad return value */
  if (ret == Z_STREAM_ERROR) {  /* state not clobbered */
    return false;
  }
  const auto compress = static_cast<uInt>(buf_out.size()) - s.avail_out;
  /* clean up and return */
  deflateEnd(&s);
  buf_out.resize(compress);
  return ret == Z_STREAM_END;
}

bool Inflate(std::FILE* in, std::FILE* out)
{
  if (in == nullptr || out == nullptr) {
    return false;
  }
  // Inflate the input file to the output file
  z_stream o{};
  ByteArray buf_in(kZlibChunk, 0);
  ByteArray buf_out(kZlibChunk,0);
  auto ret = inflateInit(&o);
  if (ret != Z_OK) {
    return false;
  }


  /* decompress until deflate stream ends or end of file */
  do {
    o.avail_in = static_cast<uInt>(fread(buf_in.data(), 1, kZlibChunk, in));
    if (ferror(in)) {
      inflateEnd(&o);
      return false;
    }
    if (o.avail_in == 0) {
      break;
    }
    o.next_in = buf_in.data();

    /* run inflate() on input until output buffer not full */
    do {
      o.avail_out = kZlibChunk;
      o.next_out = buf_out.data();
      ret = inflate(&o, Z_NO_FLUSH);

      switch (ret) {
        case Z_STREAM_ERROR:
          return false;

        case Z_NEED_DICT:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
          inflateEnd(&o);
          return false;

        default:
          break;
      }
      const auto have = kZlibChunk - o.avail_out;
      if (fwrite(buf_out.data(), 1, have, out) != have || ferror(out)) {
        inflateEnd(&o);
        return false;
      }
    } while (o.avail_out == 0);
  } while (ret != Z_STREAM_END);

  /* clean up and return */
  inflateEnd(&o);
  return ret == Z_STREAM_END;
}

bool Inflate(std::FILE *in, std::FILE *out, uint64_t nof_bytes) {
  if (in == nullptr || out == nullptr) {
    return false;
  }
  // Inflate the input file to the output file
  z_stream o{};
  ByteArray buf_in(kZlibChunk, 0);
  ByteArray buf_out(kZlibChunk,0);
  auto ret = inflateInit(&o);
  if (ret != Z_OK) {
    return false;
  }


  uint64_t count = 0;
  /* decompress until deflate stream ends or end of file */
  do {
    if (count >= nof_bytes) {
      break; // Ready
    }

    size_t bytes_to_read = kZlibChunk;
    if (count + kZlibChunk > nof_bytes) {
      bytes_to_read = nof_bytes - count;
    }

    o.avail_in = static_cast<uInt>(fread(buf_in.data(), 1, bytes_to_read, in));
    if (ferror(in)) {
      inflateEnd(&o);
      return false;
    }
    if (o.avail_in == 0) {
      break;
    }
    o.next_in = buf_in.data();
    count += bytes_to_read;

    /* run inflate() on input until output buffer not full */
    do {
      o.avail_out = kZlibChunk;
      o.next_out = buf_out.data();
      ret = inflate(&o, Z_NO_FLUSH);

      switch (ret) {
        case Z_STREAM_ERROR:
          return false;

        case Z_NEED_DICT:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
          inflateEnd(&o);
          return false;

        default:
          break;
      }
      const auto have = kZlibChunk - o.avail_out;
      if (fwrite(buf_out.data(), 1, have, out) != have || ferror(out)) {
        inflateEnd(&o);
        return false;
      }
    } while (o.avail_out == 0);
  } while (ret != Z_STREAM_END);

  /* clean up and return */
  inflateEnd(&o);
  return ret == Z_STREAM_END;
}

bool Inflate(const ByteArray& buf_in, ByteArray& buf_out)
{
  if (buf_in.empty() || buf_out.empty()) {
    return false;
  }
  // Inflate the input file to the output file
  z_stream o{};
  auto ret = inflateInit(&o);
  if (ret != Z_OK) {
    return false;
  }

  o.avail_in = static_cast<uInt>(buf_in.size());
  o.next_in = const_cast<Bytef*>(buf_in.data());
  o.avail_out = static_cast<uInt>(buf_out.size());
  o.next_out = buf_out.data();
  ret = inflate(&o, Z_NO_FLUSH);

  switch (ret) {
    case Z_STREAM_ERROR:
      return false;

    case Z_NEED_DICT:
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      inflateEnd(&o);
      return false;

    default:
      break;
  }
  if (o.avail_out > 0) {
    const auto iCompress = static_cast<uInt>(buf_out.size()) - o.avail_out;
    buf_out.resize(iCompress);
  }
  if (o.avail_in > 0) {
    inflateEnd(&o);
    return false;
  }
  inflateEnd(&o);
  return ret == Z_STREAM_END;
}

bool Inflate(const ByteArray& buf_in, std::FILE* to_file)
{
  if (buf_in.empty() || to_file == nullptr) {
    return false;
  }


  // Inflate the input file to the output file
  z_stream o{};

  ByteArray buf_out(kZlibChunk,0);
  auto ret = inflateInit(&o);
  if (ret != Z_OK) {
    return false;
  }
  o.avail_in = static_cast<uInt>(buf_in.size());
  o.next_in = const_cast<Bytef*>(buf_in.data());
     /* run inflate() on input until output buffer not full */
   do {
     o.avail_out = kZlibChunk;
     o.next_out = buf_out.data();
     ret = inflate(&o, Z_NO_FLUSH);

    switch (ret) {
      case Z_STREAM_ERROR:
        return false;

      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
        inflateEnd(&o);
        return false;

      default:
        break;
    }
    const auto have = kZlibChunk - o.avail_out;
    if (fwrite(buf_out.data(), 1, have, to_file) != have || ferror(to_file)) {
      inflateEnd(&o);
      return false;
    }
  } while (o.avail_out == 0);

  /* clean up and return */
  inflateEnd(&o);
  return ret == Z_STREAM_END;
}

void Transpose(ByteArray& data, size_t record_size)
{
  if (record_size == 0) {
    return;
  }

  ByteArray temp(data);
  const size_t rows = data.size() / record_size;
  const size_t columns = record_size;
  for (size_t row = 0; row < rows; ++row) {
    for (size_t column = 0; column < columns; ++column) {
      const size_t from = row * columns + column;
      const size_t to = column * rows + row;
      data[to] = temp[from];
    }
  }
}

void InvTranspose(ByteArray& data, size_t record_size)
{
  if (record_size == 0) {
    return;
  }
  ByteArray temp(data);
  const size_t rows = data.size() / record_size;
  const size_t columns = record_size;
  for (size_t row = 0; row < rows; ++row) {
    for (size_t column = 0; column < columns; ++column) {
      const size_t to = row * columns + column;
      const size_t from = column * rows + row;
      data[to] = temp[from];
    }
  }
}


}
