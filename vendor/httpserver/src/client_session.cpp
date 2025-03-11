#include "http_socket.h"
#include "http2_frame.h"
#include "client_session.h"
#include "http2_parse.h"
#include "terminal_color.h"
#include "http2_define.h"
#include "http2_huffman.h"
#include "https_brotli.h"
#include "unicode.h"
#include "directory_fun.h"
#include "http_header.h"
#include "clientdatacache.h"
#include "http2_ring_queue.h"

namespace http
{
client_session::client_session()
{
    auto &cc    = get_client_data_cache();
    _cache_data = cc.get_data_ptr();
    // cache_back_obj.setptr(_cache_data);
}

client_session::~client_session()
{
    isclose = true;
    if (_cache_data != nullptr)
    {
        auto &cc = get_client_data_cache();
        cc.back_data_ptr(_cache_data);
    }

    if (http2_ring_queue)
    {
        auto &cc = get_http2_ring_queue_obj();
        cc.back_cache_ptr(std::move(http2_ring_queue));
        http2_ring_queue = nullptr;
    }
}
asio::awaitable<bool> client_session::read_some(unsigned int &readnum, std::string &log_item)
{
    memset(_cache_data, 0x00, 4096);
    try
    {
        if (isssl)
        {
            readnum = co_await sslsocket->async_read_some(asio::buffer(_cache_data, 4096), asio::use_awaitable);
        }
        else
        {
            readnum = co_await socket->async_read_some(asio::buffer(_cache_data, 4096), asio::use_awaitable);
        }
    }
    catch (const std::exception &e)
    {
        log_item.append(e.what());
        DEBUG_LOG("read_some exception %s", e.what());
        isclose = true;
        iserror = true;
        co_return true;
    }
    co_return false;
}

std::shared_ptr<client_session> client_session::get_ptr() { return shared_from_this(); }

bool client_session::send_data(const std::string &msg)
{
    if (isclose)
    {
        return false;
    }
    try
    {
        if (isssl)
        {
            if (sslsocket->lowest_layer().is_open())
            {
                asio::write(*sslsocket, asio::buffer(msg));
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (socket->is_open())
            {
                asio::write(*socket, asio::buffer(msg));
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    catch (std::exception &)
    {
        isclose = true;
        iserror = true;
        return false;
    }
}
bool client_session::isopensocket()
{

    try
    {
        if (isssl)
        {
            if (sslsocket->lowest_layer().is_open())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (socket->is_open())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    catch (std::exception &)
    {
        return false;
    }
}

bool client_session::send_switch101()
{
    try
    {
        std::string tempswitch = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: h2c\r\n\r\n";
        if (isssl)
        {
            asio::write(*sslsocket, asio::buffer(tempswitch));
        }
        else
        {
            asio::write(*socket, asio::buffer(tempswitch));
        }
        return true;
    }
    catch (std::exception &)
    {
        isclose = true;
        iserror = true;
        return false;
    }
}

asio::awaitable<void> client_session::co_send_setting()
{
    unsigned char _recvack[] = {0x00, 0x00, 0x0C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x64, 0x00, 0x04, 0x00, 0xFF, 0xFF, 0xFF};
    http2_ring_queue->push(_recvack, 21);
    co_return;
}

void client_session::http2_send_enddata(unsigned int s_stream_id)
{
    unsigned char _recvack[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
    _recvack[8]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[7]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[6]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[5]              = s_stream_id & 0x7F;

    http2_ring_queue->push(_recvack, 9);
}

void client_session::http2_send_rst_stream(unsigned int s_stream_id, unsigned int stream_error_code)
{
    unsigned char _recvack[] = {0x00, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    _recvack[8]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[7]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[6]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[5]              = s_stream_id & 0x7F;

    _recvack[12]      = stream_error_code & 0xFF;
    stream_error_code = stream_error_code >> 8;
    _recvack[11]      = stream_error_code & 0xFF;
    stream_error_code = stream_error_code >> 8;
    _recvack[10]      = stream_error_code & 0xFF;
    stream_error_code = stream_error_code >> 8;
    _recvack[9]       = stream_error_code & 0xFF;

    http2_ring_queue->push(_recvack, 13);
}
asio::awaitable<void> client_session::co_http2_send_enddata(unsigned int s_stream_id)
{
    unsigned char _recvack[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
    _recvack[8]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[7]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[6]              = s_stream_id & 0xFF;
    s_stream_id              = s_stream_id >> 8;
    _recvack[5]              = s_stream_id & 0x7F;

    http2_ring_queue->push(_recvack, 9);
    co_return;
}
asio::awaitable<void> client_session::http2_send_ping()
{
    unsigned char _recvack[] =
        {0x00, 0x00, 0x08, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    http2_ring_queue->push(_recvack, 17);

    co_return;
}

void client_session::send_ping()
{
    unsigned char _recvack[] =
        {0x00, 0x00, 0x08, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    http2_ring_queue->push(_recvack, 17);
}

asio::awaitable<void> client_session::co_send_zero_data(unsigned int stream_id)
{
    static std::string _recvack = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

    _recvack[8] = stream_id & 0xFF;
    stream_id   = stream_id >> 8;
    _recvack[7] = stream_id & 0xFF;
    stream_id   = stream_id >> 8;
    _recvack[6] = stream_id & 0xFF;
    stream_id   = stream_id >> 8;
    _recvack[5] = stream_id & 0x7F;

    http2_ring_queue->push(_recvack);

    co_return;
}

void client_session::send_zero_data(unsigned int stream_id)
{
    static std::string _recvack = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

    _recvack[8] = stream_id & 0xFF;
    stream_id   = stream_id >> 8;
    _recvack[7] = stream_id & 0xFF;
    stream_id   = stream_id >> 8;
    _recvack[6] = stream_id & 0xFF;
    stream_id   = stream_id >> 8;
    _recvack[5] = stream_id & 0x7F;

    http2_ring_queue->push(_recvack);
}

asio::awaitable<void> client_session::co_send_goway()
{
    const static std::string _recvack = {0x00, 0x00, 0x08, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    http2_ring_queue->push(_recvack);

    co_return;
}

void client_session::send_recv_setting()
{
    unsigned char _recvack[] = {0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00};
    http2_ring_queue->push(_recvack, 9);
}
void client_session::send_window_update(unsigned int up_num, unsigned int stmid)
{
    unsigned char _recvack[] = {0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    _recvack[12]             = up_num & 0xFF;
    up_num                   = up_num >> 8;
    _recvack[11]             = up_num & 0xFF;
    up_num                   = up_num >> 8;
    _recvack[10]             = up_num & 0xFF;
    up_num                   = up_num >> 8;
    _recvack[9]              = up_num & 0xFF;

    std::string msg;
    msg.reserve(32);
    msg.resize(13);
    for (int i = 0; i < 13; i++)
    {
        msg[i] = _recvack[i];
    }

    _recvack[8] = stmid & 0xFF;
    stmid       = stmid >> 8;
    _recvack[7] = stmid & 0xFF;
    stmid       = stmid >> 8;
    _recvack[6] = stmid & 0xFF;
    stmid       = stmid >> 8;
    _recvack[5] = stmid & 0xFF;

    for (int i = 0; i < 13; i++)
    {
        msg.push_back(_recvack[i]);
    }
    http2_ring_queue->push(msg);
}
void client_session::recv_window_update(unsigned int up_num, unsigned int stmid)
{
    unsigned char _recvack[] = {0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    _recvack[12]             = up_num & 0xFF;
    up_num                   = up_num >> 8;
    _recvack[11]             = up_num & 0xFF;
    up_num                   = up_num >> 8;
    _recvack[10]             = up_num & 0xFF;
    up_num                   = up_num >> 8;
    _recvack[9]              = up_num & 0xFF;

    std::string msg;
    msg.reserve(32);
    msg.resize(13);
    for (int i = 0; i < 13; i++)
    {
        msg[i] = _recvack[i];
    }

    _recvack[8] = stmid & 0xFF;
    stmid       = stmid >> 8;
    _recvack[7] = stmid & 0xFF;
    stmid       = stmid >> 8;
    _recvack[6] = stmid & 0xFF;
    stmid       = stmid >> 8;
    _recvack[5] = stmid & 0xFF;

    // msg.resize(13);
    for (int i = 0; i < 13; i++)
    {
        msg.push_back(_recvack[i]);
    }
    http2_ring_queue->push(msg);
}

bool client_session::send_data(const unsigned char *buffer, unsigned int buffersize)
{
    if (isclose)
    {
        return false;
    }

    try
    {
        if (isssl)
        {
            if (sslsocket->lowest_layer().is_open())
            {
                asio::write(*sslsocket, asio::buffer(buffer, buffersize));
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (socket->is_open())
            {
                asio::write(*socket, asio::buffer(buffer, buffersize));
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    catch (std::exception &)
    {
        isclose = true;
        iserror = true;
        return false;
    }
}

// void client_session::append(const unsigned char *buffer, unsigned int buffersize)
// {
//     std::unique_lock lk(http2_loop_send_mutex);
//     http2_ring_queue_temp.append(reinterpret_cast<const char *>(buffer), buffersize);
//     lk.unlock();
// }

// void client_session::append(const std::string &item)
// {
//     std::unique_lock lk(http2_loop_send_mutex);
//     http2_ring_queue_temp.append(item);
//     lk.unlock();
// }

void client_session::waituphttp2(asio::io_context &ioc)
{
    try
    {
        std::unique_lock lk(waituphttp2_mutex);
        http2_need_wakeup = false;
        if (user_code_handler_call.size() > 0)
        {
            //auto ex = asio::get_associated_executor(user_code_handler_call.front());
            auto handle = std::move(user_code_handler_call.front());
            user_code_handler_call.pop_front();
            lk.unlock();
            asio::dispatch(ioc,
                           [handler =std::move(handle) ]() mutable -> void
                           {
                               /////////////
                               handler(1);
                               //////////
                           });
            
            DEBUG_LOG("peer_session user_code_handler_call return");
        }
        else
        {
            lk.unlock();
        }
    }
    catch (...)
    {
        DEBUG_LOG("peer_session user_code_handler_call error");
    }
}
asio::awaitable<void> client_session::co_send_writer(const unsigned char *buffer, unsigned int buffersize)
{
    if (isclose)
    {
        co_return;
    }
    try
    {
        if (buffersize == 0)
        {
            co_return;
        }
        if (isssl)
        {
            co_await asio::async_write(*sslsocket, asio::buffer(buffer, buffersize), asio::use_awaitable);
        }
        else
        {
            co_await asio::async_write(*socket, asio::buffer(buffer, buffersize), asio::use_awaitable);
        }
    }
    catch (...)
    {
        isclose = true;
        iserror = true;
    }
    co_return;
}

asio::awaitable<void> client_session::co_send_writer(const std::string &msg)
{
    if (isclose)
    {
        co_return;
    }
    try
    {
        if (msg.size() == 0)
        {
            co_return;
        }
        if (isssl)
        {
            co_await asio::async_write(*sslsocket, asio::buffer(msg), asio::use_awaitable);
        }
        else
        {
            co_await asio::async_write(*socket, asio::buffer(msg), asio::use_awaitable);
        }
    }
    catch (...)
    {
        isclose = true;
        iserror = true;
    }
    co_return;
}
void client_session::half_stop()
{
    half_close = true;
    if (iserror)
    {
        isclose = true;
    }
}
asio::awaitable<void> client_session::async_stop()
{
    DEBUG_LOG("socket async_stop");
    isclose = true;
    if (iserror)
    {
        co_return;
    }
    try
    {
        if (isssl)
        {
            co_await sslsocket->async_shutdown(asio::use_awaitable);
            if (sslsocket->lowest_layer().is_open())
            {
                sslsocket->lowest_layer().close();
            }
        }
        else
        {
            socket->close();
        }
    }
    catch (...)
    {
        DEBUG_LOG("socket exp ");
    }
    // timer_.cancel();
}

void client_session::stop()
{
    DEBUG_LOG("socket stop");
    isclose = true;
    if (iserror)
    {
        return;
    }
    try
    {
        if (isssl)
        {
            asio::error_code ec;
            sslsocket->shutdown(ec);
            if (sslsocket->lowest_layer().is_open())
            {
                sslsocket->lowest_layer().close();
            }
        }
        else
        {
            socket->close();
        }
    }
    catch (...)
    {
        DEBUG_LOG("socket exp ");
    }
    // timer_.cancel();
}

std::string client_session::getremoteip()
{
    std::string client_ip;
    if (iserror)
    {
        return "";
    }
    if (isssl)
    {
        client_ip = sslsocket->lowest_layer().remote_endpoint().address().to_string();
    }
    else
    {
        client_ip = socket->remote_endpoint().address().to_string();
    }
    return client_ip;
}
unsigned int client_session::getremoteport()
{
    unsigned int client_port = 0;
    if (iserror)
    {
        return 0;
    }
    if (isssl)
    {
        client_port = sslsocket->lowest_layer().remote_endpoint().port();
    }
    else
    {
        client_port = socket->remote_endpoint().port();
    }
    return client_port;
}
std::string client_session::getlocalip()
{
    std::string server_ip;
    if (iserror)
    {
        return "";
    }
    if (isssl)
    {
        server_ip = sslsocket->lowest_layer().local_endpoint().address().to_string();
    }
    else
    {
        server_ip = socket->local_endpoint().address().to_string();
    }
    return server_ip;
}
unsigned int client_session::getlocalport()
{
    unsigned int server_port = 0;
    if (iserror)
    {
        return 0;
    }
    if (isssl)
    {
        server_port = sslsocket->lowest_layer().local_endpoint().port();
    }
    else
    {
        server_port = socket->local_endpoint().port();
    }
    return server_port;
}

}// namespace http
