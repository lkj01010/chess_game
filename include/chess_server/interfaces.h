#ifndef GAME_SERVER_INTERFACES_H
#define GAME_SERVER_INTERFACES_H

#include <cstdint>
#include <functional>

typedef uint16_t seat_id_t;
typedef uint32_t timer_id_t;

#define INVALID_TIMER_ID      0

class IUser {
public:
  virtual ~IUser() = default;

  /** 获取玩家昵称 */
  virtual const char* GetNickName() = 0;

  /** 获取玩家金币数 */
  virtual int32_t GetCoin() = 0;

  /** 将玩家踢下线 */
  virtual void Kick() = 0;

  /** 获取玩家的座位号 */
  virtual seat_id_t seat_id() = 0;
};


class IServerLogi {
public:
  virtual ~IServerLogi() = default;

  /**
   * 向桌子上的所有玩家发送数据
   * 这个函数有两个重载，上面那个函数直接将数据传进去，
   * 下面那个函数的数据将通过IGameLogi::GetMessage回调来获取
   */
  virtual void BroadcastMessage(const void *mess, uint16_t sz) = 0;
  //virtual void BroadcastMessage() = 0;

  /** 向桌子上除seat_id之外的所有玩家发送数据 */
  virtual void BroadcastMessageExcept(seat_id_t seat_id, const void *mess, uint16_t sz) = 0;
  //virtual void BroadcastMessageExcept(seat_id_t seat_id) = 0;

  /** 仅向seat_id发送数据 */
  virtual void SendMessageTo(seat_id_t seat_id, const void *mess, uint16_t sz) = 0;
  virtual void SendMessageTo(seat_id_t seat_id) = 0;

  /** 定时器，有一次触发的，有重复触发的 */
  virtual timer_id_t SetTimeout(const std::function<void()> &callback, int delay) = 0;
  virtual void ClearTimeout(timer_id_t id) = 0;
  virtual timer_id_t SetInterval(const std::function<void()> &callback, int delay) = 0;
  virtual void ClearInterval(timer_id_t id) = 0;
};


class IGameLogi {
public:
  virtual ~IGameLogi() = default;

  /**
   * 游戏开始
   * 返回true则紧接着会执行IServerLogi::BroadcastMessage
   */
  virtual bool OnGameStart() = 0;
  
  /** 处理玩家消息 */
  virtual void HandleMessage(IUser *user, const void *mess, uint16_t sz) = 0;

  /** 获取玩家数据 */
  virtual uint16_t GetMessageSize(seat_id_t seat) = 0;
  virtual void GetMessageData(seat_id_t seat, void *buffer) = 0;

  virtual void OnUserEnter(IUser* user) {}
  virtual void OnUserLeave(IUser* user) {}
};

#endif /* GAME_SERVER_INTERFACES_H */
