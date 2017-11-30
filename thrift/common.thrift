namespace cpp community.common

enum MessageType
{
    MT_HELLO_REQ 		= 100, 					
    MT_HELLO_RES,
    MT_HELLO_NOTIFY,
    MT_KEEPALIVE_REQ,
    MT_KEEPALIVE_RES,
}

enum ErrorCode
{
    OK = 0,                              //成功
    
    TICKET_INVALID  = 100,               //票据无效
    LOGNAME_EXIST,                       //用户名已存在
    LOGNAME_NOT_EXIST,                   //用户名不存在
    PASSWORD_ERROR,                      //密码错误
    USERID_INVALID,                      //用户ID无效
    PASS_EXPIRYDATE,                     //过了有效期
    ACCOUNT_LOCKED,                      //用户帐户被锁定
	INVALID_ACCOUNT,                     //用户帐户无效
    CLIENT_VERSION_INVALID,              //客户端版无效
	ACCOUNT_ACCTIVED,                    //帐户已被激活
	VERIFYCODE_VERIFIED,                 //验证码已验证过
	USER_IN_BLACKLIST,                   //用户在黑名单
    LOGIN_CONNECT_ERROR,                 //连接不上登录服务器
    ROOMGATE_CONNECT_ERROR,              //连接不上房间网关服务器
    NICKNAME_DUPLICATE,                  //用户昵称重复
    ROOM_UNKNOWN,                        //房间未知错误
    ROOM_STATUS_NO_STARTUP,              //房间未启动
    ROOM_STATUS_CLOSE,                   //房间关闭
    ROOM_STATUS_LOCKED,                  //房间锁定
    ROOM_USER_TICKET_INVALID,            //用户票据无效
    ROOM_USER_ID_INVALID,                //用户id无效
    ROOM_USER_NO_AUTHORITY,              //用户无此权限
    ROOM_USER_MONEY_LESS,                //用户余额不足
    ROOM_USER_KICK_OUT,                  //用户被踢出房间
    ROOM_USER_OTHER_LOCAL_LOGIN,         //用户在其他地方登录
    ROOM_USER_COUNT_MAX,                 //超房间最大人数
    ROOM_NEED_PASSWORD,                  //需要房间密码
    ROOM_PASSORD_ERROR,                  //房间密码错误
    ROOM_TO_USERID_INVALID,              //目标用户id无效
    REQUEST_ERROR,                     	 //请求错误
    REQUEST_TIMEOUT,                     //请求超时
    DB_CONNECT_ERROR,                    //与数据库链接失效
    DB_OPERATION_EXCEPTION,              //数据库操作异常
    SYSTEM_BUSY,                         //系统繁忙
    UNKOWN_ERROR,                        //未知错误
}

struct ServiceAddr
{
    1:string    ip,
    2:i16       port,
}

struct ServiceInfo
{
	1:i32       			service_id,
    2:string       			service_name,
	3:i16					service_weight,
	4:ServiceAddr			service_addr,
}

struct HelloReq
{
    1:ServiceInfo service_info,
}

struct HelloNotify
{
    1:i16       error_code,
    2:string    error_text,
    3:string    service_name,
    4:i32       service_id,
    5:i64       server_random,
    6:i64       server_time,
}

struct SessionTicket
{
    1:i32       user_id,                        //用户id
    2:string    user_name,                      //用户名
    3:string    email,                          //邮箱
    4:string    nick_name,                      //昵称
    5:string    client_ip,                      //客户端ip
    6:string    client_mac,                     //客户端mac
    7:string    client_version,                 //客户端版本号
    8:i32       app_id,                         //应用id(默认为0)
    9:string    session_id,                     //会话id
    10:string   session_key,                    //会话key
    11:i64      create_time,                    //创建时间(秒为单位)
    12:i64      end_time,                       //截止时间(秒为单位)
}

//空结构(用于没消息体的消息)
struct CommonNull
{
}

struct CommonRes
{
    1:i16       error_code,
    2:string    error_text,
}

struct KeepAliveReq
{
    1:i32       req_id,
    2:i64       current_time_req,
    3:i16       network_status_level,//网络状况等级(0:无信号 1非常差 2差 3一般 4好 5非常好)
}

struct KeepAliveRes
{
    1:i16       error_code,
    2:string    error_text,
    3:i32       req_id,
    4:i64       current_time_req,
    5:i64       current_time_res,
}