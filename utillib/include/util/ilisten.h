/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <memory>
#include <vector>

namespace util::log {

class IListen {
 public:
  virtual ~IListen() = default;

  [[nodiscard]] std::string Name() const;
  void Name(const std::string& name);

  [[nodiscard]] std::string Description() const;
  void Description(const std::string& description);

  [[nodiscard]] std::string PreText() const;
  void PreText(const std::string& pre_text);

  [[nodiscard]] std::string HostName() const;
  void HostName(const std::string& host_name);

  [[nodiscard]] uint16_t Port() const;
  void Port(uint16_t port);

  virtual void ListenText(const char* format_text, ... );
  virtual void ListenTextEx(uint64_t ns1970, const std::string& pre_text, const char* format_text, ... );

  virtual void ListenTransmit(uint64_t ns1970, const std::string& pre_text, const std::vector<uint8_t>& buffer, void* hint);
  virtual void ListenReceive(uint64_t ns1970, const std::string& pre_text, const std::vector<uint8_t>& buffer, void* hint);

  [[nodiscard]] virtual bool IsActive() const = 0;
  [[nodiscard]] virtual size_t LogLevel() = 0;

  virtual bool Start();
  virtual bool Stop();
  virtual std::unique_ptr<IListen> Create(const std::string& type, const std::string& share_name);
 protected:
  IListen() = default;

  virtual void AddMessage(uint64_t nano_sec_1970, const std::string& pre_text, const std::string& text) = 0;
  static std::string ParseHex(const std::vector<uint8_t>& buffer);

 private:
  std::string name_;  ///< Display name.
  std::string description_; ///< Description of the functionality.
  std::string pre_text_; ///< Small text between time and text string.
  std::string host_name_ = "127.0.0.1"; ///< Host name
  uint16_t port_ = 0; ///< IP-port to listen on.

};

}



