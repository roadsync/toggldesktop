// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_WEBSOCKET_CLIENT_H_
#define SRC_WEBSOCKET_CLIENT_H_

#include <string>
#include <vector>
#include <ctime>

#include <Poco/Activity.h>
#include <Poco/Net/HTTPClientSession.h>

#include "types.h"
#include "util/logger.h"

namespace Poco {
namespace Net {
class HTTPClientSession;
class HTTPRequest;
class HTTPResponse;
class WebSocket;
} // namespace Poco::Net
} // namespace Poco

namespace toggl {

typedef void (*WebSocketMessageCallback)(
    void *callback,
    std::string json);

class TOGGL_INTERNAL_EXPORT WebSocketClient {
 public:
    WebSocketClient() :
    activity_(this, &WebSocketClient::runActivity),
    session_(nullptr),
    req_(nullptr),
    res_(nullptr),
    ws_(nullptr),
    on_websocket_message_(nullptr),
    ctx_(nullptr),
    last_connection_at_(0),
    api_token_("") {}
    virtual ~WebSocketClient();

    virtual void Start(
        void *ctx,
        const std::string &api_token,
        WebSocketMessageCallback on_websocket_message);
    virtual void Shutdown();

 protected:
    void runActivity();

 private:
    error createSession();

    void authenticate();

    error poll();

    std::string parseWebSocketMessageType(const std::string &json);

    error receiveWebSocketMessage(std::string *message);

    void deleteSession();

    int nextWebsocketRestartInterval();

    Logger logger() const;

    Poco::Activity<WebSocketClient> activity_;
    Poco::Net::HTTPClientSession *session_;
    Poco::Net::HTTPRequest *req_;
    Poco::Net::HTTPResponse *res_;
    Poco::Net::WebSocket *ws_;
    WebSocketMessageCallback on_websocket_message_;
    void *ctx_;

    std::time_t last_connection_at_;

    std::string api_token_;

    Poco::Mutex mutex_;
};
}  // namespace toggl

#endif  // SRC_WEBSOCKET_CLIENT_H_
