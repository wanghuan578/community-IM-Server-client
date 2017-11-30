include "common.thrift"
namespace cpp community.room_gate

enum MessageType
{  
    MT_CONNECT_REQ = 600,
    MT_CONNECT_RES,
    MT_GET_ROOMLIST_REQ,
    MT_GET_ROOMLIST_RES,
    MT_UPDATE_ROOMLIST_NOTIFY,
	UPDATE_USERINFO_REQ,
    UPDATE_USERINFO_RES,
}

struct ConnectReq
{
    1:string       sessoin_ticket,
    2:i64          client_random,
    3:i64          server_random,
    4:string       checksum,
}

struct ConnectChecksum
{
    1:i32           user_id,
    2:string        session_key,
    3:i64           client_random,
    4:i64           server_random,
}

struct RoomInfo
{
	1:i32		room_id,
	2:string 	room_name,
	3:string	room_resource_url,
	4:i32		room_category,
}

struct GetRoomlistRes
{
	1:i16       			error_code,
    2:string    			error_text,
    3:list<RoomInfo>     	room_list,
}

struct UpdateUserinfoReq
{
    1:string        nick_name,          //用户昵称
    2:string        avatar_url,         //用户头像url
    3:string        sign_name,          //个性签名
    4:i16          gender,             //性别(0男 1女 2保密)
    5:string        birthdate,          //出生日期
    6:string        chinese_zodiac,     //生肖
    7:string        zodiac,             //星座
    8:string        blood_type          //血型
    9:string       interest,           //用户爱好,兴趣
    10:string       character,          //用户性格
    11:i16          chest,              //胸围
    12:i16          waist,              //腰围
    13:i16          hip,                //臀围
    14:string       address,            //地址(故乡)
}
