/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

/**
 * Defines an interface against the listen functionality.
 */
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
namespace util::log {


/**
 * The IListen class defines an interface against various listen implementation objects. The class
 * hides the implementation classes.
 */
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

  void SetLogLevelText(uint64_t level, const std::string& menu_text);
  [[nodiscard]] const std::map<uint64_t,std::string>& LogLevelList() const;

  virtual void ListenText(const char* format_text, ... );
  virtual void ListenTextEx(uint64_t ns1970, const std::string& pre_text, const char* format_text, ... );

  virtual void ListenTransmit(uint64_t ns1970, const std::string& pre_text, const std::vector<uint8_t>& buffer, void* hint);
  virtual void ListenReceive(uint64_t ns1970, const std::string& pre_text, const std::vector<uint8_t>& buffer, void* hint);

  [[nodiscard]] virtual bool IsActive() const = 0;
  [[nodiscard]] virtual size_t LogLevel() = 0;

  virtual bool Start();
  virtual bool Stop();

 protected:
  std::string share_name_; ///< Share memory name
  std::string name_;  ///< Display name.
  std::string description_; ///< Description of the functionality.
  std::string pre_text_; ///< Small text between time and text string.
  std::string host_name_ = "127.0.0.1"; ///< Host name
  uint16_t port_ = 0; ///< IP-port to listen on.
  std::map<uint64_t,std::string> log_level_list_; // Log level index and

  IListen() = default;

  virtual void AddMessage(uint64_t nano_sec_1970, const std::string& pre_text, const std::string& text) = 0;
  static std::string ParseHex(const std::vector<uint8_t>& buffer);


 private:

};

std::unique_ptr<IListen> CreateListen(const std::string& type, const std::string& share_name);

}



