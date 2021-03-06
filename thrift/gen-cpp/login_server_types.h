/**
 * Autogenerated by Thrift Compiler (0.10.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef login_server_TYPES_H
#define login_server_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>
#include "common_types.h"


namespace community { namespace login_server {

struct MessageType {
  enum type {
    MT_CLIENT_REGISTER_REQ = 500,
    MT_CLIENT_REGISTER_RES = 501,
    MT_CLIENT_PASSWORD_LOGIN_REQ = 502,
    MT_CLIENT_LOGIN_RES = 503,
    MT_CLIENT_LOGOUT_REQ = 504,
    MT_CLIENT_LOGOUT_RES = 505,
    MT_UPDATE_AVAILABLE_RES = 506
  };
};

extern const std::map<int, const char*> _MessageType_VALUES_TO_NAMES;

class ClientPasswordLoginReq;

class ClientPasswordLoginReqChecksum;

class ClientTicketLoginReqChecksum;

class ClientLoginRes;

class ClientLoginResUserInfo;

class LogoutReq;

class LogoutRes;

class UserRegisterReq;

class UserRegisterRes;

class UpdateResourceReq;

class UpdateResourceRes;

typedef struct _ClientPasswordLoginReq__isset {
  _ClientPasswordLoginReq__isset() : user_id(false), client_random(false), client_mac(false), client_version(false), check_sum(false) {}
  bool user_id :1;
  bool client_random :1;
  bool client_mac :1;
  bool client_version :1;
  bool check_sum :1;
} _ClientPasswordLoginReq__isset;

class ClientPasswordLoginReq : public virtual ::apache::thrift::TBase {
 public:

  ClientPasswordLoginReq(const ClientPasswordLoginReq&);
  ClientPasswordLoginReq& operator=(const ClientPasswordLoginReq&);
  ClientPasswordLoginReq() : user_id(0), client_random(0), client_mac(), client_version(), check_sum() {
  }

  virtual ~ClientPasswordLoginReq() throw();
  int64_t user_id;
  int64_t client_random;
  std::string client_mac;
  std::string client_version;
  std::string check_sum;

  _ClientPasswordLoginReq__isset __isset;

  void __set_user_id(const int64_t val);

  void __set_client_random(const int64_t val);

  void __set_client_mac(const std::string& val);

  void __set_client_version(const std::string& val);

  void __set_check_sum(const std::string& val);

  bool operator == (const ClientPasswordLoginReq & rhs) const
  {
    if (!(user_id == rhs.user_id))
      return false;
    if (!(client_random == rhs.client_random))
      return false;
    if (!(client_mac == rhs.client_mac))
      return false;
    if (!(client_version == rhs.client_version))
      return false;
    if (!(check_sum == rhs.check_sum))
      return false;
    return true;
  }
  bool operator != (const ClientPasswordLoginReq &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientPasswordLoginReq & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ClientPasswordLoginReq &a, ClientPasswordLoginReq &b);

inline std::ostream& operator<<(std::ostream& out, const ClientPasswordLoginReq& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ClientPasswordLoginReqChecksum__isset {
  _ClientPasswordLoginReqChecksum__isset() : user_id(false), password(false), client_random(false), server_random(false) {}
  bool user_id :1;
  bool password :1;
  bool client_random :1;
  bool server_random :1;
} _ClientPasswordLoginReqChecksum__isset;

class ClientPasswordLoginReqChecksum : public virtual ::apache::thrift::TBase {
 public:

  ClientPasswordLoginReqChecksum(const ClientPasswordLoginReqChecksum&);
  ClientPasswordLoginReqChecksum& operator=(const ClientPasswordLoginReqChecksum&);
  ClientPasswordLoginReqChecksum() : user_id(0), password(), client_random(0), server_random(0) {
  }

  virtual ~ClientPasswordLoginReqChecksum() throw();
  int64_t user_id;
  std::string password;
  int64_t client_random;
  int64_t server_random;

  _ClientPasswordLoginReqChecksum__isset __isset;

  void __set_user_id(const int64_t val);

  void __set_password(const std::string& val);

  void __set_client_random(const int64_t val);

  void __set_server_random(const int64_t val);

  bool operator == (const ClientPasswordLoginReqChecksum & rhs) const
  {
    if (!(user_id == rhs.user_id))
      return false;
    if (!(password == rhs.password))
      return false;
    if (!(client_random == rhs.client_random))
      return false;
    if (!(server_random == rhs.server_random))
      return false;
    return true;
  }
  bool operator != (const ClientPasswordLoginReqChecksum &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientPasswordLoginReqChecksum & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ClientPasswordLoginReqChecksum &a, ClientPasswordLoginReqChecksum &b);

inline std::ostream& operator<<(std::ostream& out, const ClientPasswordLoginReqChecksum& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ClientTicketLoginReqChecksum__isset {
  _ClientTicketLoginReqChecksum__isset() : user_id(false), session_ticket(false), client_random(false), server_random(false) {}
  bool user_id :1;
  bool session_ticket :1;
  bool client_random :1;
  bool server_random :1;
} _ClientTicketLoginReqChecksum__isset;

class ClientTicketLoginReqChecksum : public virtual ::apache::thrift::TBase {
 public:

  ClientTicketLoginReqChecksum(const ClientTicketLoginReqChecksum&);
  ClientTicketLoginReqChecksum& operator=(const ClientTicketLoginReqChecksum&);
  ClientTicketLoginReqChecksum() : user_id(0), session_ticket(), client_random(0), server_random(0) {
  }

  virtual ~ClientTicketLoginReqChecksum() throw();
  int64_t user_id;
  std::string session_ticket;
  int64_t client_random;
  int64_t server_random;

  _ClientTicketLoginReqChecksum__isset __isset;

  void __set_user_id(const int64_t val);

  void __set_session_ticket(const std::string& val);

  void __set_client_random(const int64_t val);

  void __set_server_random(const int64_t val);

  bool operator == (const ClientTicketLoginReqChecksum & rhs) const
  {
    if (!(user_id == rhs.user_id))
      return false;
    if (!(session_ticket == rhs.session_ticket))
      return false;
    if (!(client_random == rhs.client_random))
      return false;
    if (!(server_random == rhs.server_random))
      return false;
    return true;
  }
  bool operator != (const ClientTicketLoginReqChecksum &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientTicketLoginReqChecksum & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ClientTicketLoginReqChecksum &a, ClientTicketLoginReqChecksum &b);

inline std::ostream& operator<<(std::ostream& out, const ClientTicketLoginReqChecksum& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ClientLoginRes__isset {
  _ClientLoginRes__isset() : error_code(false), error_text(false), user_info(false), session_ticket(false), room_gate_info(false) {}
  bool error_code :1;
  bool error_text :1;
  bool user_info :1;
  bool session_ticket :1;
  bool room_gate_info :1;
} _ClientLoginRes__isset;

class ClientLoginRes : public virtual ::apache::thrift::TBase {
 public:

  ClientLoginRes(const ClientLoginRes&);
  ClientLoginRes& operator=(const ClientLoginRes&);
  ClientLoginRes() : error_code(0), error_text(), user_info(), session_ticket() {
  }

  virtual ~ClientLoginRes() throw();
  int16_t error_code;
  std::string error_text;
  std::string user_info;
  std::string session_ticket;
  std::vector< ::community::common::ServiceInfo>  room_gate_info;

  _ClientLoginRes__isset __isset;

  void __set_error_code(const int16_t val);

  void __set_error_text(const std::string& val);

  void __set_user_info(const std::string& val);

  void __set_session_ticket(const std::string& val);

  void __set_room_gate_info(const std::vector< ::community::common::ServiceInfo> & val);

  bool operator == (const ClientLoginRes & rhs) const
  {
    if (!(error_code == rhs.error_code))
      return false;
    if (!(error_text == rhs.error_text))
      return false;
    if (!(user_info == rhs.user_info))
      return false;
    if (!(session_ticket == rhs.session_ticket))
      return false;
    if (!(room_gate_info == rhs.room_gate_info))
      return false;
    return true;
  }
  bool operator != (const ClientLoginRes &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientLoginRes & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ClientLoginRes &a, ClientLoginRes &b);

inline std::ostream& operator<<(std::ostream& out, const ClientLoginRes& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ClientLoginResUserInfo__isset {
  _ClientLoginResUserInfo__isset() : user_id(false), user_name(false), email(false), nick_name(false), avatar_url(false), client_ip(false) {}
  bool user_id :1;
  bool user_name :1;
  bool email :1;
  bool nick_name :1;
  bool avatar_url :1;
  bool client_ip :1;
} _ClientLoginResUserInfo__isset;

class ClientLoginResUserInfo : public virtual ::apache::thrift::TBase {
 public:

  ClientLoginResUserInfo(const ClientLoginResUserInfo&);
  ClientLoginResUserInfo& operator=(const ClientLoginResUserInfo&);
  ClientLoginResUserInfo() : user_id(0), user_name(), email(), nick_name(), avatar_url(), client_ip() {
  }

  virtual ~ClientLoginResUserInfo() throw();
  int64_t user_id;
  std::string user_name;
  std::string email;
  std::string nick_name;
  std::string avatar_url;
  std::string client_ip;

  _ClientLoginResUserInfo__isset __isset;

  void __set_user_id(const int64_t val);

  void __set_user_name(const std::string& val);

  void __set_email(const std::string& val);

  void __set_nick_name(const std::string& val);

  void __set_avatar_url(const std::string& val);

  void __set_client_ip(const std::string& val);

  bool operator == (const ClientLoginResUserInfo & rhs) const
  {
    if (!(user_id == rhs.user_id))
      return false;
    if (!(user_name == rhs.user_name))
      return false;
    if (!(email == rhs.email))
      return false;
    if (!(nick_name == rhs.nick_name))
      return false;
    if (!(avatar_url == rhs.avatar_url))
      return false;
    if (!(client_ip == rhs.client_ip))
      return false;
    return true;
  }
  bool operator != (const ClientLoginResUserInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ClientLoginResUserInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ClientLoginResUserInfo &a, ClientLoginResUserInfo &b);

inline std::ostream& operator<<(std::ostream& out, const ClientLoginResUserInfo& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _LogoutReq__isset {
  _LogoutReq__isset() : user_id(false), session_ticket(false) {}
  bool user_id :1;
  bool session_ticket :1;
} _LogoutReq__isset;

class LogoutReq : public virtual ::apache::thrift::TBase {
 public:

  LogoutReq(const LogoutReq&);
  LogoutReq& operator=(const LogoutReq&);
  LogoutReq() : user_id(0), session_ticket() {
  }

  virtual ~LogoutReq() throw();
  int64_t user_id;
  std::string session_ticket;

  _LogoutReq__isset __isset;

  void __set_user_id(const int64_t val);

  void __set_session_ticket(const std::string& val);

  bool operator == (const LogoutReq & rhs) const
  {
    if (!(user_id == rhs.user_id))
      return false;
    if (!(session_ticket == rhs.session_ticket))
      return false;
    return true;
  }
  bool operator != (const LogoutReq &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LogoutReq & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(LogoutReq &a, LogoutReq &b);

inline std::ostream& operator<<(std::ostream& out, const LogoutReq& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _LogoutRes__isset {
  _LogoutRes__isset() : error_code(false), user_id(false) {}
  bool error_code :1;
  bool user_id :1;
} _LogoutRes__isset;

class LogoutRes : public virtual ::apache::thrift::TBase {
 public:

  LogoutRes(const LogoutRes&);
  LogoutRes& operator=(const LogoutRes&);
  LogoutRes() : error_code(0), user_id(0) {
  }

  virtual ~LogoutRes() throw();
  int16_t error_code;
  int64_t user_id;

  _LogoutRes__isset __isset;

  void __set_error_code(const int16_t val);

  void __set_user_id(const int64_t val);

  bool operator == (const LogoutRes & rhs) const
  {
    if (!(error_code == rhs.error_code))
      return false;
    if (!(user_id == rhs.user_id))
      return false;
    return true;
  }
  bool operator != (const LogoutRes &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const LogoutRes & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(LogoutRes &a, LogoutRes &b);

inline std::ostream& operator<<(std::ostream& out, const LogoutRes& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _UserRegisterReq__isset {
  _UserRegisterReq__isset() : user_name(false), nick_name(false), gender(false), email(false), client_ip(false), client_mac(false), client_version(false), app_id(false), session_id(false), session_key(false), cellphone(false), password(false) {}
  bool user_name :1;
  bool nick_name :1;
  bool gender :1;
  bool email :1;
  bool client_ip :1;
  bool client_mac :1;
  bool client_version :1;
  bool app_id :1;
  bool session_id :1;
  bool session_key :1;
  bool cellphone :1;
  bool password :1;
} _UserRegisterReq__isset;

class UserRegisterReq : public virtual ::apache::thrift::TBase {
 public:

  UserRegisterReq(const UserRegisterReq&);
  UserRegisterReq& operator=(const UserRegisterReq&);
  UserRegisterReq() : user_name(), nick_name(), gender(0), email(), client_ip(), client_mac(), client_version(), app_id(0), session_id(), session_key(), cellphone(), password() {
  }

  virtual ~UserRegisterReq() throw();
  std::string user_name;
  std::string nick_name;
  int16_t gender;
  std::string email;
  std::string client_ip;
  std::string client_mac;
  std::string client_version;
  int32_t app_id;
  std::string session_id;
  std::string session_key;
  std::string cellphone;
  std::string password;

  _UserRegisterReq__isset __isset;

  void __set_user_name(const std::string& val);

  void __set_nick_name(const std::string& val);

  void __set_gender(const int16_t val);

  void __set_email(const std::string& val);

  void __set_client_ip(const std::string& val);

  void __set_client_mac(const std::string& val);

  void __set_client_version(const std::string& val);

  void __set_app_id(const int32_t val);

  void __set_session_id(const std::string& val);

  void __set_session_key(const std::string& val);

  void __set_cellphone(const std::string& val);

  void __set_password(const std::string& val);

  bool operator == (const UserRegisterReq & rhs) const
  {
    if (!(user_name == rhs.user_name))
      return false;
    if (!(nick_name == rhs.nick_name))
      return false;
    if (!(gender == rhs.gender))
      return false;
    if (!(email == rhs.email))
      return false;
    if (!(client_ip == rhs.client_ip))
      return false;
    if (!(client_mac == rhs.client_mac))
      return false;
    if (!(client_version == rhs.client_version))
      return false;
    if (!(app_id == rhs.app_id))
      return false;
    if (!(session_id == rhs.session_id))
      return false;
    if (!(session_key == rhs.session_key))
      return false;
    if (!(cellphone == rhs.cellphone))
      return false;
    if (!(password == rhs.password))
      return false;
    return true;
  }
  bool operator != (const UserRegisterReq &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UserRegisterReq & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(UserRegisterReq &a, UserRegisterReq &b);

inline std::ostream& operator<<(std::ostream& out, const UserRegisterReq& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _UserRegisterRes__isset {
  _UserRegisterRes__isset() : error_code(false), error_text(false), user_id(false) {}
  bool error_code :1;
  bool error_text :1;
  bool user_id :1;
} _UserRegisterRes__isset;

class UserRegisterRes : public virtual ::apache::thrift::TBase {
 public:

  UserRegisterRes(const UserRegisterRes&);
  UserRegisterRes& operator=(const UserRegisterRes&);
  UserRegisterRes() : error_code(0), error_text(), user_id() {
  }

  virtual ~UserRegisterRes() throw();
  int16_t error_code;
  std::string error_text;
  std::string user_id;

  _UserRegisterRes__isset __isset;

  void __set_error_code(const int16_t val);

  void __set_error_text(const std::string& val);

  void __set_user_id(const std::string& val);

  bool operator == (const UserRegisterRes & rhs) const
  {
    if (!(error_code == rhs.error_code))
      return false;
    if (!(error_text == rhs.error_text))
      return false;
    if (!(user_id == rhs.user_id))
      return false;
    return true;
  }
  bool operator != (const UserRegisterRes &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UserRegisterRes & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(UserRegisterRes &a, UserRegisterRes &b);

inline std::ostream& operator<<(std::ostream& out, const UserRegisterRes& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _UpdateResourceReq__isset {
  _UpdateResourceReq__isset() : client_version(false), resource_version(false) {}
  bool client_version :1;
  bool resource_version :1;
} _UpdateResourceReq__isset;

class UpdateResourceReq : public virtual ::apache::thrift::TBase {
 public:

  UpdateResourceReq(const UpdateResourceReq&);
  UpdateResourceReq& operator=(const UpdateResourceReq&);
  UpdateResourceReq() : client_version(), resource_version() {
  }

  virtual ~UpdateResourceReq() throw();
  std::string client_version;
  std::string resource_version;

  _UpdateResourceReq__isset __isset;

  void __set_client_version(const std::string& val);

  void __set_resource_version(const std::string& val);

  bool operator == (const UpdateResourceReq & rhs) const
  {
    if (!(client_version == rhs.client_version))
      return false;
    if (!(resource_version == rhs.resource_version))
      return false;
    return true;
  }
  bool operator != (const UpdateResourceReq &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UpdateResourceReq & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(UpdateResourceReq &a, UpdateResourceReq &b);

inline std::ostream& operator<<(std::ostream& out, const UpdateResourceReq& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _UpdateResourceRes__isset {
  _UpdateResourceRes__isset() : result(false), update_type(false), new_source_version(false), download_url(false), md5_value(false), feature_url(false), file_size(false) {}
  bool result :1;
  bool update_type :1;
  bool new_source_version :1;
  bool download_url :1;
  bool md5_value :1;
  bool feature_url :1;
  bool file_size :1;
} _UpdateResourceRes__isset;

class UpdateResourceRes : public virtual ::apache::thrift::TBase {
 public:

  UpdateResourceRes(const UpdateResourceRes&);
  UpdateResourceRes& operator=(const UpdateResourceRes&);
  UpdateResourceRes() : result(0), update_type(0), new_source_version(), download_url(), md5_value(), feature_url(), file_size(0) {
  }

  virtual ~UpdateResourceRes() throw();
  int16_t result;
  int16_t update_type;
  std::string new_source_version;
  std::string download_url;
  std::string md5_value;
  std::string feature_url;
  int32_t file_size;

  _UpdateResourceRes__isset __isset;

  void __set_result(const int16_t val);

  void __set_update_type(const int16_t val);

  void __set_new_source_version(const std::string& val);

  void __set_download_url(const std::string& val);

  void __set_md5_value(const std::string& val);

  void __set_feature_url(const std::string& val);

  void __set_file_size(const int32_t val);

  bool operator == (const UpdateResourceRes & rhs) const
  {
    if (!(result == rhs.result))
      return false;
    if (!(update_type == rhs.update_type))
      return false;
    if (!(new_source_version == rhs.new_source_version))
      return false;
    if (!(download_url == rhs.download_url))
      return false;
    if (!(md5_value == rhs.md5_value))
      return false;
    if (!(feature_url == rhs.feature_url))
      return false;
    if (!(file_size == rhs.file_size))
      return false;
    return true;
  }
  bool operator != (const UpdateResourceRes &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UpdateResourceRes & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(UpdateResourceRes &a, UpdateResourceRes &b);

inline std::ostream& operator<<(std::ostream& out, const UpdateResourceRes& obj)
{
  obj.printTo(out);
  return out;
}

}} // namespace

#endif
