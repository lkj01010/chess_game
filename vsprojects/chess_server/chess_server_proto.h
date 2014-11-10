#ifndef CHESS_SERVER_PROTO_H
#define CHESS_SERVER_PROTO_H

#include <cstdint>

#define CMD_LOGIN 1 // 登录请求/响应
#define CMD_SITDOWN 2 // 坐下请求
#define CMD_RANDSITDOWN 3 // 随机坐下请求
#define CMD_READY 4 // 准备请求
#define CMD_STANDUP 5 // 站立请求
#define CMD_GAME 6 // 游戏逻辑
#define CMD_ADDUSER 7 // 用户进入房间
#define CMD_RMUSER 8 // 用户离开房间
#define CMD_UPDATEUSERSTATE 9 // 更新用户状态

#define CMD_STOPSERVER 0xff // 关闭服务器

typedef uint16_t table_id_t;
typedef uint16_t seat_id_t;

#define INVALID_SEAT_ID 0xffff


enum {
  USER_STATE_STANDUP,
  USER_STATE_SITDOWN,
  USER_STATE_READY,
  USER_STATE_PLAY
};

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(1)
#else
#error set byte align
#endif /* _MSC_VER */

/** 登录请求 */
struct CmdLogin {
  char uid[16]; // 账号
  char passwd[16]; // 密码
};


struct UserInfo {
  uint32_t user_id; // 用户唯一ID
  char nick_name[16]; // 昵称
  uint8_t state; // 用户状态（站立，坐下，准备，游戏）
  table_id_t table_id; // 桌子号
  seat_id_t seat_id; // 椅子号
}; 


/** 登录响应 */
struct CmdLoginResponse {
  uint8_t succeed; // 是否登录成功
  uint32_t user_id; // 用户唯一ID
  uint32_t sz;
  UserInfo users[1];
};


/** 坐下请求 */
struct CmdSitDown {
  table_id_t table_id_; // 桌子号
  seat_id_t seat_id_; // 座位号
};


/** 用户进入服务器 */
struct CmdAddUser {
  UserInfo user;
};

/** 用户离开服务器 */
struct CmdRemoveUser {
  uint32_t user_id; // 用户唯一ID
};

/** 用户进入桌子 */
struct CmdTableUserEnter {
  uint32_t user_id; // 用户唯一ID
  table_id_t table_id_; // 桌子号
  seat_id_t seat_id_; // 座位号
};

/** 更新用户状态 */
struct CmdUpdateUserState {
  uint32_t user_id; // 用户唯一ID
  uint8_t state; // 更新后的状态
};

#ifdef _MSC_VER
#pragma pack(pop)
#else
#error set byte align
#endif /* _MSC_VER */

#endif /* CHESS_SERVER_PROTO_H */
