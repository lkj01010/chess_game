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

  /** ��ȡ����ǳ� */
  virtual const char* GetNickName() = 0;

  /** ��ȡ��ҽ���� */
  virtual int32_t GetCoin() = 0;

  /** ����������� */
  virtual void Kick() = 0;

  /** ��ȡ��ҵ���λ�� */
  virtual seat_id_t seat_id() = 0;
};


class IServerLogi {
public:
  virtual ~IServerLogi() = default;

  /**
   * �������ϵ�������ҷ�������
   * ����������������أ������Ǹ�����ֱ�ӽ����ݴ���ȥ��
   * �����Ǹ����������ݽ�ͨ��IGameLogi::GetMessage�ص�����ȡ
   */
  virtual void BroadcastMessage(const void *mess, uint16_t sz) = 0;
  //virtual void BroadcastMessage() = 0;

  /** �������ϳ�seat_id֮���������ҷ������� */
  virtual void BroadcastMessageExcept(seat_id_t seat_id, const void *mess, uint16_t sz) = 0;
  //virtual void BroadcastMessageExcept(seat_id_t seat_id) = 0;

  /** ����seat_id�������� */
  virtual void SendMessageTo(seat_id_t seat_id, const void *mess, uint16_t sz) = 0;
  virtual void SendMessageTo(seat_id_t seat_id) = 0;

  /** ��ʱ������һ�δ����ģ����ظ������� */
  virtual timer_id_t SetTimeout(const std::function<void()> &callback, int delay) = 0;
  virtual void ClearTimeout(timer_id_t id) = 0;
  virtual timer_id_t SetInterval(const std::function<void()> &callback, int delay) = 0;
  virtual void ClearInterval(timer_id_t id) = 0;
};


class IGameLogi {
public:
  virtual ~IGameLogi() = default;

  /**
   * ��Ϸ��ʼ
   * ����true������Ż�ִ��IServerLogi::BroadcastMessage
   */
  virtual bool OnGameStart() = 0;
  
  /** ���������Ϣ */
  virtual void HandleMessage(IUser *user, const void *mess, uint16_t sz) = 0;

  /** ��ȡ������� */
  virtual uint16_t GetMessageSize(seat_id_t seat) = 0;
  virtual void GetMessageData(seat_id_t seat, void *buffer) = 0;

  virtual void OnUserEnter(IUser* user) {}
  virtual void OnUserLeave(IUser* user) {}
};

#endif /* GAME_SERVER_INTERFACES_H */
