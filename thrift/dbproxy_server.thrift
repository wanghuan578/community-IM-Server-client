
namespace cpp community.dbproxy_server

enum MessageType
{
    MT_LOGIN_CHECK_REQ 		= 5000,
    MT_LOGIN_CHECK_RES,
	
	MT_USER_LOGIN_REQ,
    MT_USER_LOGIN_RES,
	
    MT_LOGIN_GET_USERINFO_REQ,
    MT_LOGIN_GET_USERINFO_RES,
	
	MT_GET_USER_IDENTITY_REQ,
	MT_GET_USER_IDENTITY_RES,
	
	MT_LOGIN_REGIST0R_REQ,
	MT_LOGIN_REGIST0R_RES,
	
	MT_ROOMGATE_REGIST0R_REQ,
	MT_ROOMGATE_REGIST0R_RES,
	
	MT_ROOM_REGIST0R_REQ,
	MT_ROOM_REGIST0R_RES,
	
    MT_ROOM_ENTER_ROOM_REQ,
    MT_ROOM_ENTER_ROOM_RES,
    
    MT_LEAVE_ROOM_PENDING_REQ,
    MT_LEAVE_ROOM_PENDING_RES,

    MT_LEAVE_ROOM_COMPLETE_REQ,
    MT_LEAVE_ROOM_COMPLETE_RES,

    MT_ROOM_UPDATE_ROOMINFO_REQ,
    MT_ROOM_UPDATE_ROOMINFO_RES,
    
    MT_ROOMGATE_UPDATE_USERINFO_REQ,
    MT_ROOMGATE_UPDATE_USERINFO_RES,
    
    MT_ROOM_UPDATE_ROOM_ONLINE_COUNT_REQ,
    MT_ROOM_UPDATE_ROOM_ONLINE_COUNT_RES,
}

struct UserLoginReq
{
	1:i32		user_id,
	2:string 	user_name,
	3:string	session_id,
	4:i64		log_time,
}

struct LoginCheckReq
{
    1:string    user_name,
    2:string    check_sum,
    3:string    client_mac,
    4:string    client_ip,
}

struct LoginCheckRes
{
    1:i16       error_code,
    2:string    error_text,
}

struct LoginGetUserInfoReq
{
    1:string    user_name,
}

struct UserInfo
{
	1:i32       user_id,                        //用户ID
    2:string    user_name,                      //用户名
    3:string	password,			//用户密码
    4:string    email,                          //邮箱
    5:string    nick_name,                      //昵称
    6:string    avatar_url,                     //用户头像url
    7:string    birthdate,                      //出生日期
    8:i16      base_role_id,                   //基本角色
    9:byte     gender,                         //性别(0男 1女 2保密)
    10:string   chinese_zodiac,                 //生肖
    11:string   zodiac,                         //星座
    12:string   blood_type                      //血型
    13:string   interest,                       //用户爱好,兴趣
    14:string   character,                      //用户性格
    15:string	sign_name,			//个性签名
    16:i16      chest,                          //胸围
    17:i16      waist,                          //腰围
    18:i16      hip,                            //臀围
    19:string   address,                        //故乡
}

struct LoginGetUserInfoRes
{
    1:i16       error_code,
    2:string    error_text,
	3:UserInfo	user_info,
}

//增加房间成员(关联数据表tb_room_member)
struct RoomAddMemberReq
{
    1:i32	user_id,          //用户id
}

//删除房间成员(关联数据表tb_room_member)
struct RoomRemoveMemberReq
{
    1:i32	user_id,
}

//更新用户信息
struct RoomGateUpdateUserinfoReq
{
    1:string    nick_name,                      //用户昵称
    2:string    avatar_url,                     //用户头像url
    3:i32       decolor,                        //彩色马甲
    4:string    sign_name,                      //个性签名
    5:byte      gender,                         //性别(0男 1女 2保密)
    6:string    birthdate,                      //出生日期
    7:string    chinese_zodiac,                 //生肖
    8:string    zodiac,                         //星座
    9:string    blood_type                      //血型
    10:string   interest,                       //用户爱好、兴趣
    11:string   character,                      //用户性格
    12:i16      chest,                          //胸围
    13:i16      waist,                          //腰围
    14:i16      hip,                            //臀围
    15:string   address,                        //地址(故乡)
}

struct GetUserIdentityReq
{
	1:i32	user_id,
}

struct GetUserIdentityRes
{
	1:i16       error_code,
    2:string    error_text,
	3:string	user_key,
	4:string	session_id,
}

struct ServerRegisterReq
{
	1:i16 		serice_type,
	2:i32 		service_id,
	3:string 	service_name,
	4:i64		log_time,
}

struct RoomGateUserRegisterReq
{
	1:i32 		user_id,
	2:i32 		room_gate_id,
	3:string 	session_id,
	4:i64		log_time,
}

//更新房大辉，博信的服务roomgate直接连dbp吗间信息(关联数据表tb_room_info)
struct RoomUpdateRoominfoReq
{
    1:string    room_name,                      //房间名称
    2:i32       room_flags,                     //房间标识
    3:string    room_broadinfo,                 //房间公告
    4:string    recharge_broadinfo,             //房间充值公告
    5:string    welcome_phrase,                 //房间欢迎词
    6:bool      is_change_password,             //是否改密码
    7:string    password,                       //房间密码(房间服务推送下来时为空, 用在改密码)
    8:bool      is_change_image,                //是否改房间图片
    9:string    room_image_url,                 //房间图片url
    10:i16	room_broadinfo_status,		//房间广播审核状态
    11:i16	recharge_broadinfo_status,	//充值公告审核状态
    12:i16	welcome_phrase_status,		//房间欢迎词审核状态
    13:i16	room_broadinfo_minutes,		//房间公告显示间隔
    14:i16	recharge_broadinfo_minutes,	//充值公告显示间隔
    15:i16	welcome_phrase_minutes,		//房间欢迎词显示间隔
}

struct RoomEnterRoomReq
{
	1:string 	room_password,
    2:i16       base_role_id,                   //基本角色
    3:i16       use_role_id,                    //当前使用角色id
    4:string    user_ip,                        //用户ip
    5:string    user_mac,                       //用户mac
    6:bool      is_temp_user,                   //是否为游客
}

struct RoomEnterRoomRes
{ 
    1:i16       error_code,
    2:string    error_text,
    3:bool      is_in_blacklist,                //是否在黑名单中
    4:bool      is_favorite,                    //是否收藏房间
}