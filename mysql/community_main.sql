/*
Navicat MySQL Data Transfer

Source Server         : MySql_Communication
Source Server Version : 50528
Source Host           : 172.22.20.250:3306
Source Database       : community_main

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2014-10-16 14:06:31
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for tb_login_info
-- ----------------------------
DROP TABLE IF EXISTS `tb_login_info`;
CREATE TABLE `tb_login_info` (
  `f_login_name` varchar(32) DEFAULT NULL,
  `f_login_id` int(11) DEFAULT NULL,
  `f_log_time` bigint(20) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of tb_login_info
-- ----------------------------
INSERT INTO `tb_login_info` VALUES ('LoginServer_1', '1002', '1413003014');

-- ----------------------------
-- Table structure for tb_login_member
-- ----------------------------
DROP TABLE IF EXISTS `tb_login_member`;
CREATE TABLE `tb_login_member` (
  `f_user_id` int(11) NOT NULL,
  `f_session_id` varchar(36) DEFAULT NULL,
  `f_log_time` bigint(20) DEFAULT NULL,
  `f_login_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`f_user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of tb_login_member
-- ----------------------------

-- ----------------------------
-- Table structure for tb_roomgate_info
-- ----------------------------
DROP TABLE IF EXISTS `tb_roomgate_info`;
CREATE TABLE `tb_roomgate_info` (
  `f_roomgate_id` int(11) NOT NULL,
  `f_roomgate_name` varchar(32) DEFAULT NULL,
  `f_log_time` bigint(20) DEFAULT NULL,
  `f_online` int(11) DEFAULT NULL,
  `f_reference` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`f_roomgate_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of tb_roomgate_info
-- ----------------------------
INSERT INTO `tb_roomgate_info` VALUES ('11', 'RoomGate_1', '1413003015', null, null);

-- ----------------------------
-- Table structure for tb_roomgate_member
-- ----------------------------
DROP TABLE IF EXISTS `tb_roomgate_member`;
CREATE TABLE `tb_roomgate_member` (
  `f_user_id` int(11) DEFAULT NULL,
  `f_session_id` varchar(64) DEFAULT NULL,
  `f_log_time` bigint(20) DEFAULT NULL,
  `f_roomgate_id` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of tb_roomgate_member
-- ----------------------------

-- ----------------------------
-- Table structure for tb_room_info
-- ----------------------------
DROP TABLE IF EXISTS `tb_room_info`;
CREATE TABLE `tb_room_info` (
  `f_room_name` varchar(32) DEFAULT NULL,
  `f_room_id` int(11) NOT NULL,
  `f_theme` varchar(64) DEFAULT NULL,
  `f_room_broad_info` varchar(512) DEFAULT NULL,
  `f_welcome_phrase` varchar(512) DEFAULT NULL,
  `f_room_password` varchar(36) DEFAULT NULL,
  `f_room_broad_info_reivew_status` mediumint(9) DEFAULT NULL,
  `f_welcome_phrase_review_status` mediumint(9) DEFAULT NULL,
  `f_room_broad_info_interval` mediumint(9) DEFAULT NULL,
  `f_welcome_phrase_interval` mediumint(9) DEFAULT NULL,
  `f_log_time` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`f_room_id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of tb_room_info
-- ----------------------------
INSERT INTO `tb_room_info` VALUES ('午夜剧场', '10000', '交友', null, '欢迎来到午夜剧场', '', null, null, '5', '3', '1413003016');
INSERT INTO `tb_room_info` VALUES ('国庆乐', '10001', '娱乐', null, '欢迎来到国庆七天乐', null, null, null, '5', '3', '1412848886');

-- ----------------------------
-- Table structure for tb_room_member
-- ----------------------------
DROP TABLE IF EXISTS `tb_room_member`;
CREATE TABLE `tb_room_member` (
  `f_user_id` int(11) DEFAULT NULL,
  `f_room_id` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of tb_room_member
-- ----------------------------

-- ----------------------------
-- Table structure for tb_service_type
-- ----------------------------
DROP TABLE IF EXISTS `tb_service_type`;
CREATE TABLE `tb_service_type` (
  `f_service_type` tinyint(4) NOT NULL,
  `f_service_name` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`f_service_type`),
  KEY `f_service_type` (`f_service_type`) USING HASH
) ENGINE=InnoDB DEFAULT CHARSET=gbk;

-- ----------------------------
-- Records of tb_service_type
-- ----------------------------
INSERT INTO `tb_service_type` VALUES ('0', 'center_server');
INSERT INTO `tb_service_type` VALUES ('1', 'login_server');
INSERT INTO `tb_service_type` VALUES ('2', 'room_gate');
INSERT INTO `tb_service_type` VALUES ('3', 'room_server');
INSERT INTO `tb_service_type` VALUES ('4', 'dbproxy_server');

-- ----------------------------
-- Table structure for tb_user_info
-- ----------------------------
DROP TABLE IF EXISTS `tb_user_info`;
CREATE TABLE `tb_user_info` (
  `f_user_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '用户ID',
  `f_user_name` varchar(50) CHARACTER SET gbk NOT NULL,
  `f_password` varchar(64) NOT NULL COMMENT '密码值',
  `f_nick_name` varchar(50) DEFAULT NULL COMMENT '昵称',
  `f_secure_question` varchar(255) DEFAULT NULL,
  `f_secure_answer` varchar(255) DEFAULT NULL,
  `f_signature` varchar(255) DEFAULT NULL,
  `f_identification_card_id` varchar(20) DEFAULT NULL,
  `f_gender` smallint(6) DEFAULT NULL,
  `f_avatar_url` varchar(255) DEFAULT NULL,
  `f_blood_type` varchar(20) DEFAULT NULL,
  `f_chinese_zodiac` varchar(50) DEFAULT NULL,
  `f_zodiac` varchar(50) DEFAULT NULL,
  `f_interest` varchar(255) DEFAULT NULL,
  `f_character` varchar(255) DEFAULT NULL,
  `f_chest` smallint(6) DEFAULT NULL,
  `f_waist` smallint(6) DEFAULT NULL,
  `f_hip` varchar(6) DEFAULT NULL,
  `f_country_id` smallint(6) DEFAULT NULL,
  `f_province_id` smallint(6) DEFAULT NULL,
  `f_city_id` smallint(6) DEFAULT NULL,
  `f_address` varchar(255) DEFAULT NULL,
  `f_qq` varchar(50) DEFAULT NULL,
  `f_msn` varchar(255) DEFAULT NULL,
  `f_homepage_url` varchar(255) DEFAULT NULL,
  `f_register_time` datetime DEFAULT NULL,
  `f_mac` varchar(36) DEFAULT NULL,
  `f_register_ip` varchar(50) DEFAULT NULL,
  `f_status` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`f_user_id`),
  UNIQUE KEY `f_user_name` (`f_user_name`)
) ENGINE=InnoDB AUTO_INCREMENT=789796 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of tb_user_info
-- ----------------------------
INSERT INTO `tb_user_info` VALUES ('1', '高丽棒子', '000000', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO `tb_user_info` VALUES ('10000000', '王欢', '1', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO `tb_user_info` VALUES ('10000001', '张立磊', '1', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO `tb_user_info` VALUES ('10000002', '岳景超', '1', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO `tb_user_info` VALUES ('10000003', '张海江', '1', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);
INSERT INTO `tb_user_info` VALUES ('10000004', 'liyunchun', '33333', null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null, null);

-- ----------------------------
-- Procedure structure for SP_EnterRoom
-- ----------------------------
DROP PROCEDURE IF EXISTS `SP_EnterRoom`;
DELIMITER ;;
CREATE DEFINER=`wanghuan`@`` PROCEDURE `SP_EnterRoom`(IN `user_id` int,IN `room_id` int,IN `room_pwd` varchar(32),IN `user_status` int,OUT `error_code` int)
BEGIN
	#Routine body goes here...
	declare tmp_user_id int;
	declare room_password varchar(32);

	set error_code = 0;

	select f_room_password into room_password from tb_room_info where f_room_id = room_id;

	if room_password = "" then

			select f_user_id into tmp_user_id from tb_room_member where f_user_id = user_id;

			if tmp_user_id != "" then
				
					delete from tb_room_member where f_user_id = user_id;

					commit;

			end if;

			insert into tb_room_member(f_user_id, f_room_id) values(user_id, room_id);

			commit;

	elseif room_password = room_pwd then

			select f_user_id into tmp_user_id from tb_room_member where f_user_id = user_id;

			if tmp_user_id != "" then
					
					delete from tb_room_member where f_user_id = user_id;

					commit;

			end if;

			insert into tb_room_member(f_user_id, f_room_id) values(user_id, room_id);

			commit;

	else

			set error_code = 127;

	end if;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_GetUserIdentity
-- ----------------------------
DROP PROCEDURE IF EXISTS `SP_GetUserIdentity`;
DELIMITER ;;
CREATE DEFINER=`wanghuan`@`` PROCEDURE `SP_GetUserIdentity`(IN `user_id` int,OUT `user_key` varchar(40),OUT `session_id` varchar(64))
BEGIN
	#Routine body goes here...
	select f_password into user_key from tb_user_info where f_user_id = user_id;
	select f_session_id into session_id from tb_login_member where f_user_id = user_id;

	if session_id != "" then
		
		delete from tb_login_member where f_user_id = user_id;
		
		commit;

	end if;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_LeaveRoom
-- ----------------------------
DROP PROCEDURE IF EXISTS `SP_LeaveRoom`;
DELIMITER ;;
CREATE DEFINER=`wanghuan`@`` PROCEDURE `SP_LeaveRoom`(IN `user_id` int,OUT `rtn` int)
BEGIN
	#Routine body goes here...
	declare tmp_user_id int;

	set rtn = 0;

	select f_user_id into tmp_user_id from tb_room_member where f_user_id = user_id;

	if tmp_user_id != "" then

			delete from tb_room_member where f_user_id = user_id;

			commit;

	else

			set rtn = 120;

	end if;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_RegisterRoomGateUser
-- ----------------------------
DROP PROCEDURE IF EXISTS `SP_RegisterRoomGateUser`;
DELIMITER ;;
CREATE DEFINER=`wanghuan`@`` PROCEDURE `SP_RegisterRoomGateUser`(IN `user_id` int,IN `session_id` varchar(40),IN `log_time` bigint,OUT `rtn` int)
BEGIN
	#Routine body goes here...
	declare user_id_tmp int;
	declare roomgate_user_id int;

	set rtn = 0;

	select f_user_id into user_id_tmp from tb_user_info where f_user_id = user_id;

	if user_id_tmp != "" then
		
			select f_user_id into roomgate_user_id from tb_roomgate_member where f_user_id = user_id;

			if roomgate_user_id != "" then

					update tb_roomgate_member set f_session_id = session_id, f_log_time = log_time where f_user_id = user_id;

			else

					insert into tb_roomgate_member(f_user_id, f_session_id, f_log_time) values(user_id, session_id, log_time);

			end if;

			commit;

	else

			set rtn = 102;

	end if;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_RemoveRoomGateUser
-- ----------------------------
DROP PROCEDURE IF EXISTS `SP_RemoveRoomGateUser`;
DELIMITER ;;
CREATE DEFINER=`wanghuan`@`` PROCEDURE `SP_RemoveRoomGateUser`(IN `user_id` int,OUT `rtn` int)
BEGIN
	#Routine body goes here...
	declare roomgate_user_id int;

	set rtn = 0;

	select f_user_id into roomgate_user_id from tb_roomgate_member where f_user_id = user_id;

	if roomgate_user_id != "" then
		
			delete from tb_roomgate_member where f_user_id = user_id;

			commit;

	end if;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_ServiceRegistor
-- ----------------------------
DROP PROCEDURE IF EXISTS `SP_ServiceRegistor`;
DELIMITER ;;
CREATE DEFINER=`wanghuan`@`` PROCEDURE `SP_ServiceRegistor`(IN `service_type` int,IN `service_id` int,IN `service_name` varchar(32),IN `log_time` bigint,OUT `error_code` int)
BEGIN
	#Routine body goes here...
	declare login_id int;
	declare room_id int;
	declare room_gate_id int;

	

	if service_type = 0 then

			set error_code = 0;

	elseif service_type = 1 then

			select f_login_id into login_id from tb_login_info where f_login_id = service_id;

			if login_id != "" then
				
					#delete from tb_login_info where f_login_id = service_id;

					#commit;

					set error_code = 0;

			else

					insert into tb_login_info(f_login_name, f_login_id, f_log_time) values(service_name, service_id, log_time);

					commit;

			end if;

			

	elseif service_type = 2 then

			select f_roomgate_id into room_gate_id from tb_roomgate_info where f_roomgate_id = service_id;

			if room_gate_id != "" then
				
					#delete from tb_roomgate_info where room_gate_id = service_id;

					#commit;

					set error_code = 0;

			else

					insert into tb_roomgate_info(f_roomgate_name, f_roomgate_id, f_log_time) values(service_name, service_id, log_time);

					commit;

			end if;

	elseif service_type = 3 then

			select f_room_id into room_id from tb_room_info where f_room_id = service_id;

			if room_id != "" then
				
					#delete from tb_room_info where f_room_id = service_id;

					#commit;

					set error_code = 0;

			else

					insert into tb_room_info(f_room_name, f_room_id, f_log_time) values(service_name, service_id, log_time);

					commit;

			end if;

			
	end if;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for SP_UserStatusCheck
-- ----------------------------
DROP PROCEDURE IF EXISTS `SP_UserStatusCheck`;
DELIMITER ;;
CREATE DEFINER=`wanghuan`@`` PROCEDURE `SP_UserStatusCheck`(IN `user_id` int,OUT `rtn` int)
BEGIN
	#Routine body goes here...
	declare tmp_user_id int;

	set rtn = 0;

	select f_user_id into tmp_user_id from tb_roomgate_member where f_user_id = user_id;

	if tmp_user_id != "" then

			set rtn = 101;

	end if;
END
;;
DELIMITER ;
