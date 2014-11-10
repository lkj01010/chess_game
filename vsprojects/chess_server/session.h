#ifndef SESSION_H
#define SESSION_H

#include <cstdint>
#include <memory>
#include <functional>
#include <atomic>
#include <queue>
#include <boost/asio.hpp>
#include <boost/utility.hpp>

class Session;
typedef std::shared_ptr<Session> SessionPtr;
typedef std::weak_ptr<Session> SessionWeakPtr;

class Session : public std::enable_shared_from_this<Session>, public boost::noncopyable{
  typedef boost::asio::ip::tcp::socket socket_t;
  typedef boost::asio::deadline_timer timer_t;
  typedef std::function<void(const void *data, uint16_t sz)> on_message_cb_t;
  typedef std::function<void()> on_timeout_cb_t;
  typedef std::function<void()> on_close_cb_t;

  struct WireBuffer {
    WireBuffer(void *data, uint16_t sz) {
      this->data = data;
      this->sz = sz;
    }
    ~WireBuffer() {
      if (data) {
        free(data);
      }
    }
    void *data;
    uint16_t sz;
  };
  typedef std::shared_ptr<WireBuffer> WireBufferPtr;

public:
  static SessionPtr Create(socket_t &socket) {
    SessionPtr session = std::make_shared<Session>(socket);
    session->Init();
    return session;
  }

  Session(socket_t &socket);
  ~Session();

  /** �յ�һ���������ݰ��ص� */
  void SetMessageCallback(const on_message_cb_t &cb) { on_message_cb_ = cb; }

  /** ��ʱδ�յ����Է����ݻص� */
  void SetTimeoutCallback(const on_timeout_cb_t &cb) { on_timeout_cb_ = cb; }

  /** �Է��ر�����/����ص���֮�����еĻص������ٱ����� */
  void SetCloseCallback(const on_close_cb_t &cb) { on_close_cb_ = cb; }

  /** �Ͽ����ӣ����еĻص������ٱ����� */
  void Disconnect();

  /** �������ݣ����Զ����2B�ĳ���ǰ׺ */
  void SendMessage(const void* data, uint16_t sz);

private:
  void Init();
  void ReadHead();
  void ReadBody();
  void SetTimer();
  void OnTimeout();
  void DoWrite();
  void OnError();

private:
  socket_t socket_;
  timer_t timer_;
  on_message_cb_t on_message_cb_;
  on_timeout_cb_t on_timeout_cb_;
  on_close_cb_t on_close_cb_;
  uint16_t message_size_;
  std::unique_ptr<char[]> message_;
  std::atomic_bool closed_;
  std::queue<WireBufferPtr> send_queue_;
};

#endif /* SESSION_H */
