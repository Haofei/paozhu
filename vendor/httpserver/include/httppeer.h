#ifndef HTTP_PEER_H
#define HTTP_PEER_H

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include <sstream>
#include <map>
#include <list>
#include <filesystem>
#include <functional>
#include <string_view>
#include <type_traits>
#include <concepts>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

#include "request.h"
#include "client_session.h"
#include "websockets_parse.h"
#include "websockets.h"

namespace http
{
template <typename T>
concept VALNUM_T = std::is_floating_point_v<T> || std::is_integral_v<T>;

class httppeer : public std::enable_shared_from_this<httppeer>
{
  public:
    unsigned char get_fileinfo();
    void send_files(std::string);
    void status(unsigned int);
    unsigned int get_status();
    void type(const std::string &);
    void length(unsigned long long);
    void scheme(unsigned char);
    std::shared_ptr<httppeer> get_ptr();
    std::string make_http2_header(unsigned char flag_code = 0);
    std::string make_http1_header();
    void set_header(const std::string &, const std::string &);
    void get_header(const std::string &);
    void get_cookie(const std::string &);

    void flush_out();

    void set_cookie(std::string key,
                    std::string val,
                    long long exptime      = 0,
                    std::string path       = "",
                    std::string domain     = "",
                    bool secure            = false,
                    bool httponly          = true,
                    std::string issamesite = "");
    bool is_ssl();
    std::list<std::string> cookietoheader();
    std::string get_hosturl();
    std::string get_sitepath();
    unsigned long long get_siteid();
    unsigned long long get_groupid();
    std::string get_theme();
    std::string get_themeurl();
    unsigned char has_urlfileext();
    bool isuse_fastcgi();
    void goto_url(const std::string &url, unsigned char second = 0, const std::string &msg = "");
    bool isset_type();

    void parse_session();
    void save_session();
    void clear_session();
    std::string get_session_id();
    void set_session_id(const std::string &a);
    void parse_session_file(std::string &sessionfile_id);
    void parse_session_memory(std::string &sessionfile_id);
    void save_session_memory(std::string &sessionfile);
    void save_session_file(std::string &sessionfile);

    void view(const std::string &a);
    void view(const std::string &a, obj_val &b);
    std::string fetchview(const std::string &a);
    std::string fetchview(const std::string &a, obj_val &b);
    void send(const std::string &a);
    httppeer &out(std::string);
    httppeer &out(const std::string &a);

    httppeer &operator<<(obj_val &a);
    httppeer &operator<<(std::string &&a);
    httppeer &operator<<(std::string &a);
    httppeer &operator<<(std::string_view a);
    httppeer &operator<<(char a);
    httppeer &operator<<(unsigned char a);
    httppeer &operator<<(const char *a);
    httppeer &operator<<(const std::string &a);
    httppeer &operator<<(unsigned int a);
    httppeer &operator<<(int a);
    httppeer &operator<<(unsigned long long a);
    httppeer &operator<<(unsigned long a);
    httppeer &operator<<(long long a);
    httppeer &operator<<(float a);
    httppeer &operator<<(double a);

    template <typename T>
    httppeer &operator<<(VALNUM_T auto a);

    template <typename T>
    httppeer &operator<<(T a);
    template <typename T>
    httppeer &operator<<(T *a);

    httppeer &get_peer();
    void out_json(obj_val &a);
    void out_json();
    void json_type();

    void cors_domain(const std::string &, const std::string &header_v = "");
    void cors_method(const std::string &header_v = "");
    void push_flow(const std::string &);
    void push_front_flow(const std::string &);
    std::string pop_flow();
    unsigned char add_timeloop_task(const std::string &, unsigned int);
    void clear_timeloop_task();
    unsigned int get_timeloop_count();
    void add_timeloop_count(unsigned int a = 1);
    void clsoesend(asio::io_context &);

    bool find_host_index();
    unsigned int check_upload_limit();

  public:
    std::string host;
    std::string url;
    std::string urlpath;
    std::string querystring;

    std::string content_type;
    std::string etag;

    std::string chartset;
    std::string accept_language;
    std::string rawheader;
    std::string rawcontent;
    std::map<std::string, std::string> header;
    http::obj_val get;
    http::obj_val post;
    http::obj_val files;
    http::obj_val json;
    http::obj_val val;
    cookie cookie;
    std::vector<std::string> pathinfos;

    bool issendheader = false;
    bool ischunked    = false;
    bool isfinish     = false;
    bool issend       = false;
    bool isclose      = false;
    bool isssl        = false;
    bool keepalive    = true;
    bool isso         = false;

    unsigned char posttype     = 0;
    unsigned char compress     = 0;
    unsigned int host_index    = 0;
    unsigned int stream_id     = 0;
    unsigned int status_code   = 0;
    unsigned int timeloop_num  = 0;
    unsigned int timecount_num = 0;
    unsigned int request_time  = 0;
    //unsigned int time_limit             = 0;
    unsigned long long content_length   = 0;
    unsigned long long sessionfile_time = 0;
    unsigned long long upload_length    = 0;

    //std::atomic_uint time_limit  = 0;
    struct headstate_t state;
    struct websocket_t websocket;

    std::unique_ptr<websocketparse> ws;
    std::shared_ptr<websockets_api> websockets;

    http::obj_val session;

    //cookie send_cookie;
    std::list<std::string> send_cookie_lists;
    std::map<std::string, std::string> send_header;
    std::map<unsigned char, std::string> http2_send_header;

    struct stat fileinfo;
    std::string output;
    std::string sitepath;
    std::string sendfilename;
    std::unique_ptr<std::list<std::string>> flow_method;
    unsigned char sendfiletype;
    unsigned char linktype;
    unsigned char method;
    unsigned char httpv;

    std::string server_ip;
    std::string client_ip;
    unsigned int client_port;
    unsigned int server_port;

    std::shared_ptr<client_session> socket_session = nullptr;

    std::list<asio::detail::awaitable_handler<asio::any_io_executor, size_t>> user_code_handler_call;
    std::mutex pop_user_handleer_mutex;
    // 等待滑动窗口
    // std::atomic_bool window_update_bool = false;
    // std::list<std::future<int>> window_update_results;
    // std::promise<int> window_update_promise;
};
struct regmethold_t
{
    std::function<std::string(std::shared_ptr<httppeer>)> pre = nullptr;
    std::function<std::string(std::shared_ptr<httppeer>)> regfun;
};
struct regmethold_co_t
{
    std::function<asio::awaitable<std::string>(std::shared_ptr<httppeer>)> pre = nullptr;
    std::function<asio::awaitable<std::string>(std::shared_ptr<httppeer>)> regfun;
};
// extern std::map<std::string, std::function<std::string(std::shared_ptr<httppeer>)>> _http_regmethod_table;
extern std::map<std::string, regmethold_t> _http_regmethod_table;
extern std::map<std::string, std::map<std::string, regmethold_t>> _domain_regmethod_table;

extern std::map<std::string, regmethold_co_t> _co_http_regmethod_table;
extern std::map<std::string, std::map<std::string, regmethold_co_t>> _co_domain_regmethod_table;

extern std::map<std::string, std::vector<std::string>> _http_regurlpath_table;
extern std::map<std::string, std::map<std::string, std::vector<std::string>>> _domain_regurlpath_table;
void make_404_content(std::shared_ptr<httppeer> peer);
}// namespace http
#endif
