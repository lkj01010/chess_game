#ifndef CHESS_SERVER_PROTO_H
#define CHESS_SERVER_PROTO_H

#include <cstdint>

#define CMD_LOGIN 1 // ��¼����/��Ӧ
#define CMD_SITDOWN 2 // ��������
#define CMD_RANDSITDOWN 3 // �����������
#define CMD_READY 4 // ׼������
#define CMD_STANDUP 5 // վ������
#define CMD_GAME 6 // ��Ϸ�߼�
#define CMD_ADDUSER 7 // �û����뷿��
#define CMD_RMUSER 8 // �û��뿪����
#define CMD_UPDATEUSERSTATE 9 // �����û�״̬

#define CMD_STOPSERVER 0xff // �رշ�����

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

/** ��¼���� */
struct CmdLogin {
  char uid[16]; // �˺�
  char passwd[16]; // ����
};


struct UserInfo {
  uint32_t user_id; // �û�ΨһID
  char nick_name[16]; // �ǳ�
  uint8_t state; // �û�״̬��վ�������£�׼������Ϸ��
  table_id_t table_id; // ���Ӻ�
  seat_id_t seat_id; // ���Ӻ�
}; 


/** ��¼��Ӧ */
struct CmdLoginResponse {
  uint8_t succeed; // �Ƿ��¼�ɹ�
  uint32_t user_id; // �û�ΨһID
  uint32_t sz;
  UserInfo users[1];
};


/** �������� */
struct CmdSitDown {
  table_id_t table_id_; // ���Ӻ�
  seat_id_t seat_id_; // ��λ��
};


/** �û���������� */
struct CmdAddUser {
  UserInfo user;
};

/** �û��뿪������ */
struct CmdRemoveUser {
  uint32_t user_id; // �û�ΨһID
};

/** �û��������� */
struct CmdTableUserEnter {
  uint32_t user_id; // �û�ΨһID
  table_id_t table_id_; // ���Ӻ�
  seat_id_t seat_id_; // ��λ��
};

/** �����û�״̬ */
struct CmdUpdateUserState {
  uint32_t user_id; // �û�ΨһID
  uint8_t state; // ���º��״̬
};

#ifdef _MSC_VER
#pragma pack(pop)
#else
#error set byte align
#endif /* _MSC_VER */

#endif /* CHESS_SERVER_PROTO_H */
