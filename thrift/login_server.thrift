//1)会话票据: hex(aes(SessionTicket序列化), 密码 从login_server配置文件读取))
//2)会话id:   login_server:uuid()产生, 保存在center_server服务
//3)会话key:  login_server:随机产生，用于通讯密码
include "common.thrift"
namespace cpp community.login_server

enum MessageType
{   
	MT_CLIENT_REGISTER_REQ = 500,
	MT_CLIENT_REGISTER_RES,
    MT_CLIENT_PASSWORD_LOGIN_REQ,         //客户端密码登录请求
    MT_CLIENT_LOGIN_RES,                        //客户端登录回复
    MT_CLIENT_LOGOUT_REQ,                       //用户从客户端退出
    MT_CLIENT_LOGOUT_RES,
    MT_UPDATE_AVAILABLE_RES,						//客户端升级
}

struct ClientPasswordLoginReq
{
    1:i64    	user_id,                        //登录名
    2:i64       client_random,                  //客户端随机数
    3:string    client_mac,                     //客户端mac
    4:string    client_version,                 //客户端版本号
    5:string    check_sum,                       //检验和( hex(md5(ClientPasswordLoginReqChecksum序列化)) )
}

struct ClientPasswordLoginReqChecksum
{
    1:i64    	user_id,                        //登录名
    2:string    password,                       //hex(md5(用户密码明文))
    3:i64       client_random,                  //客户端随机数
    4:i64       server_random,                  //服务端随机数
}

struct ClientTicketLoginReqChecksum
{
    1:i64       user_id,                        //用户id
    2:string    session_ticket,                 //会话票据
    3:i64       client_random,                  //客户端随机数
    4:i64       server_random,                  //服务端随机数
}

struct ClientLoginRes
{
    1:i16       error_code,
    2:string    error_text,
    3:string    user_info,                      //用户信息( hex( aes(ClientLoginResUserInfo序列化), 密码为 md5(用户密码明文)) )
    4:string    session_ticket,                 //会话票据
	5:list<common.ServiceInfo> room_gate_info,       //roomgate地址列表
}

struct ClientLoginResUserInfo
{
    1:i64       user_id,                        //用户ID
    2:string    user_name,                      //用户名
    3:string    email,                          //邮箱
    4:string    nick_name,                      //昵称
    5:string    avatar_url,                     //用户头像url
    6:string   client_ip,                      //客户端ip
}

struct LogoutReq
{
    1:i64       user_id,                        //用户id
    2:string    session_ticket,                 //会话票据
}

struct LogoutRes
{
    1:i16       error_code,
    2:i64       user_id,
}

struct UserRegisterReq
{
    1:string    user_name,                      //用户名
    2:string    nick_name,                          //邮箱
    3:i16    	gender,                      //昵称
    4:string 	email,                   //性别
    5:string   client_ip,                      //客户端ip
    6:string   client_mac,                     //客户端mac
    7:string   client_version,                 //客户端版本号
    8:i32      app_id,                         //应用id(默认为0)
    9:string   session_id,                     //会话id
    10:string   session_key,                    //会话key
    11:string 	cellphone,
    12:string   password,
}

struct UserRegisterRes
{
    1:i16       error_code,
    2:string    error_text,
    3:string    user_id,
}

struct UpdateResourceReq
{
	1:string	client_version,		//客户端版本号
	2:string    resource_version,	//资源版本号
}

struct UpdateResourceRes
{
	1:i16		result,							//0:(不需要升级) -1:(需要升级)
	2:i16		update_type,					//升级类型:  1　选择升级; 2 强制升级; 3 静默升级；
	3:string	new_source_version,				//新版本号
	4:string	download_url;					//升级包的下载地址
	5:string	md5_value;						//md5值
	6:string	feature_url;					//新特url
	7:i32		file_size;						//升级包大小
}
