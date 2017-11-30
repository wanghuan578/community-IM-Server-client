
include "common.thrift"
namespace cpp community.room_server

enum MessageType
{
	MT_CONNECT_REQ 			= 2000,
	MT_CONNECT_RES,
    MT_ENTER_ROOM_REQ,                   		//进入房间请求
    MT_ENTER_ROOM_RES,                          //进入房间回应
    MT_ENTER_ROOM_NOTIFY,                       //进入房间通告
    
    MT_LEAVE_ROOM_REQ,                          //离开房间请求
    MT_LEAVE_ROOM_RES,
    MT_LEAVE_ROOM_NOTIFY,
    
    MT_GET_ROOMINFO_REQ,                        //取得房间信息请求
    MT_GET_ROOMINFO_RES,
        
    MT_GET_SERVICE_IP_REQ,                      //取得服务ip请求
    MT_GET_SERVICE_IP_RES,
    MT_UPDATE_SERVICE_IP_NOTIFY,                //更新服务ip通告
    
    MT_GET_USER_LIST_REQ,                       //取得房间用户列表请求
    MT_GET_USER_LIST_RES,                       //取得房间用户列表回应
    
    MT_GET_MIC_LIST_REQ,                        //上麦用户列表请求
    MT_GET_MIC_LIST_RES,                        //上麦用户列表回应
    
    MT_UPDATE_NETWORK_STATUS_REQ,               //更新网络状况
    MT_UPDATE_NETWORK_STATUS_RES
    MT_UPDATE_NETWORK_STATUS_NOTIFY,
            
    MT_GET_SPECIAL_LIST_REQ,                    //取得房间特定用户列表(房主, 副房主, 管理员, 黑名单, 禁言, 被踢)请求
    MT_GET_SPECIAL_LIST_RES,
        
    //获取房间公告
    MT_ROOMBOARD_REQ,                           //获取公告请求
    MT_ROOMBOARD_RES,                           //获取公告响应
    
    //房间信息设置
    MT_UPDATE_ROOMINFO_REQ,                     //更新房间信息请求
    MT_UPDATE_ROOMINFO_RES,                     //更新房间信息响应
    MT_UPDATE_ROOMINFO_NOTIFY,                  //更新房间信息通告
    
    MT_MEDIA_DEVICE_SETTING_REQ,                //媒体设备设置(音视频设备参数)
    MT_MEDIA_DEVICE_SETTING_RES,
    MT_MEDIA_DEVICE_SETTING_NOTIFY,
        
    //上麦
    MT_MEDIA_UP_REQ,                            //上麦请求
    MT_MEDIA_UP_RES,
    MT_MEDIA_UP_NOTIFY,
    
    //下麦
    MT_MEDIA_DOWN_REQ,                          //下麦请求
    MT_MEDIA_DOWN_RES,
    MT_MEDIA_DOWN_NOTIFY,       
    
    //公聊,私聊
    MT_CHAT_REQ,                                //聊天消息请求
    MT_CHAT_RES,                                //聊天消息回应
    MT_CHAT_NOTIFY,                             //聊天消息通告
    
    //广播
    MT_BROADCAST_REQ,                           //广播请求
    MT_BROADCAST_RES,                           //广播回应
    MT_BROADCAST_NOTIFY,                        //广播通告
        
    //普通赠送礼物(单个礼物/一对一送礼)
    MT_GIFT_REQ,                                //赠送礼物请求
    MT_GIFT_RES,                                //赠送礼物回应
    MT_GIFT_NOTIFY,                             //赠送礼物通告
    
    //群刷礼物(单个礼物/一对多送礼)
    MT_MULTITOUSER_GIFT_REQ,                    //群刷礼物请求
    MT_MULTITOUSER_GIFT_RES,                    //群刷礼物回应
    MT_MULTITOUSER_GIFT_NOTIFY,                 //群刷礼物通告
    
    //踢人(用户被踢出房间后15分钟内不能再次进入)
    MT_KICKUSER_REQ,                            //踢人请求
    MT_KICKUSER_RES,                            //踢人响应
    MT_NOKICK_REQ,                              //解踢请求
    MT_NOKICK_RES,                              //解踢回应
    MT_NOKICK_NOTIFY,                           //解踢通告
    
    //黑名单
    MT_ADD_BLACK_LIST_REQ,                      //加入黑名单(拉黑)
    MT_ADD_BLACK_LIST_RES,
    MT_ADD_BLACK_LIST_NOTIFY,
    MT_REMOVE_BLACK_LIST_REQ,                   //解除黑名单
    MT_REMOVE_BLACK_LIST_RES,
    MT_REMOVE_BLACK_LIST_NOTIFY,
    
    //禁言10分钟
    MT_ADD_FORBID_CHAT_LIST_REQ,                //加入禁言列表
    MT_ADD_FORBID_CHAT_LIST_RES,
    MT_ADD_FORBID_CHAT_LIST_NOTIFY,
    MT_REMOVE_FORBID_CHAT_LIST_REQ,             //解除禁言
    MT_REMOVE_FORBID_CHAT_LIST_RES,
    MT_REMVOE_FORBID_CHAT_LIST_NOTIFY,
    
    //加房管(房主, 副房主, 管理员)
    MT_ADD_ROOM_MANAGER_LIST_REQ,                //加入房管
    MT_ADD_ROOM_MANAGER_LIST_RES,
    MT_ADD_ROOM_MANAGER_LIST_NOTIFY,
    MT_REMOVE_ROOM_MANAGER_LIST_REQ,             //解除房管
    MT_REMOVE_ROOM_MANAGER_LIST_RES,
    MT_REMOVE_ROOM_MANAGER_LIST_NOTIFY,
    
    //投票
    MT_VOTE_REQ,                                //投票请求
    MT_VOTE_RES,                                //投票响应
    MT_VOTE_NOTIFY,                             //投票通告
    
    //收藏房间
    MT_ADD_FAVORITE_REQ,                        //加入收藏夹
    MT_ADD_FAVORITE_RES,
    MT_REMOVE_FAVORITE_REQ,                     //从收藏夹移除
    MT_REMOVE_FAVORITE_RES,
    MT_QUERY_FAVORITE_REQ,                      //查询收藏
    MT_QUERY_FAVORITE_RES,
    
    //用户角色切换(若用户有多个角色, 用户可以选择角色)
    MT_SWITCH_USERROLE_REQ,
    MT_SWITCH_USERROLE_RES,
    MT_SWITCH_USERROLE_NOTIFY,
    
    //截图
    MT_SCREENSHOT_LOG_REQ,                      //上麦客户发送截图的记录请求
    MT_SCREENSHOT_LOG_RES,                      //上麦客户发送截图的记录回应
        
    MT_RESOURCE_CHANGE_NOTIFY,                  //资源变化通告 - 服务端检测到资源(礼物,角色权限等)改变, 通告客户端
    
    MT_BOX_REQ,                                 //宝箱请求
    MT_BOX_RES,                                 //宝箱回应
    MT_BOX_NOTIFY,                              //宝箱通告
    
    MT_PRIZE_NOTIFY,                            //奖品通告
    MT_SYSMSG_NOTIFY,                           //系统消息通告
        
    MT_WORLD_BROADCAST_REQ,                     //世界广播请求
    MT_WORLD_BROADCAST_RES,                     //世界广播回应
    MT_WORLD_BROADCAST_NOTIFY,                  //世界广播通告
    
    MT_WORLD_GIFT_REQ,                          //世界礼物请求
    MT_WORLD_GIFT_RES,                          //世界礼物回应
    MT_WORLD_GIFT_NOTIFY,                       //世界礼物通告
        
    MT_TIMEOUT_VIDEOGIFT_NOTIFY,                //魔法视频到期通告 (取消魔法视频 取消魔法视频的请求通过送礼物实现; 只发给单个用户)
    MT_SET_STAMPGIFT_NOTIFTY,                   //设置盖章的通告(设置盖章的请求通过送道具实现)
    MT_TIMEOUT_STAMPGIFT_NOTIFY,                //盖章到期通告 (取消盖章 取消盖章的请求通过送道具实现)
    MT_SET_BIRTHDAY_NOTIFY,                     //设置生日的通告
    MT_TIMEOUT_BIRTHDAY_NOTIFY,                 //生日到期通告 (取消生日)
    MT_TIMEOUT_ROLE_NOTIFY,                     //角色到期通告 (取消用户角色)
    
    MT_USERINFO_CHANGE_NOTIFY,                  //用户信息变更通告
    
    MT_SITE_PRIVATE_MESSAGE_REQ,                //站内私信请求
    MT_SITE_PRIVATE_MESSAGE_RES,                //站内私信回应
    MT_SITE_PRIVATE_MESSAGE_NOTIFY,             //站内私信通告
    
    //更新用户状态
    MT_UPDATE_USER_STATUS_REQ,
    MT_UPDATE_USER_STATUS_RES,
    MT_UPDATE_USER_STATUS_NOTIFY,
}

struct RoomConnectReq
{
	1:i32 		room_gate_id,
	2:string 	room_gate_name,
}

enum RoomInfoStatus
{
    PENDING = 1,		//审核状态
    PASS = 2,			//审核通过
}

enum GiftShowType
{
    SINGLE = 1,			//单一显示（仅发送方和接收方可见）
    ROOM   = 2,			//房间显示（全房间可见）			
    SITE   = 3,			//全站可见（全站可见）
}

enum NetworkLine                                //机房网络线路
{
    CHINA_TELECOM = 1,                          //电信
    CHINA_UNICOM,                               //联通
    CHINA_MODILE,                               //移动
    EDUCATION_NETWORK,                          //教育网
    MULTI_NETWORK = 10,                         //多线机房
}

enum RoomStatus                                 //房间状态
{
    INITIAL = 0,                                //初始
    NORMAL = 1,                                 //正常
    LOCK = 2,                                   //锁定
    CLOSE = 3,                                  //关闭
}

//房间属性标记(按位处理)
enum RoomFlag
{
    PROHIBIT_PRIVATE_CHAT = 1,                  //第1位: 禁止私聊
    PROHIBIT_PUBLIC_CHAT = 2,                   //第2位: 禁止公聊
    PROHIBIT_TEMPUSER_PUBLIC_CHAT = 4,          //第3位: 禁止游客公聊
    PROHIBIT_DJ_MEDIA_UP = 8,                   //第4位: 禁止主持人自由上下麦
    PROHIBIT_ROOM_BROADCAST = 16,               //第5位: 禁止房间广播
}

enum BroadcastType                              //广播类型
{
    NO_BROADCAST = 0,                           //不广播
    ONLY_SELF = 1,                              //只发给自己
    PRIVATE_MESSAGE = 2,                        //站内私信
    ROOM_BROADCAST = 3,                         //房间广播
    SITE_BROADCAST = 4,                         //站内广播
    WORLD_BROADCAST = 5,                        //世界广播
}

enum MediaStatus                                //媒体状态
{
    NONE = 0,                                  //没有麦
    READY = 1,                                  //未上麦
    PUBLIC_MIC = 2,                             //公麦
    PRIVATE_MIC = 3,                            //私麦
    FRIEND_MIC = 4,                             //好友麦(上麦用户,可拒绝或同意其他用户看麦)
}

enum MediaAction                                //媒体行为
{
    MEDIA_ACTION_UP = 1,                        //上麦
    MEDIA_ACTION_DOWN,                          //下麦
}

enum UserStatus                                 //用户状态
{
    OFFLINE = 0,                                //离线
    ONLINE = 1,                                 //在线
    AWAY = 2,                                   //离开(不在电脑旁) 当用户为这个状态时, 启动自动回应功能
    HIDE = 4,                                   //隐身
}

enum PermissionItemType                         //操作项类型
{
    ID0 = 0,                                    //对别人实施某种操作(对其他人行为控制:收麦，踢人等)
    ID1 = 1,                                    //系统对自已限制(如:公聊,隐身进入房间等)
    ID2 = 2,                                    //系统对自已限制,输入一个数字(如:每日发送窗口震动次数)
    ID3 = 3,                                    //对别人实施某种操作,但可能需对方确认(看好麦)
}

enum PermissionItemValue                        //权限值
{
    NOALLOW    = 0,                             //权限不允许
    ALLOW      = 1,                             //权限允许
    NEEDREPLY  = 2,                             //需对方回复
}

//权限操作项(必须与数据库中tb_permission_item表保持一致)
enum PermissionItem
{
    SinglePublicChat=1,                         //针对某个人公聊
    PublicChat=2,                               //公聊
    PrivateChat=3,                              //私聊(悄悄话)
    Dice=4,                                     //掷色子
    SetFont=5,                                  //设置字体
    AutoResponder=6,                            //自动回复
    SendEmotion=7,                              //发送表情
    SendMood=8,                                 //发送心情
    SendPokeUser=9,                             //发送"动一下"表情
    ShakeWindowTimes_Day=10,                    //每日发送窗口震动次数
    PublicMIC=11,                               //自由上下公麦
    PrivateMIC=12,                              //自由上下私麦
    EnterFullRoom=13,                           //进入人满房间
    HideEnterRoom=14,                           //隐身进入房间(不提示进入房间信息)
    SeeHide=15,                                 //可看到房间隐身者
                                                
    WaitTimeKMoney=16,                          //挂K币
    SeeUserIP=17,                               //查看用户IP
    ShowIdiograph=18,                           //显示个性签名(进入房间时显示用户个性签名)
    ShowUserIcon=19,                            //显示用户图标(进入房间时显示用户虚拟形象)
    FreeDecoColor=20,                           //免费彩色马甲
    TipOfEnterRoom=21,                          //进入房间醒目提示(进入房间时显示欢迎词)
    SendSystemBroadcast=22,                     //发送系统广播
    SendFreeSystemBroadcast=23,                 //免费发送系统广播
    SendRoomBroadcast=24,                       //发送房间内广播
    SendFreeRoomBroadcast=25,                   //免费发送房间内广播
                                                
    ToBeSiege=26,	                            //是否可攻城
    EnterLockedRoom=27,                         //进入上锁房间(不需要密码进入房间)
    ShowBirthday=28,				            //是否显示生日
                                                
    RoomManage=29,                              //房间管理
    RoomManage_PublicNotice=30,                 //房间管理-公告
    RoomManage_Setting=31,                      //房间管理-属性设置
    RoomManage_QuickReply=32,                   //房间管理-快捷回复
                                                
    KickUser=33,                                //踢人(将用户踢出房间)
    NoKickUser=34,                              //解踢
    ForbidChat=35,                              //禁言(禁止在房间发言)
    NoForbidChat=36,                            //解除禁言
    AddRoomOwner=37,                            //加为房主
    DelRoomOwner=38,                            //解除房主
    AddViceRoomOwner=39,                        //加为副房主
    DelViceRoomOwner=40,                        //解除副房主
    AddRoomManager=41,                          //加为房间管理员
    DelRoomManager=42,                          //解除房间管理员
    AddBlacklist=43,                            //拉黑(将用户加入房间黑名单)
    DelBlacklist=44,                            //解除黑名单
                                                
    Vote=45,                                    //投票
    ForcePublicMic=46,                          //递别人上公麦(抱上公麦)
    KickPublicMic=47,                           //收公麦
    KickPrivateMic=48,                          //收私麦
    KickFriendMic=49,                           //收好友麦
    SeePublicMic=50,                            //看公麦
    SeePrivateMic=51,                           //看私麦
    SeeFriendMic=52,                            //看好友麦
                                                
    ShieldUser=53,                              //屏蔽用户
    ShieldTempUserChat=54,                      //屏蔽游客聊天信息
                                                
    //房间特有权限(禁止私聊, 禁止公聊, 禁止游客登录, 禁止主持自由上麦, 禁上房间广播)
    Room_NoForbid_PrivateChat=55,               //不受房间"禁止私聊"限制
    Room_NoForbid_PublicChat=56,                //不受房间"禁止公聊"限制
    Room_NoForbid_MIC=57,                       //不受房间"禁止主持自由上麦"限制
    NoForbid_SendRoomBroadcast=58,              //不受房间"禁止房间广播"限制
                                                
    HideExitRoom=59,                            //隐身退出房间(不提示退出房间信息)
    RecordVideo=60,                             //录制视频
    ExchangeCash=61,                            //兑换现金
    ExchangeInfraredCamera=62,                  //兑换红外线摄像头
    ShowEnterRoomTip=63,                        //是否用户进入/退出房间提示信息
    ShowLevelSmallIcon_Chat=64,                 //聊天时是否显示用户等级小图标
    InRoomManageList=65,                        //是否在房间的用户列表的"管理"标签中
    Room_NoForbid_TempUser_PublicChat=66,       //不受房间"止禁上游客公聊"限制
    VideoStreamNumber=67,                       //视频流数量(几路视频)
    SeeFriendMic_Check=68,                      //看好友麦是否需对方回复 (废弃)
    FriendMIC=69,                               //自由上下好友麦
    SendChatInterval=70,                        //发送聊天消息的时间间隔(秒)
    SendPokeInterval=71,                        //发送"动一下"表情的时间间隔(秒)
    UserinfoSetting=72,                         //房间中用户的"个人设置"是否能打开
    RecvGift=73,                                //是否可以接受礼物(如:游客不能接收礼物)
    SendProp=74,                                //是否可以发送道具
    ShowChatTime=75,                            //聊天记录显示时间	巡管聊天记录后面显示时间
    KickUserSoftware=76,						//将用户踢出软件
    Auto_ProhibitUserIN=79,                     //自动封号,如果用户在房间则踢出软件同时通告所有房间，如果不在房间则禁止用户再进入房间(77 78 已经给网页权限使用了)
    Auto_OpenPriceMic=80,                       //自动打开用户私卖(目前国王以上级用户可用)
}

//消费渠道
enum ConsumerChannel
{
    NORMAL_CONSUMER = 1,                        //普通消费
    GIFTCAR,                                    //礼物车
    FLIGHT_GIFT,                                //飞翔的礼物
    MULTI_TOUSER_GIFT,                          //请客
    WORLD_GIFT,                                 //世界礼物
    SLOT_MACHINE,                               //老虎机(原来掷色子的改进)
    SEND_BOARDCAST,                             //发广播
}

//宝箱来源类型
enum BoxSourceType
{
    DAY_CONSUME_PRIZE = 1,                      //每日消费奖励
    ACHIEVEMENT_PRIZE = 2,                      //成就奖励
    OTHER = 3,                                  //其它
}

//奖品类型
enum PrizeType
{
    EXP_VALUE = 1,                              //经验值
    KMONEY,                                     //K币
    USER_ROLE,                                  //用户角色
    DECOLOR,                                    //彩色马甲
    BOX,                                        //宝箱
}

//黑名单类型
enum BlacklistType
{
    UNKNOW = 0,                                 //未知
    BREAK_LAW,                                  //违法信息    
}

//黑名单处置方式
enum BlacklistDisposeType
{
    KICKOUT_SITE = 1,                           //踢出社区(封id, 封mac)
}

//角色类型
enum RoleType
{
    REGISTER_USER = 10,                         //注册用户
    RECHARGE_USER = 11,                         //充值用户
    NORMAL_MEMBER = 20,                         //普通会员(vip1,vip2,vip3)
    SENIOR_MEMBER = 21,                         //高级会员(大亨，贵族，国王，至尊等)
    DISC_JOCKEY = 30,                           //主持人
    NORMAL_AGENT = 40,                          //代理
    GENERAL_AGENT = 50,                         //总代理
    SITE_MANAGER = 60,                          //管理角色(维护站点的角色:技术支持，客服，财务，站务管理)
    ROOM_ROLE = 70,                             //房间专用角色(房间管理员，房主，副房主)
    PRIZE_ROLE= 80,                             //特殊奖励角色(盟主, 周星, 女神等)
}

//礼物类型
enum GiftType
{
    //礼物类: id<1000
    TYPE_100 = 100,                             //普通礼物  40% k币 -> 礼物币
    TYPE_200 = 200,                             //货币礼物  80% k币 -> k币
    TYPE_210 = 210,                             //不定额货币礼物  80% k币 -> k币 (用户手工输入礼物的面额)
    TYPE_300 = 300,                             //彩票式礼物 如：幸运礼物
    TYPE_400 = 400,                             //经验礼物（k币 -> 100%经验值)
    
    //道具类 id>1000 and id<2000  直接消耗k币
    TYPE_1100 = 1100,                           //魔法视频道具       视频叠加    视频区
    TYPE_1101 = 1101,                           //取消魔法视频道具   视频叠加    视频区
    TYPE_1200 = 1200,                           //特殊头衔(盖章)     用户列表区
    TYPE_1201 = 1201,                           //取消特殊头衔(盖章) 用户列表区
    TYPE_1300 = 1300,                           //烟花               显示型
    TYPE_1400 = 1400,                           //时装秀搞恶         用户衣服
    TYPE_1401 = 1401,                           //取消时装秀搞恶     用户衣服
    TYPE_1500 = 1500,                           //世界礼物(站点间可以发送的礼物)  特殊玩法
    TYPE_1510 = 1510,                           //飞翔的礼物 特殊玩法
}

//聊天类型(按位处理)
enum ChatType
{
    PRIVATE_CHAT = 0,                           //私聊
    PUBLIC_CHAT = 1,                            //第1位: 0私聊      1公聊
    AUTO_REPLY  = 2,                            //第2位: 0手工输入  1自动回复
    USER_MOOD = 4,                              //第3位: 0普通      1用户心情
    POKE = 8,                                   //第4位: 0普通      1动一下表情
    SHAKE_WINDOW = 16,                          //第5位: 0普通      1震动窗口
}

//房间特定用户列表类型
enum SpecialListType
{
    BLACK_LIST = 1,                             //黑名单列表
    FORBID_CHAT_LIST = 2,                       //禁言列表
    KICK_LIST = 3,                              //被踢列表
    ROOM_OWNER_LIST = 25000,                    //房间主列表
    VICE_ROOM_OWNER_LIST = 24000,               //副房间主列表
    ROOM_MANAGER_LIST = 16000,                  //房间管理员列表
}

enum VideoSizeType                              //视频大小
{
    SIZE_160_120 = 0,                           //160*120
    SIZE_176_144 = 1,                           //176*144
    SIZE_320_240 = 2,                           //320*240
    SIZE_240_176 = 3,                           //240*176
    SIZE_352_288 = 4,                           //352*288
    SIZE_640_480 = 5,                           //640*480
    SIZE_704_576 = 6,                           //704*576
    SIZE_800_600 = 7,                           //800*600
    SIZE_1024_768 = 8,                          //1024*768
    SIZE_1280_720 = 9,                          //1280*720 720p
    SIZE_1920_1080 = 10,                        //1920*1080 1080i/1080p
}

//宝箱类型
enum BoxType
{
    GOLD_BOX = 1,                               //金宝箱
    SILVER_BOX = 2,                             //银宝箱
}

//媒体设备信息
struct MediaDeviceInfo
{
    1:i16       video_size_type,                //采集视频大小 关联 enum VideoSizeType
    2:i32       video_height,                   //视频高度
    3:i32       video_width,                    //视频宽度
    4:i16       video_frame_rate,               //帧率(一般有5,10,15,20,25,30,40,60 默认为10)
    5:i32       audio_sample_rate,              //音频采样率
}

//用户角色
struct UserRole
{
    1:i16       role_id,
    2:i64       role_id_endtime,                 //角色结束时间(0:表示没有结束时间)
}

//用户信息
struct UserInfo
{
    1:i32       user_id,                        //用户id
    2:string    user_name,                      //用户名
    3:string    email,                          //邮箱
    4:string    nick_name,                      //昵称
    5:i32       good_number,                    //靓号
    6:i16       use_role_id,                    //当前使用角色id
    7:i32       stamp_gift_id,                  //特殊头衔(盖章)的礼物id
    8:bool      is_robot,                       //用户是否为机器人
    9:bool      is_birthday,                    //用户生日是否到了
    10:i16      user_status,                    //关联 enum UserStatus
    11:i16      media_status,                   //关联 enum MediaStatus
    12:i16      network_line_id,                //网络提供商的id 关联 enum NetworkLine
    13:i16      network_status_level,           //网络状况等级(0:无信号 1非常差 2差 3一般 4好 5非常好)
    14:i64      network_status_time,            //网络状况更新时间
    15:i32      decolor,                        //彩色马甲
    16:map<i16, UserRole> user_role_list,       //用户角色集合
    17:string   sign_name,                      //个性签名
}

//用户的麦信息
struct MicInfo
{
    1:i32       user_id,                                //用户id
    2:i16       media_pos,                              //麦位置(公麦有三个位置:1,2,3)
    3:i64       media_time,                             //上麦时间    
    4:optional  MediaDeviceInfo media_device_info,      //媒体设备设置信息
}

//房间信息
struct RoomInfo
{
    1:string    room_name,                      //房间名称
    2:i32       room_flags,                     //房间属性标记  关联 enum RoomFlag
    3:string    room_theme,                     //主题
    4:string    room_broadinfo,                 //房间公告
    5:string    recharge_broadinfo,             //房间充值公告
    6:string    welcome_phrase,                 //房间欢迎词
    7:bool      is_change_password,             //是否改密码
    8:string    password,                       //房间密码(房间服务推送下来时为空, 用在改密码)
    9:bool      is_change_image,                //是否改房间图片
    10:string   room_image_url,                 //房间图片url
    11:i16      room_status,                    //房间状态      关联 enum RoomStatus
    12:string   room_desc,                      //房间描述
    13:i32      owner_user_id,                  //房主id
    14:i32      owner_good_number,              //房主靓号
    15:i32      max_count,                      //房间最大人数
    16:i16	room_broadinfo_status,		//房间广播审核状态
    17:i16	recharge_broadinfo_status,	//充值广播审核状态
    18:i16	welcome_phrase_status,		//房间欢迎词审核状态
    19:i16	room_broadinfo_minutes,		//房间广播显示间隔（0表示不重复显示，其他表示多少分钟显示一次）
    20:i16	recharge_broadinfo_minutes,     //充值公告审核状态（0表示不重复显示，其他表示多少分钟显示一次）
    21:i16	welcome_phrase_minutes,		//房间欢迎词审核状态（0表示不重复显示，其他表示多少分钟显示一次）
}

//房间服务ip信息
struct ServiceIPInfo
{
    1:string   video_service_ip1,
    2:i16      video_service_port1,
    3:string   video_service_ip2,
    4:i16      video_service_port2,
    5:string   video_service_ip3,
    6:i16      video_service_port3,
    
    7:string   audio_service_ip,
    8:i16      audio_service_port,
    9:string   audio_service_ip2,
    10:i16     audio_service_port2,
    11:string  audio_service_ip3,
    12:i16     audio_service_port3,
    
    13:string  image_service_ip,
    14:i16     image_service_port,
    15:string  screenshot_service_ip,
    16:i16     screenshot_service_port,
}

//进房间
struct EnterRoomReq
{
    1:string        room_password,              //房间密码
    2:bool          is_robot,                   //是否机器人
    3:i16           user_status,                //用户状态          关联 enum UserStatus
    4:i16           media_status,               //用户有无麦        关联 enum MediaStatus
    5:i16           network_line_id,            //网络提供商的id    关联 enum NetworkLine
    6:common.SessionTicket session_ticket,      //会话票据
}

struct EnterRoomRes
{
    1:i16       error_code,
    2:string    error_text,
}

//检验房间密码
struct CheckPasswordReq
{
    1:string    password,
}

//取得房间信息
struct GetRoomInfoRes
{
    1:i16       error_code,
    2:string    error_text,
    3:RoomInfo  room_info,
}

//取得服务ip
struct GetServiceIPRes
{
    1:i16       error_code,
    2:string    error_text,
    3:ServiceIPInfo service_ip_info,
}

//取得房间用户列表
struct GetUserListRes
{
    1:i16       error_code,
    2:string    error_text,
	3:i32       user_count,                     //房间用户总数
    4:list<UserInfo> user_list,
}

//取得上麦用户列表
struct GetMicUserListRes
{
    1:i16       error_code,
    2:string    error_text,
    3:list<MicInfo> mic_list,
}

struct UpdateNetworkStatusReq
{
    1:i16       network_status_level,           //网络状况等级(0:无信号 1非常差 2差 3一般 4好 5非常好)
    2:i64       update_time,                    //更新时间
}

//取得房间特定用户列表
struct SpecialUserinfo
{
    1:i32       user_id,
    2:string    nick_name,
    3:i32       good_number,
    4:i16       special_list_type,              //关联 enum SpecialListType
}
struct GetSpecialListRes
{
    1:i16       error_code,
    2:string    error_text,
	3:i32       special_user_count,
    4:list<SpecialUserinfo> special_user_list,
}

//大礼物列表
struct Biggift
{
    1:i32       gift_id,
    2:i16       gift_type_id,
    3:i32       gift_quantity,
    4:i16       is_biggift,
    5:i32       room_id,
    6:string    room_name,
    7:i32       from_user_id,
    8:i32       from_good_number,
    9:string    from_username,
    10:i32      from_amount,
    11:i32      to_user_id,
    12:i32      to_good_number,
    13:string   to_username,
    14:i32      to_amount,
    15:i32      trade_id,
    16:i64      create_timestamp,
}
struct GetBiggiftRes
{
    1:i16       error_code,
    2:string    error_text,
    3:i32       biggift_count,
    5:list<Biggift> biggift_list,
}

//取得攻城信息
struct GetSiegeRes
{
    1:i16       error_code,
    2:string    error_text,
    3:i32       trade_id,
    4:i32       room_id,
    5:string    room_name,
    6:i32       from_user_id,
    7:i32       from_good_number,
    8:string    from_username,
    9:i32       from_amount,
    10:i32      to_user_id,
    11:i32      to_good_number,
    12:string   to_username,
    13:i32      to_amount,
    14:i32      gift_id,
    15:i16      gift_type_id,
    16:i32      gift_quantity,
    17:i64      create_timestamp,
}

//字体
struct FontInfo
{
    1:string        name,                       //名称
    2:i16           size,                       //大小
    3:bool          bold,                       //是否加粗
    4:bool          italic,                     //是否倾斜
    5:bool          underline,                  //是否加下划线
    6:bool          strikeout,                  //是否加删除线
    7:i32           color,                      //颜色值
}

//聊天请求
struct ChatReq
{
    1:string        from_nick_name,             //发送用户呢称
    2:i32           from_good_number,           //发送用户靓号
    3:string        to_nick_name,               //接收用户呢称
    4:i32           to_good_number,             //接收用户靓号
    5:i32           chat_type,                  //聊天类型 关联enum ChatType
    6:string        chat_text,                  //聊天文本
    7:i64           chat_time,                  //聊天时间
    8:FontInfo      font_info,                  //字体
    9:i16           from_role_id,               //发送者角色id
    10:i32          from_role_icon_id,          //发送者角色图标id
    11:i16          to_role_id,                 //接收者角色id
    12:i32          to_role_icon_id,            //接收者角色图标id
}

//广播请求/站内私信
struct BroadcastReq
{
    1:string        from_room_name,             //源房间名
    2:string        from_nick_name,             //发送用户呢称
    3:i32           from_good_number,           //发送用户靓号
    4:i32           to_room_id,                 //目标房间id
    5:string        to_room_name,               //目标房间名称
    6:string        to_nick_name,               //接收用户呢称
    7:i32           to_good_number,             //接收用户靓号
    8:i16           broadcast_type,             //广播类型 关联enum BroadcastType
    9:string        broadcast_text,             //广播文本
    10:i64          send_time,                  //广播时间
    11:bool         is_color,                   //是否带颜色和表情
    12:FontInfo     font_info,                  //字体
    13:i16          from_role_id,               //发送者角色id
    14:i32          from_role_icon_id,          //发送者角色图标id
    15:i16          to_role_id,                 //接收者角色id
    16:i32          to_role_icon_id,            //接收者角色图标id
    17:i32	    gift_id,			//礼物ID（转播用）	
    18:i32	    gift_count,			//礼物数量
}

//普通赠送礼物(单个礼物/一对一送礼)
struct GiftReq
{
    1:string        from_nick_name,             //发送用户呢称
    2:i32           from_good_number,           //发送用户靓号
    3:string        to_nick_name,               //接收用户呢称
    4:i32           to_good_number,             //接收用户靓号
    5:i32           gift_id,                    //礼物id
    6:i32           gift_quantity,              //礼物数量
    7:i32           gift_sum_quantity,          //接收者收到这种礼物累计数量
    8:i64           send_time,                  //发送时间
    9:i16           is_gift_broadcast,          //礼物是否上系统广播
    10:string       from_room_name,             //房间名称
    11:i16          from_role_id,               //发送者角色id
    12:i32          from_role_icon_id,          //发送者角色图标id
    13:i16          to_role_id,                 //接收者角色id
    14:i32          to_role_icon_id,            //接收者角色图标id
    15:string 	    congratulations_text,       //祝语（烟花使用）
    16:i16	    show_type,			//显示类型，关联 enum GiftShowType
}

//礼物车请求(多个礼物/一对一送礼)
struct SingleGift
{
    1:i32           gift_id,
    2:i32           gift_quantity,
    3:i32           gift_sum_quantity,          //接收者收到这种礼物累计数量
}
struct MultiGiftReq
{
    1:string        from_nick_name,             //发送用户呢称
    2:i32           from_good_number,           //发送用户靓号
    3:string        to_nick_name,               //接收用户呢称
    4:i32           to_good_number,             //接收用户靓号
    5:i64           send_time,                  //发送时间
    6:i32           gift_count,                 //礼物种类数
    7:map<i32, SingleGift> gift_list,           //礼物列表
    8:i16           is_gift_broadcast,          //礼物是否上系统广播
    9:string        from_room_name,             //房间名称
    10:i16          from_role_id,               //发送者角色id
    11:i32          from_role_icon_id,          //发送者角色图标id
    12:i16          to_role_id,                 //接收者角色id
    13:i32          to_role_icon_id,            //接收者角色图标id
}

//群刷礼物(单个礼物/一对多送礼)
struct SingleUser
{
    1:i32           user_id,
    2:string        nick_name,
    3:i32           good_number,
    4:i32           gift_sum_quantity,          //接收者收到这种礼物累计数量
    5:i16           role_id,                    //接收者角色id
    6:i32           role_icon_id,               //接收者角色图标id
}
struct MultiToUserGiftReq
{
    1:string        from_nick_name,             //发送用户呢称
    2:i32           from_good_number,           //发送用户靓号
    3:i32           gift_id,                    //礼物id
    4:i32           gift_quantity,              //礼物数量
    5:i64           send_time,                  //发送时间
    6:i32           user_count,                 //接收用户数
    7:map<i32, SingleUser> to_user_list,        //接收用户列表
    8:i16           is_gift_broadcast,          //礼物是否上系统广播
    9:string        from_room_name,             //房间名称
    10:i16          from_role_id,               //发送者角色id
    11:i32          from_role_icon_id,          //发送者角色图标id
}

//上麦请求
struct MediaReq
{
    1:i16           media_status,                   //麦状态 关联MediaStatus
    2:i16           media_pos,                      //麦位置(公麦有三个位置:1,2,3)
    3:optional MediaDeviceInfo media_device_info,   //媒体设备信息
}

//媒体设置
struct MediaDeviceSettingReq
{
    1:MediaDeviceInfo media_device_info,
}

//订阅媒体
enum SubscribeMediaResult                           //订阅媒体结果
{    
    NOMEDIA = 0,                                   //没有麦
    REFUSE = 1,                                     //拒绝
    ACCEPT = 2,                                     //同意
}
struct SubscribeMediaRes
{
    1:i16       error_code,
    2:string    error_text,
    3:i16       result,                             //订阅结果 关联 enum SubscribeMediaResult
    4:optional  MediaDeviceInfo media_device_info,  //媒体设备信息
}


//截图
struct ScreenShotLogReq
{
    1:string    nick_name,
    2:i32       good_number,
    3:string    client_ip,
    4:string    client_mac,
    5:string    file_name,
}


//世界广播
struct WorldBroadcastReq
{
    1:i16           from_site_id,               //站点id
    2:string        from_site_name,             //站点名称
    3:i32           from_room_id,               //源房间id
    4:string        from_room_name,             //源房间名
    5:i32           from_user_id,               //发送用户id
    6:string        from_nick_name,             //发送用户呢称
    7:i32           from_good_number,           //发送用户靓号
    8:i32           from_amount,                //花费k币金额
    9:i32           from_exp_value,             //增长经验值    
    10:string       to_site_ids,                //能看到的站点id集合(以分号分隔)
    11:i32          to_room_id,                 //目标房间id
    12:string       to_room_name,               //目标房间名称
    13:i32          to_user_id,                 //接收用户id
    14:string       to_nick_name,               //接收用户呢称
    15:i32          to_good_number,             //接收用户靓号
    16:i16          broadcast_type,             //广播类型 关联enum BroadcastType
    17:string       broadcast_text,             //广播文本
    18:i64          send_time,                  //广播时间
    19:bool         is_color,                   //是否带颜色和表情
    20:FontInfo     font_info,                  //字体
    21:i16          from_role_id,               //发送者角色id
    22:i32          from_role_icon_id,          //发送者角色图标id    
    23:i16          to_role_id,                 //接收者角色id
    24:i32          to_role_icon_id,            //接收者角色图标id
}

//切换用户角色
struct SwitchUserRole
{
    1:i16       new_role_id,                    //新角色
    2:i16       old_role_id,                    //老角色
    3:i16       switch_type,                    //切换类型(1:用户主动切换 2:系统切换(删除老角色))
}

//用户角色到期通知
struct TimeoutRoleNotify
{
    1:i16       role_id,                        //角色
}

//礼物币兑换为k币(对应客户端的积分兑换功能)
struct ExchangeToKmoneyReq
{
    1:i32       from_giftmoney_amount,          //礼物币数量
    2:i32       to_kmoney_amount,               //k币数量
}
struct ExchangeToKmoneyRes
{
    1:i16       error_code,
    2:string    error_text,
    3:i64       kmoney_balance,                 //k币余额(-1:未变)
    4:i64       giftmoney_balance,              //礼物币余额(-1:未变)
}

//设置盖章通告
struct SetStampGiftNotify
{
    1:i32       stamp_gift_id,
}

//用户信息变更通告
struct UserinfoChangeNotify
{
    1:string    nick_name,                      //昵称(为空:未变)
    2:i32       good_number,                    //靓号(-1:未变 0:取消 other:新靓号)
    3:i32       decolor,                        //彩色马甲(-1:未变 other:新马甲)
}

//宝箱通告
struct BoxNotify
{
    1:string    nick_name,                      //用户呢称
    2:i32       good_number,                    //用户靓号
    3:i16       box_type,                       //宝箱类型 关联 enum BoxType
    4:i32       box_id,                         //宝箱id
    5:i64       box_time,                       //得到宝箱时间
    6:i32       today_send_amount,              //今天累计送出的k币(消费额)
    7:i32       today_recv_amount,              //今天累计收到的礼物币
    8:i32	role_id,			//用户角色ID
}

//更新用户状态
struct UpdateUserStatusReq
{
    1:i16       status_value,                   //状态值 关联 enum UserStatus
    2:string    status_text,                    //状态文本
}

//自动封号
struct AccountLockReq
{
    1:i32       to_good_number,                 //被封用号靓号
    2:string    to_ip,                          //被封ip
    3:string    to_mac,                         //被封mac
}


struct GiftPrizeNotify
{
	1:i32  good_number, 			//中奖者靓号
	2:string user_nick_name,		//中奖则昵称
	3:i32	multiple,			//中奖总倍率
	4:i32	prize_gift_id,			//中奖礼物ID
	5:i32	prize_amount,			//中奖总额
	6:list<i16> prize_multiples,		//中奖倍率明细
	7:i32  user_role_icon_id,		//用户角色ID
}