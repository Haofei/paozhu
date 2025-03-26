/**
 *  @copyright copyright 2022, huang ziquan  All rights reserved.
 *  @author huang ziquan
 *  @author 黄自权
 *  @file httppeer.cpp
 *  @date 2022-11-12
 *
 *  http client peer file
 *
 *
 */
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
#include <concepts>
#include <string_view>
#include <type_traits>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

#ifndef _MSC_VER
#include <sys/fcntl.h>
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif
#include "cost_define.h"
#include "request.h"
#include "datetime.h"
#include "client_session.h"
#include "viewso_param.h"
#include "httppeer.h"
#include "serverconfig.h"
#include "http2_frame.h"
#include "http2_huffman.h"
#include "terminal_color.h"

#ifdef ENABLE_BOOST
#include "loadmodule.h"
#include "loadviewso.h"
#include "http_so_common_api.h"
#endif
#include "viewmethold_reg.h"
#include "debug_log.h"
#include "server_localvar.h"
#include "pzcache.h"
#include "func.h"

namespace http
{

std::map<std::string, regmethold_t> _http_regmethod_table;
std::map<std::string, regmethold_co_t> _co_http_regmethod_table;
std::map<std::string, std::vector<std::string>> _http_regurlpath_table;
std::map<std::string, std::map<std::string, regmethold_t>> _domain_regmethod_table;
std::map<std::string, std::map<std::string, regmethold_co_t>> _co_domain_regmethod_table;
std::map<std::string, std::map<std::string, std::vector<std::string>>> _domain_regurlpath_table;
void make_404_content(std::shared_ptr<httppeer> peer)
{
    peer->output = "<h3>404 Not Found</h3>";
    peer->output.append("<hr /><p>File: " + peer->urlpath + " </p>");
    peer->status(404);
    peer->type("text/html; charset=utf-8");
}
void httppeer::clear_timeloop_task() { timeloop_num = 0; }
unsigned int httppeer::get_timeloop_count() { return timecount_num; }
void httppeer::add_timeloop_count(unsigned int a)
{
    if (a == 0)
    {
        timecount_num = 0;
    }
    else
    {
        timecount_num++;
    }
}
unsigned char httppeer::add_timeloop_task(const std::string &path_method, unsigned int count_id)
{
    linktype      = 7;
    timeloop_num  = count_id;
    timecount_num = 1;
    if (pathinfos.size() == 0)
    {
        pathinfos.push_back(path_method);
    }
    else
    {
        pathinfos[0] = path_method;
    }
    std::string temptaskhash = path_method;
    temptaskhash.append(url);
    std::size_t temp_name_id = std::hash<std::string>{}(temptaskhash);
    std::ostringstream oss;
    oss << temp_name_id;
    etag = oss.str();
    return linktype;
}
void httppeer::send(const std::string &a)
{
    if (httpv == 2)
    {
        return;
    }
    if (socket_session)
    {
        socket_session->send_data(a);
    }
}
void httppeer::flush_out()
{
    // current empty chunk out
    send_header["Transfer-Encoding"] = "chunked";
    ischunked                        = true;
    if (httpv == 2)
    {
        return;
    }
    if (socket_session)
    {
        return;
    }
}
void httppeer::parse_session_file(const std::string &sessionfile)
{
    std::string root_path,temp_session_file;
    server_loaclvar &localvar = get_server_global_var();
    root_path                 = localvar.temp_path;
    //sessionfile.append("_sess");
    root_path.append(sessionfile);
    root_path.append("_sess");

    struct stat sessfileinfo;
    unsigned long long tempsesstime = 0;
    unsigned long long vistsesstime = 0;
    memset(&sessfileinfo, 0, sizeof(sessfileinfo));
    if (stat(root_path.c_str(), &sessfileinfo) == 0)
    {
        if (sessfileinfo.st_mode & S_IFREG)
        {
            tempsesstime = sessfileinfo.st_mtime;
            vistsesstime = sessfileinfo.st_atime;
        }
    }

    unsigned long long reseetime = timeid();

    if (reseetime > (vistsesstime + 5400))
    {
        temp_session_file = cookie.get(COOKIE_SESSION_NAME);
        // cookie.set(COOKIE_SESSION_NAME, sessionfile, 7200, "/", host);
        // send_cookie.set(COOKIE_SESSION_NAME, sessionfile, 7200, "/", host);
        set_cookie(COOKIE_SESSION_NAME, temp_session_file, 30000, host, "/");
    }

    if (tempsesstime > 0 && tempsesstime == sessionfile_time)
    {
        return;
    }
#ifndef _MSC_VER
    int fd = open(root_path.c_str(), O_RDONLY);
    if (fd == -1)
    {
        // perror("open");
        return;
    }

#ifndef _WIN32
    // 锁住整个文件
    struct flock lock = {};
    lock.l_type       = F_RDLCK;
    lock.l_whence     = 0;
    lock.l_start      = 0;
    lock.l_len        = 0;

    lock.l_pid = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        return;
    }
#else
    // lock file by using win32 api
    auto native_handle = (HANDLE)_get_osfhandle(fd);
    if (!LockFile(native_handle, 0, 0, 0, MAXDWORD))
    {
        return;
    }
#endif

    int filelen = lseek(fd, 0L, SEEK_END);
    temp_session_file.clear();
    temp_session_file.resize(filelen);
    lseek(fd, 0L, SEEK_SET);
    int readsize = read(fd, temp_session_file.data(), filelen);
    if (readsize > 0)
    {
        temp_session_file.resize(readsize);
        session.from_json(temp_session_file);
    }

#ifndef _WIN32
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {

        return;
    }
#else
    if (!UnlockFile(native_handle, 0, 0, 0, MAXDWORD))
    {
        return;
    }
#endif

    close(fd);
#endif

    sessionfile_time = tempsesstime;
}
void httppeer::parse_session_memory(const std::string &sessionfile_id)
{
    http::pzcache<http::obj_val> &temp_cache = http::pzcache<http::obj_val>::conn();
    temp_cache.update(sessionfile_id, 30000);
    session = temp_cache.get(sessionfile_id);
}
std::string httppeer::get_session_id()
{
    if (cookie.check(COOKIE_SESSION_NAME))
    {
        return cookie.get(COOKIE_SESSION_NAME);
    }
    return "";
}
void httppeer::set_session_id(const std::string &a)
{
    // cookie.set(COOKIE_SESSION_NAME, a, 7200, "/", host);
    // send_cookie.set(COOKIE_SESSION_NAME, a, timeid() + 7200 * 12, "/", host);
    set_cookie(COOKIE_SESSION_NAME, a, (timeid() + 7200 * 12), host, "/");
    parse_session();
}
void httppeer::parse_session()
{

    if (cookie.check(COOKIE_SESSION_NAME))
    {
        server_loaclvar &localvar = get_server_global_var();
        std::string sessionfile   = cookie.get(COOKIE_SESSION_NAME);
        if (sessionfile.empty())
        {
            return;
        }
        for (unsigned int i = 0; i < sessionfile.size(); i++)
        {
            if (sessionfile[i] == '/')
            {
                // cookie.set(COOKIE_SESSION_NAME, sessionfile, timeid() - 7200, "/", host);
                // send_cookie.set(COOKIE_SESSION_NAME, sessionfile, timeid() - 7200, "/", host);
                set_cookie(COOKIE_SESSION_NAME, sessionfile, timeid() - 7200, host, "/");
                return;
            }
        }
        switch (localvar.session_type)
        {
        case 0: parse_session_file(sessionfile); break;
        case 1: parse_session_memory(sessionfile); break;
        default: parse_session_file(sessionfile); break;
        }
    }
}
void httppeer::save_session()
{
    server_loaclvar &localvar = get_server_global_var();
    std::string sessionfile;
    if (cookie.check(COOKIE_SESSION_NAME))
    {
        sessionfile = cookie.get(COOKIE_SESSION_NAME);
    }

    if (sessionfile.size() > 0)
    {
        unsigned int j = 0;
        for (unsigned int i = 0; i < sessionfile.size(); i++)
        {
            if (sessionfile[i] == '.' || sessionfile[i] == '/')
            {
                continue;
            }
            sessionfile[j] = sessionfile[i];
            j++;
        }

        if (j > 0)
        {
            sessionfile.resize(j);
        }
        else
        {
            sessionfile.clear();
        }
    }

    if (sessionfile.empty())
    {
        sessionfile =
            client_ip + std::to_string(client_port) + std::to_string(timeid()) + std::to_string(rand_range(1000, 9999));
        sessionfile = std::to_string(std::hash<std::string>{}(sessionfile));
        // cookie.set(COOKIE_SESSION_NAME, sessionfile, 7200, "/", host);
        // send_cookie.set(COOKIE_SESSION_NAME, sessionfile, 7200, "/", host);
        set_cookie(COOKIE_SESSION_NAME, sessionfile, 30000, host, "/");
    }
    if (localvar.session_type == 1)
    {
        save_session_memory(sessionfile);
    }
    else
    {
        save_session_file(sessionfile);
    }
}
void httppeer::save_session_memory(const std::string &sessionfile)
{
    pzcache<obj_val> &temp_cache = pzcache<obj_val>::conn();
    temp_cache.save(sessionfile, session, 30000, true);
}
void httppeer::save_session_file(const std::string &sessionfile)
{

    std::string root_path,temp_session_file;
    // serverconfig &sysconfigpath = getserversysconfig();
    server_loaclvar &localvar = get_server_global_var();
    root_path                 = localvar.temp_path;

    // sessionfile.append("_sess");
    root_path.append(sessionfile);
    root_path.append("_sess");
#ifndef _MSC_VER
    int fd = open(root_path.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        // perror("open");
        return;
    }

#ifndef _WIN32
    // 锁住整个文件
    struct flock lock = {};
    lock.l_type       = F_WRLCK;
    lock.l_whence     = 0;
    lock.l_start      = 0;
    lock.l_len        = 0;

    lock.l_pid = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        return;
    }
#else
    // lock file by using win32 api
    auto native_handle = (HANDLE)_get_osfhandle(fd);
    if (!LockFile(native_handle, 0, 0, 0, MAXDWORD))
    {
        return;
    }
#endif

    temp_session_file = session.to_json();

    ssize_t n = write(fd, temp_session_file.data(), temp_session_file.size());
    if (n > 0)
    {
        n = 0;
    }

#ifndef _WIN32
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {

        return;
    }
#else
    if (!UnlockFile(native_handle, 0, 0, 0, MAXDWORD))
    {
        return;
    }
#endif

    close(fd);
#endif
    sessionfile_time = timeid();
}
void httppeer::clear_session()
{
    // serverconfig &sysconfigpath = getserversysconfig();
    if (cookie.check(COOKIE_SESSION_NAME))
    {
        std::string root_path;
        server_loaclvar &localvar = get_server_global_var();
        root_path                 = localvar.temp_path;

        std::string sessionfile = cookie.get(COOKIE_SESSION_NAME);
        if (sessionfile.empty())
        {
            return;
        }
        if (localvar.session_type == 1)
        {
            pzcache<obj_val> &temp_cache = pzcache<obj_val>::conn();
            temp_cache.remove(sessionfile);
            return;
        }

        sessionfile.append("_sess");
        root_path.append(sessionfile);

        struct stat sessfileinfo;
        // unsigned long long tempsesstime = 0;
        memset(&sessfileinfo, 0, sizeof(sessfileinfo));
        if (stat(root_path.c_str(), &sessfileinfo) == 0)
        {

            if (sessfileinfo.st_mode & S_IFREG)
            {
                remove(root_path.c_str());
                sessionfile_time = 0;
                session.clear();
                // cookie.set(COOKIE_SESSION_NAME, sessionfile, timeid() - 7200, "/", host);
                // send_cookie.set(COOKIE_SESSION_NAME, sessionfile, timeid() - 7200, "/", host);
                set_cookie(COOKIE_SESSION_NAME, sessionfile, timeid() - 7200, host, "/");
            }
        }
        else
        {
            return;
        }
    }
}
std::string httppeer::get_sitepath() { return getserversysconfig().getsitewwwpath(host_index); }
unsigned long long httppeer::get_siteid()
{
    serverconfig &sysconfigpath = getserversysconfig();
    if (host_index >= sysconfigpath.sitehostinfos.size())
    {
        return 0;
    }
    return sysconfigpath.sitehostinfos[host_index].siteid;
}
unsigned long long httppeer::get_groupid()
{
    serverconfig &sysconfigpath = getserversysconfig();
    if (host_index >= sysconfigpath.sitehostinfos.size())
    {
        return 0;
    }
    return sysconfigpath.sitehostinfos[host_index].groupid;
}
std::string httppeer::get_theme()
{
    serverconfig &sysconfigpath = getserversysconfig();
    if (host_index >= sysconfigpath.sitehostinfos.size())
    {
        return "";
    }
    return sysconfigpath.sitehostinfos[host_index].themes;
}
std::string httppeer::get_themeurl()
{
    serverconfig &sysconfigpath = getserversysconfig();
    if (host_index >= sysconfigpath.sitehostinfos.size())
    {
        return "";
    }
    return sysconfigpath.sitehostinfos[host_index].themes_url;
}
bool httppeer::is_ssl()
{
    if (isssl)
    {
        return true;
    }
    else
    {
        return false;
    }
}
std::string httppeer::get_hosturl()
{
    std::string tempurl;
    if (isssl)
    {
        tempurl.append("https://");
    }
    else
    {
        tempurl.append("http://");
    }
    tempurl.append(host);
    if (isssl)
    {
        if (server_port != 443)
        {
            tempurl.push_back(':');
            tempurl.append(std::to_string(server_port));
        }
    }
    else
    {
        if (server_port != 80)
        {
            tempurl.push_back(':');
            tempurl.append(std::to_string(server_port));
        }
    }
    return tempurl;
}
unsigned int httppeer::check_upload_limit()
{
    serverconfig &sysconfigpath = getserversysconfig();
    if (sysconfigpath.sitehostinfos[host_index].is_limit_upload && content_length > sysconfigpath.sitehostinfos[host_index].upload_max_size)
    {
        DEBUG_LOG("upload file size limit %lld", content_length);
        return 403;
    }
    return 0;
}
bool httppeer::find_host_index()
{
    serverconfig &sysconfigpath = getserversysconfig();
    auto hiter                  = sysconfigpath.host_toint.find(host);
    if (hiter != sysconfigpath.host_toint.end())
    {
        host_index = hiter->second;
        if (host_index >= sysconfigpath.sitehostinfos.size())
        {
            host_index = 0;
            return false;
        }
        return true;
    }
    return false;
}
bool httppeer::isuse_fastcgi()
{
    serverconfig &sysconfigpath = getserversysconfig();
    compress                    = 0;
    linktype                    = 0;
    output.clear();
    sendfilename.clear();
    if (sysconfigpath.sitehostinfos[host_index].isuse_php == 1)
    {
        DEBUG_LOG("check php file");
        if (pathinfos.size() > 0)
        {
            //sendfilename.clear();
            for (unsigned int i = 0; i < pathinfos.size(); i++)
            {
                unsigned int extfilesize = pathinfos[i].size();
                if (extfilesize > 4 && pathinfos[i][extfilesize - 1] == 'p' && pathinfos[i][extfilesize - 2] == 'h' &&
                    pathinfos[i][extfilesize - 3] == 'p' && pathinfos[i][extfilesize - 4] == '.')
                {
                    if (sendfilename.size() > 0)
                    {
                        sendfilename.append("/");
                    }
                    sendfilename.append(pathinfos[i]);
                    struct stat sessfileinfo;
                    std::string tempac = sysconfigpath.sitehostinfos[host_index].php_root_document + sendfilename;
                    DEBUG_LOG("php file %s", tempac.c_str());
                    memset(&sessfileinfo, 0, sizeof(sessfileinfo));
                    if (stat(tempac.c_str(), &sessfileinfo) == 0)
                    {
                        if (sessfileinfo.st_mode & S_IFREG)
                        {
                            compress = 10;
                            if (i == 0)
                            {
                                linktype = 10;
                            }
                            else
                            {
                                linktype = 50 + i;
                            }
                            DEBUG_LOG("is php file %s", sendfilename.c_str());
                            return true;
                        }
                    }
                }
                else
                {
                    if (sendfilename.size() > 0)
                    {
                        sendfilename.append("/");
                    }
                    sendfilename.append(pathinfos[i]);
                    //check url path reg points
                    if (_http_regmethod_table.contains(sendfilename))
                    {
                        return false;
                    }
                }
            }

            DEBUG_LOG("check urlpath reg %s", sendfilename.c_str());
            // if (_http_regmethod_table.contains(sendfilename))
            // {
            //     return false;
            // }
            // else
            // {
            struct stat sessfileinfo;
            std::string tempac = sysconfigpath.sitehostinfos[host_index].wwwpath + sendfilename;
            memset(&sessfileinfo, 0, sizeof(sessfileinfo));
            if (stat(tempac.c_str(), &sessfileinfo) == 0)
            {
                if (sessfileinfo.st_mode & S_IFREG)
                {
                    compress = 0;
                    linktype = 0;
                    return false;
                }
                else if (sessfileinfo.st_mode & S_IFDIR)
                {
                    compress = 0;
                    linktype = 0;
                    tempac   = sysconfigpath.sitehostinfos[host_index].php_root_document + sendfilename + "/index.php";
                    memset(&sessfileinfo, 0, sizeof(sessfileinfo));
                    if (stat(tempac.c_str(), &sessfileinfo) == 0)
                    {
                        if (sessfileinfo.st_mode & S_IFREG)
                        {
                            compress = 10;
                            linktype = 15;
                            sendfilename.append("/index.php");
                            return true;
                        }
                    }
                    return false;
                }
            }

            DEBUG_LOG("rewrite_php_lists: %zu", sysconfigpath.sitehostinfos[host_index].rewrite_php_lists.size());
            if (sysconfigpath.sitehostinfos[host_index].rewrite_php_lists.size() > 0)
            {
                unsigned int i = 0;
                if (sysconfigpath.sitehostinfos[host_index].rewrite_php_lists[0].first.size() == 0)
                {
                    tempac = sysconfigpath.sitehostinfos[host_index].php_root_document + sysconfigpath.sitehostinfos[host_index].rewrite_php_lists[0].second;
                    memset(&sessfileinfo, 0, sizeof(sessfileinfo));
                    if (stat(tempac.c_str(), &sessfileinfo) == 0)
                    {
                        if (sessfileinfo.st_mode & S_IFREG)
                        {
                            compress = 10;
                            linktype = 19;
                            return true;
                        }
                    }
                    i = 1;
                }
                for (; i < sysconfigpath.sitehostinfos[host_index].rewrite_php_lists.size(); i++)
                {
                    unsigned int j = 0;

                    for (; j < sysconfigpath.sitehostinfos[host_index].rewrite_php_lists[i].first.size(); j++)
                    {
                        if (j < sendfilename.size() && sysconfigpath.sitehostinfos[host_index].rewrite_php_lists[i].first[j] == sendfilename[j])
                        {
                            continue;
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (j == sysconfigpath.sitehostinfos[host_index].rewrite_php_lists[i].first.size())
                    {
                        compress = 10;
                        linktype = 20 + i;
                        return true;
                    }
                }
            }
            //}
        }
        else
        {

            struct stat sessfileinfo;
            std::string tempac = sysconfigpath.sitehostinfos[host_index].wwwpath + sysconfigpath.sitehostinfos[host_index].document_index;
            memset(&sessfileinfo, 0, sizeof(sessfileinfo));
            if (stat(tempac.c_str(), &sessfileinfo) == 0)
            {
                if (sessfileinfo.st_mode & S_IFREG)
                {
                    return false;
                }
            }

            if (sysconfigpath.sitehostinfos[host_index].php_root_document.size() > 0)
            {
                //struct stat sessfileinfo;
                tempac = sysconfigpath.sitehostinfos[host_index].php_root_document + "index.php";
                memset(&sessfileinfo, 0, sizeof(sessfileinfo));
                if (stat(tempac.c_str(), &sessfileinfo) == 0)
                {
                    if (sessfileinfo.st_mode & S_IFREG)
                    {
                        compress = 10;
                        linktype = 12;
                    }
                }
            }
        }
    }
    return false;
}
unsigned char httppeer::has_urlfileext()
{
    unsigned char temp = 0;
    if (pathinfos.size() > 0)
    {
        for (unsigned i = 0, n = pathinfos.size() - 1; i < pathinfos[n].size(); i++)
        {
            if (pathinfos[n][i] == '.')
            {
                temp = 1;
                break;
            }
        }
    }
    if (temp == 0)
    {
        sendfilename.clear();
        for (unsigned int i = 0; i < pathinfos.size(); i++)
        {
            if (i > 0 && i < pathinfos.size())
            {
                if (_http_regmethod_table.contains(sendfilename))
                {
                    return 4;
                }
                else if (_co_http_regmethod_table.contains(sendfilename))
                {
                    return 40;
                }
            }
            if (i > 0)
            {
                sendfilename.append("/");
            }
            sendfilename.append(pathinfos[i]);
        }
        if (sendfilename.size() > 0)
        {
            unsigned int j = 0;
            unsigned int n = 0;
            for (; n < sendfilename.size(); n++)
            {
                if (sendfilename[n] == '.')
                {
                    if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '.')
                    {
                        n += 1;
                        for (; n < sendfilename.size();)
                        {
                            if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                            {
                                n += 1;
                                continue;
                            }
                            break;
                        }
                        continue;
                    }
                    else if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                    {
                        n += 1;
                        for (; n < sendfilename.size();)
                        {
                            if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                            {
                                n += 1;
                                continue;
                            }
                            break;
                        }
                        continue;
                    }
                }
                j++;
            }
            if (j < sendfilename.size())
            {
                n = 0;
                j = 0;
                for (; n < sendfilename.size(); n++)
                {
                    if (sendfilename[n] == '.')
                    {
                        if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '.')
                        {
                            n += 1;
                            for (; n < sendfilename.size();)
                            {
                                if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                                {
                                    n += 1;
                                    continue;
                                }
                                break;
                            }
                            continue;
                        }
                        else if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                        {
                            n += 1;
                            for (; n < sendfilename.size();)
                            {
                                if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                                {
                                    n += 1;
                                    continue;
                                }
                                break;
                            }
                            continue;
                        }
                    }
                    sendfilename[j] = sendfilename[n];
                    j++;
                }
                sendfilename.resize(j);
            }
            //file
            if (_http_regmethod_table.contains(sendfilename))
            {
                temp = 4;
                //maybe let urlpath functions read index.html filetime
                if (pathinfos.size() == 5)
                {
                    serverconfig &sysconfigpath = getserversysconfig();
                    if (sysconfigpath.siteusehtmlchache)
                    {
                        memset(&fileinfo, 0, sizeof(fileinfo));
                        if (sitepath.size() > 0 && sitepath.back() == '/')
                        {
                            sendfilename = sitepath + "/" + sendfilename;
                        }
                        else
                        {
                            if (sysconfigpath.wwwpath.size() > 0 && sitepath.back() == '/')
                            {
                                sendfilename = sysconfigpath.wwwpath + sendfilename;
                            }
                            else
                            {
                                sendfilename = sysconfigpath.wwwpath + "/" + sendfilename;
                            }
                        }

                        if (sendfilename.size() > 0 && sendfilename.back() != '/')
                        {
                            sendfilename.push_back('/');
                        }
                        sendfilename.append(sysconfigpath.map_value["default"]["index"]);

                        if (stat(sendfilename.c_str(), &fileinfo) == 0)
                        {
                            if (fileinfo.st_mode & S_IFREG)
                            {
                                if (sysconfigpath.siteusehtmlchachetime > 10 &&
                                    sysconfigpath.siteusehtmlchachetime > (timeid() - (unsigned long)fileinfo.st_mtime))
                                {
                                    temp = 5;
                                }
                            }
                        }
                    }
                }
                //end file
            }
            else if (_co_http_regmethod_table.contains(sendfilename))
            {
                temp = 4;
                //maybe let urlpath functions read index.html filetime
                if (pathinfos.size() == 5)
                {
                    serverconfig &sysconfigpath = getserversysconfig();
                    if (sysconfigpath.siteusehtmlchache)
                    {
                        memset(&fileinfo, 0, sizeof(fileinfo));
                        if (sitepath.size() > 0 && sitepath.back() == '/')
                        {
                            sendfilename = sitepath + "/" + sendfilename;
                        }
                        else
                        {
                            if (sysconfigpath.wwwpath.size() > 0 && sitepath.back() == '/')
                            {
                                sendfilename = sysconfigpath.wwwpath + sendfilename;
                            }
                            else
                            {
                                sendfilename = sysconfigpath.wwwpath + "/" + sendfilename;
                            }
                        }

                        if (sendfilename.size() > 0 && sendfilename.back() != '/')
                        {
                            sendfilename.push_back('/');
                        }
                        sendfilename.append(sysconfigpath.map_value["default"]["index"]);

                        if (stat(sendfilename.c_str(), &fileinfo) == 0)
                        {
                            if (fileinfo.st_mode & S_IFREG)
                            {
                                if (sysconfigpath.siteusehtmlchachetime > 10 &&
                                    sysconfigpath.siteusehtmlchachetime > (timeid() - (unsigned long)fileinfo.st_mtime))
                                {
                                    temp = 5;
                                }
                            }
                        }
                    }
                }
                //end file
            }
        }
    }
    return temp;
}
unsigned char httppeer::get_fileinfo()
{

    std::string filenamebase, fileexttype;
    serverconfig &sysconfigpath = getserversysconfig();

    sendfilename = sitepath;
    if (sendfilename.size() > 0)
    {
        if (sendfilename.back() != '/')
        {
            sendfilename.push_back('/');
        }
    }
    else
    {
        sendfilename.push_back('/');
    }
    for (unsigned int i = 0; i < pathinfos.size(); i++)
    {
        if (i > 0)
        {
            sendfilename.append("/");
        }
        sendfilename.append(pathinfos[i]);
    }

    if (sendfilename.size() > 0)
    {
        unsigned int j = 0;
        unsigned int n = 0;
        for (; n < sendfilename.size(); n++)
        {
            if (sendfilename[n] == '.')
            {
                if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '.')
                {
                    n += 1;
                    for (; n < sendfilename.size();)
                    {
                        if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                        {
                            n += 1;
                            continue;
                        }
                        break;
                    }
                    continue;
                }
                else if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                {
                    n += 1;
                    for (; n < sendfilename.size();)
                    {
                        if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                        {
                            n += 1;
                            continue;
                        }
                        break;
                    }
                    continue;
                }
            }
            j++;
        }
        if (j < sendfilename.size())
        {
            n = 0;
            j = 0;
            for (; n < sendfilename.size(); n++)
            {
                if (sendfilename[n] == '.')
                {
                    if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '.')
                    {
                        n += 1;
                        for (; n < sendfilename.size();)
                        {
                            if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                            {
                                n += 1;
                                continue;
                            }
                            break;
                        }
                        continue;
                    }
                    else if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                    {
                        n += 1;
                        for (; n < sendfilename.size();)
                        {
                            if ((n + 1) < sendfilename.size() && sendfilename[n + 1] == '/')
                            {
                                n += 1;
                                continue;
                            }
                            break;
                        }
                        continue;
                    }
                }
                sendfilename[j] = sendfilename[n];
                j++;
            }
            sendfilename.resize(j);
        }
    }

    if (pathinfos.size() > 0)
    {
        filenamebase = pathinfos.back();
    }
    else
    {
        filenamebase.clear();
    }

    unsigned int filebasesize   = filenamebase.size();
    unsigned int filenameoffset = 0;

    if (filebasesize > 0)
    {
        for (filenameoffset = filebasesize - 1; filenameoffset > 0; filenameoffset--)
        {
            if (filenamebase[filenameoffset] == '.')
            {
                break;
            }
            else if (filenamebase[filenameoffset] == '/')
            {
                filenameoffset = filebasesize - 1;
                break;
            }
        }
        filenameoffset += 1;
        for (; filenameoffset < filebasesize; filenameoffset++)
        {
            fileexttype.push_back(filenamebase[filenameoffset]);
        }
    }

    memset(&fileinfo, 0, sizeof(fileinfo));
    sendfiletype = 0;
    if (stat(sendfilename.c_str(), &fileinfo) == 0)
    {

        if (fileinfo.st_mode & S_IFDIR)
        {
            sendfiletype             = 2;
            unsigned int nowpathsize = sendfilename.size();
            if (sendfilename.size() > 0 && sendfilename.back() != '/')
            {
                sendfilename.push_back('/');
            }
            sendfilename.append(sysconfigpath.map_value["default"]["index"]);
            memset(&fileinfo, 0, sizeof(fileinfo));
            if (stat(sendfilename.c_str(), &fileinfo) == 0)
            {
                if (fileinfo.st_mode & S_IFREG)
                {
                    sendfiletype = 1;
                    fileexttype  = "html";
                }
            }
            if (sendfiletype == 2)
            {
                sendfilename.resize(nowpathsize);
            }
        }
        else if (fileinfo.st_mode & S_IFREG)
        {
            sendfiletype = 1;
            // use cahce html ,modulepath same urlpath
            // sample: /module/method/202204/22333.html

            // if(isusehtmlcache&&header->pathinfo.size()>2&&chachefiletime>10&&chachefiletime<(http::timeid()-(unsigned
            // long)fileinfo.st_mtime)){
            //     pathtype=3;
            // }
            if (sysconfigpath.siteusehtmlchache && pathinfos.size() > 3 && sysconfigpath.siteusehtmlchachetime > 10 &&
                sysconfigpath.siteusehtmlchachetime < (timeid() - (unsigned long)fileinfo.st_mtime))
            {
                sendfiletype = 3;
            }
        }
    }
    if (sendfiletype == 0)
    {
        if (sysconfigpath.sitehostinfos[host_index].isrewrite)
        {
            if (sysconfigpath.sitehostinfos[host_index].rewrite404 == 1)
            {
                //check multi 404 rewrite
                if (sysconfigpath.sitehostinfos[host_index].action_404_lists.size() > 0)
                {
                    if (pathinfos.size() > 0)
                    {
                        for (unsigned int k = 0; k < sysconfigpath.sitehostinfos[host_index].action_404_lists.size(); k++)
                        {
                            if (pathinfos[0].size() <= sysconfigpath.sitehostinfos[host_index].action_404_lists[k].size() && str_cmp_pre(sysconfigpath.sitehostinfos[host_index].action_404_lists[k], pathinfos[0], pathinfos[0].size()))
                            {
                                sendfilename = sitepath;
                                sendfilename.append(sysconfigpath.sitehostinfos[host_index].action_404_lists[k]);
                                memset(&fileinfo, 0, sizeof(fileinfo));
                                if (stat(sendfilename.c_str(), &fileinfo) == 0)
                                {
                                    if (fileinfo.st_mode & S_IFREG)
                                    {
                                        sendfiletype = 1;
                                        fileexttype  = "html";
                                        return sendfiletype;
                                    }
                                }
                            }
                        }
                    }
                }
                sendfilename = sitepath;
                if (sysconfigpath.sitehostinfos[host_index].rewrite_404_action.size() > 0)
                {
                    sendfilename.append(sysconfigpath.sitehostinfos[host_index].rewrite_404_action);
                    memset(&fileinfo, 0, sizeof(fileinfo));
                    if (stat(sendfilename.c_str(), &fileinfo) == 0)
                    {
                        if (fileinfo.st_mode & S_IFREG)
                        {
                            sendfiletype = 1;
                            fileexttype  = "html";
                        }
                    }
                }
            }
            else if (sysconfigpath.sitehostinfos[host_index].rewrite404 == 2)
            {
                //dynamic method, use urlpath function
                if (sysconfigpath.sitehostinfos[host_index].action_404_lists.size() > 0)
                {
                    if (pathinfos.size() > 0)
                    {
                        for (unsigned int k = 0; k < sysconfigpath.sitehostinfos[host_index].action_404_lists.size(); k++)
                        {
                            if (pathinfos[0].size() > 2 && sysconfigpath.sitehostinfos[host_index].action_404_lists[k].size() > 2 && str_cmp_pre(sysconfigpath.sitehostinfos[host_index].action_404_lists[k], pathinfos[0], 3))
                            {
                                unsigned int tempsize = pathinfos.size();
                                if (tempsize < 20)
                                {
                                    tempsize += 1;
                                    pathinfos.resize(tempsize);
                                    for (unsigned int j = tempsize - 1; j > 0; j--)
                                    {
                                        pathinfos[j] = pathinfos[j - 1];
                                    }
                                    pathinfos[0] = sysconfigpath.sitehostinfos[host_index].action_404_lists[k];
                                    sendfiletype = 3;
                                    return sendfiletype;
                                }
                            }
                        }
                    }
                }

                if (sysconfigpath.sitehostinfos[host_index].rewrite_404_action.size() > 0)
                {
                    unsigned int tempsize = pathinfos.size();
                    if (tempsize < 20)
                    {
                        tempsize += 1;
                        pathinfos.resize(tempsize);
                        for (unsigned int j = tempsize - 1; j > 0; j--)
                        {
                            pathinfos[j] = pathinfos[j - 1];
                        }
                        pathinfos[0] = sysconfigpath.sitehostinfos[host_index].rewrite_404_action;
                        sendfiletype = 3;
                    }
                }
            }
        }
    }
    return sendfiletype;
}
std::shared_ptr<httppeer> httppeer::get_ptr() { return shared_from_this(); }
void httppeer::send_files(std::string filename)
{
    if (filename.size() > 0)
    {
    }
    // socket_session->send_data("hello world!");
}
unsigned int httppeer::get_status() { return status_code; }
void httppeer::status(unsigned int http_code) { status_code = http_code; }
void httppeer::type(const std::string &a) { content_type = a; }
bool httppeer::isset_type() { return content_type.size() > 0 ? true : false; }
void httppeer::set_header(const std::string &a, const std::string &v)
{
    if (httpv == 2)
    {
        std::string temp = a;
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        if (http2_header_codes_table[temp] > 0)
        {
            http2_send_header[http2_header_codes_table[temp]] = v;
        }
        else
        {
            send_header[temp] = v;
        }
    }
    else
    {
        send_header[a] = v;
    }
}
void httppeer::set_cookie(std::string key,
                          std::string val,
                          long long exptime,
                          std::string domain,
                          std::string path,
                          bool secure,
                          bool httponly,
                          std::string issamesite)
{
    cookie.set(key, val, exptime, domain, path, secure, httponly, issamesite);
    //send_cookie.set(key, val, exptime, domain, path, secure, httponly, issamesite);

    std::string temph;
    temph.append(url_encode(key.data(), key.size()));
    temph.push_back('=');
    temph.append(url_encode(val.data(), val.size()));

    if (exptime > 0 && exptime < 63072000)
    {
        exptime = timeid() + exptime;
    }

    if (exptime > 0)
    {
        temph.append("; Expires=");
        temph.append(get_gmttime((unsigned long long)exptime));
    }

    if (domain.size() > 1)
    {
        temph.append("; Domain=");
        temph.append(domain);
    }

    if (path.size() > 0)
    {
        temph.append("; Path=");
        temph.append(path);
    }

    if (secure)
    {
        temph.append("; Secure");
    }

    if (httponly)
    {
        temph.append("; HttpOnly");
    }
    if (!issamesite.empty())
    {
        switch (issamesite[0])
        {
        case 'L':
        case 'l': temph.append("; SameSite=Lax"); break;
        case 'S':
        case 's': temph.append("; SameSite=Strict"); break;
        }
    }

    send_cookie_lists.emplace_back(temph);
}

std::list<std::string> httppeer::cookietoheader()
{
    // std::string temph;
    // for (auto &iter : send_cookie)
    // {
    //     temph.clear();
    //     std::string key        = iter.first;
    //     std::string domain     = send_cookie.getDomain(key);
    //     std::string domainpath = send_cookie.getPath(key);
    //     unsigned long timeexp  = send_cookie.getExpires(key);
    //     unsigned char issecure = send_cookie.getSecure(key);
    //     unsigned char isonly   = send_cookie.getHttponly(key);

    //     std::string samesite = send_cookie.getSamesite(key);

    //     temph.append(url_encode(iter.first.data(), iter.first.size()));
    //     temph.push_back('=');
    //     temph.append(url_encode(iter.second.data(), iter.second.size()));

    //     if (timeexp > 0 && timeexp < 63072000)
    //     {
    //         timeexp = timeid() + timeexp;
    //     }

    //     key.clear();
    //     if (timeexp > 0)
    //     {
    //         key = get_gmttime((unsigned long long)timeexp);
    //         temph.append("; Expires=");
    //         temph.append(key);
    //     }

    //     if (domain.size() > 1)
    //     {
    //         temph.append("; Domain=");
    //         temph.append(domain);
    //     }

    //     if (domainpath.size() > 0)
    //     {
    //         temph.append("; Path=");
    //         temph.append(domainpath);
    //     }

    //     if (issecure > 0)
    //     {
    //         temph.append("; Secure");
    //     }

    //     if (isonly > 0)
    //     {
    //         temph.append("; HttpOnly");
    //     }
    //     if (!samesite.empty())
    //     {
    //         switch (samesite[0])
    //         {
    //         case 'L':
    //         case 'l': temph.append("; SameSite=Lax"); break;
    //         case 'S':
    //         case 's': temph.append("; SameSite=Strict"); break;
    //         }
    //     }

    //     send_cookie_lists.emplace_back(temph);
    // }
    return send_cookie_lists;
}
void httppeer::scheme(unsigned char code) { isssl = (code == 1) ? 1 : 0; }
void httppeer::length(unsigned long long num) { content_length = num; }
std::string httppeer::make_http1_header()
{
    std::string http1header;
    http1header.append("HTTP/1.1 ");
    http1header.append(std::to_string(status_code));
    http1header.push_back(' ');
    http1header.append(http_status_static_table[status_code]);
    http1header.append("\r\n");

    if (ischunked)
    {
        http1header.append("Transfer-Encoding: chunked\r\n");
    }
    else
    {
        http1header.append("Content-Length: ");
        http1header.append(std::to_string(content_length));
        http1header.append("\r\n");
    }
    if (compress == 1)
    {
        http1header.append("Content-Encoding: ");
        http1header.append("gzip");
        http1header.append("\r\n");
        http1header.append("Vary: Accept-Encoding\r\n");
    }
    else if (compress == 2)
    {
        http1header.append("Content-Encoding: ");
        http1header.append("br");
        http1header.append("\r\n");
        http1header.append("Vary: Accept-Encoding\r\n");
    }
    if (content_type.size() > 0)
    {
        http1header.append("Content-Type: ");
        http1header.append(content_type);
        http1header.append("\r\n");
    }

    if (keepalive)
    {
        http1header.append("Connection: Keep-Alive\r\n");
    }
    else
    {
        http1header.append("Connection: close\r\n");
    }

    if (send_header.size() > 0)
    {
        for (auto [first, second] : send_header)
        {
            http1header.append(first);
            http1header.append(": ");
            http1header.append(second);
            http1header.append("\r\n");
        }
    }
    //std::list<std::string> cookielist = cookietoheader();

    if (send_cookie_lists.size() > 0)
    {
        for (auto iter = send_cookie_lists.begin(); iter != send_cookie_lists.end(); iter++)
        {
            http1header.append("Set-Cookie: ");
            http1header.append(*iter);
            http1header.append("\r\n");
        }
    }
    http1header.append("Server: Paozhu\r\n");
    return http1header;
}
std::string httppeer::make_http2_header(unsigned char flag_code)
{
    std::string http2header;

    http2header.resize(9);
    http2header[3]        = 0x01;
    unsigned int streamid = stream_id;
    http2header[8]        = streamid & 0xFF;
    streamid              = streamid >> 8;
    http2header[7]        = streamid & 0xFF;
    streamid              = streamid >> 8;
    http2header[6]        = streamid & 0xFF;
    streamid              = streamid >> 8;
    http2header[5]        = streamid & 0xFF;

    make_http2_headers_static(http2header, status_code);
    set_http2_headers_flag(http2header, HTTP2_HEADER_END_HEADERS | flag_code);
    make_http2_headers_item3(http2header, HTTP2_CODE_content_length, content_length);
    make_http2_headers_item3(http2header, HTTP2_CODE_content_type, content_type);

    if (compress == 1)
    {
        make_http2_headers_item3(http2header, HTTP2_CODE_content_encoding, "gzip");
    }
    else if (compress == 2)
    {
        make_http2_headers_item3(http2header, HTTP2_CODE_content_encoding, "br");
    }
    if (http2_send_header.size() > 0)
    {
        for (auto [first, second] : http2_send_header)
        {
            make_http2_headers_item3(http2header, (unsigned char)first, second);
        }
    }
    make_http2_headers_item3(http2header, "server", "Paozhu");
    if (send_header.size() > 0)
    {
        for (auto [first, second] : send_header)
        {
            make_http2_headers_item3(http2header, first, second);
        }
    }
    //std::list<std::string> cookielist = cookietoheader();
    if (send_cookie_lists.size() > 0)
    {
        for (auto iter = send_cookie_lists.begin(); iter != send_cookie_lists.end(); iter++)
        {
            make_http2_headers_item3(http2header, HTTP2_CODE_set_cookie, *iter);
            //cookielist.pop_front();
        }
    }

    set_http2_headers_size(http2header, (unsigned int)http2header.size() - 9);
    DEBUG_LOG("make_http2_header end");
    return http2header;
}
void httppeer::goto_url(const std::string &url, unsigned char second, const std::string &msg)
{
    output.append("<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"");
    output.append(std::to_string(second));
    output.append(";url=");
    output.append(url);
    output.append("\"></head><body>");
    if (msg.size() > 0)
    {
        output.append("<div style=\"margin-top:20px;margin-left: auto;margin-right: auto;with:60%\"><h3>");
        output.append(msg);
        output.append("</h3></div>");
    }
    output.append("</body></html>");
}

httppeer &httppeer::operator<<(obj_val &a)
{
    output.append(a.to_string());
    return *this;
}
httppeer &httppeer::operator<<(const std::string &a)
{
    output.append(a);
    return *this;
}
httppeer &httppeer::operator<<(std::string &&a)
{
    output.append(a);
    return *this;
}
httppeer &httppeer::operator<<(std::string &a)
{
    output.append(a);
    return *this;
}
httppeer &httppeer::operator<<(std::string_view a)
{
    output.append(a);
    return *this;
}
httppeer &httppeer::operator<<(char a)
{
    output.push_back(a);
    return *this;
}
httppeer &httppeer::operator<<(unsigned char a)
{
    output.push_back(a);
    return *this;
}
httppeer &httppeer::operator<<(const char *a)
{
    output.append(a);
    return *this;
}
httppeer &httppeer::operator<<(unsigned int a)
{
    output.append(std::to_string(a));
    return *this;
}
httppeer &httppeer::operator<<(int a)
{
    output.append(std::to_string(a));
    return *this;
}
httppeer &httppeer::operator<<(unsigned long long a)
{
    output.append(std::to_string(a));
    return *this;
}
httppeer &httppeer::operator<<(unsigned long a)
{
    output.append(std::to_string(a));
    return *this;
}
httppeer &httppeer::operator<<(long long a)
{
    output.append(std::to_string(a));
    return *this;
}
httppeer &httppeer::operator<<(float a)
{
    output.append(std::to_string(a));
    return *this;
}
httppeer &httppeer::operator<<(double a)
{
    output.append(std::to_string(a));
    return *this;
}
template <typename T>
httppeer &httppeer::operator<<(VALNUM_T auto a)
{
    output.append(std::to_string(a));
    return *this;
}

template <typename T>
httppeer &httppeer::operator<<(T a)
{
    output.append(a.to_string());
    return *this;
}
template <typename T>
httppeer &httppeer::operator<<(T *a)
{
    output.append(a->to_string());
    return *this;
}

httppeer &httppeer::get_peer() { return *this; }
httppeer &httppeer::out(std::string a)
{
    output.append(a);
    return *this;
}
httppeer &httppeer::out(const std::string &a)
{
    output.append(a);
    return *this;
}

void httppeer::view(const std::string &a)
{
    struct view_param tempvp(get, post, cookie, session);
    if (!isso)
    {
        try
        {
            VIEW_REG &viewreg = get_viewmetholdreg();
            auto viter        = viewreg.find(a);
            if (viter != viewreg.end())
            {
                output.append(viter->second(tempvp, val));
                return;
            }
            output.append("Not Found View ");
            output.append(a);
        }
        catch (const std::exception &e)
        {
            output.append(std::string(e.what()));
            return;
        }
    }
#ifdef ENABLE_BOOST
    serverconfig &sysconfigpath = getserversysconfig();
    if (sysconfigpath.map_value.find(host) != sysconfigpath.map_value.end())
    {
        if (sysconfigpath.map_value[host].find("viewsopath") != sysconfigpath.map_value[host].end())
        {
            tempvp.viewsopath = sysconfigpath.map_value[host]["viewsopath"];
        }
    }
    if (tempvp.viewsopath.empty())
    {
        tempvp.viewsopath = sysconfigpath.map_value["default"]["viewsopath"];
    }
    if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
    {
        tempvp.viewsopath.push_back('/');
    }

    std::string tempp = tempvp.viewsopath;
    tempp.append(a);
    try
    {
        if (isso)
        {
            if (clientapi::get().map_value.find(host) != clientapi::get().map_value.end())
            {
                if (clientapi::get().map_value[host].find("viewsopath") != clientapi::get().map_value[host].end())
                {
                    tempvp.viewsopath = clientapi::get().map_value[host]["viewsopath"];
                }
            }
            if (tempvp.viewsopath.empty())
            {
                tempvp.viewsopath = clientapi::get().map_value["default"]["viewsopath"];
            }
            if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
            {
                tempvp.viewsopath.push_back('/');
            }
            tempp = tempvp.viewsopath;
            tempp.append(a);
            output.append(clientapi::get().api_loadview(tempp)(tempvp, val));
        }
        else
        {
            output.append(loadviewso(tempp)(tempvp, val));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
#endif
    return;
}
void httppeer::view(const std::string &a, obj_val &b)
{
    struct view_param tempvp(get, post, cookie, session);
    if (!isso)
    {
        try
        {
            VIEW_REG &viewreg = get_viewmetholdreg();
            auto viter        = viewreg.find(a);
            if (viter != viewreg.end())
            {
                output.append(viter->second(tempvp, b));
                return;
            }
            output.append("Not Found View ");
            output.append(a);
        }
        catch (const std::exception &e)
        {
            output.append(std::string(e.what()));
            return;
        }
    }
#ifdef ENABLE_BOOST
    serverconfig &sysconfigpath = getserversysconfig();
    if (sysconfigpath.map_value.find(host) != sysconfigpath.map_value.end())
    {
        if (sysconfigpath.map_value[host].find("viewsopath") != sysconfigpath.map_value[host].end())
        {
            tempvp.viewsopath = sysconfigpath.map_value[host]["viewsopath"];
        }
    }
    if (tempvp.viewsopath.empty())
    {
        tempvp.viewsopath = sysconfigpath.map_value["default"]["viewsopath"];
    }
    if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
    {
        tempvp.viewsopath.push_back('/');
    }
    std::string tempp = tempvp.viewsopath;
    tempp.append(a);
    try
    {
        if (isso)
        {
            if (clientapi::get().map_value.find(host) != clientapi::get().map_value.end())
            {
                if (clientapi::get().map_value[host].find("viewsopath") != clientapi::get().map_value[host].end())
                {
                    tempvp.viewsopath = clientapi::get().map_value[host]["viewsopath"];
                }
            }
            if (tempvp.viewsopath.empty())
            {
                tempvp.viewsopath = clientapi::get().map_value["default"]["viewsopath"];
            }
            if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
            {
                tempvp.viewsopath.push_back('/');
            }
            tempp = tempvp.viewsopath;
            tempp.append(a);
            output.append(clientapi::get().api_loadview(tempp)(tempvp, b));
        }
        else
        {
            output.append(loadviewso(tempp)(tempvp, b));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
#endif
    return;
}
std::string httppeer::fetchview(const std::string &a)
{
    struct view_param tempvp(get, post, cookie, session);
    if (!isso)
    {
        try
        {
            VIEW_REG &viewreg = get_viewmetholdreg();
            auto viter        = viewreg.find(a);
            if (viter != viewreg.end())
            {
                return viter->second(tempvp, val);
            }
            output.append("Not Found View ");
            output.append(a);
        }
        catch (const std::exception &e)
        {
            output.append(std::string(e.what()));
            return "";
        }
    }
#ifdef ENABLE_BOOST
    serverconfig &sysconfigpath = getserversysconfig();
    if (sysconfigpath.map_value.find(host) != sysconfigpath.map_value.end())
    {
        if (sysconfigpath.map_value[host].find("viewsopath") != sysconfigpath.map_value[host].end())
        {
            tempvp.viewsopath = sysconfigpath.map_value[host]["viewsopath"];
        }
    }
    if (tempvp.viewsopath.empty())
    {
        tempvp.viewsopath = sysconfigpath.map_value["default"]["viewsopath"];
    }
    if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
    {
        tempvp.viewsopath.push_back('/');
    }
    std::string tempp = tempvp.viewsopath;
    tempp.append(a);
    try
    {

        if (isso)
        {
            if (clientapi::get().map_value.find(host) != clientapi::get().map_value.end())
            {
                if (clientapi::get().map_value[host].find("viewsopath") != clientapi::get().map_value[host].end())
                {
                    tempvp.viewsopath = clientapi::get().map_value[host]["viewsopath"];
                }
            }
            if (tempvp.viewsopath.empty())
            {
                tempvp.viewsopath = clientapi::get().map_value["default"]["viewsopath"];
            }
            if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
            {
                tempvp.viewsopath.push_back('/');
            }
            tempp = tempvp.viewsopath;
            tempp.append(a);
            return (clientapi::get().api_loadview(tempp)(tempvp, val));
        }
        else
        {

            return loadviewso(tempp)(tempvp, val);
        }
    }
    catch (const std::exception &e)
    {
        return "";
    }
#endif
    return "";
}
std::string httppeer::fetchview(const std::string &a, obj_val &b)
{
    struct view_param tempvp(get, post, cookie, session);
    if (!isso)
    {
        try
        {
            VIEW_REG &viewreg = get_viewmetholdreg();
            auto viter        = viewreg.find(a);
            if (viter != viewreg.end())
            {
                return viter->second(tempvp, b);
            }
            output.append("Not Found View ");
            output.append(a);
        }
        catch (const std::exception &e)
        {
            output.append(std::string(e.what()));
            return "";
        }
    }
#ifdef ENABLE_BOOST
    serverconfig &sysconfigpath = getserversysconfig();
    if (sysconfigpath.map_value.find(host) != sysconfigpath.map_value.end())
    {
        if (sysconfigpath.map_value[host].find("viewsopath") != sysconfigpath.map_value[host].end())
        {
            tempvp.viewsopath = sysconfigpath.map_value[host]["viewsopath"];
        }
    }
    if (tempvp.viewsopath.empty())
    {
        tempvp.viewsopath = sysconfigpath.map_value["default"]["viewsopath"];
    }
    if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
    {
        tempvp.viewsopath.push_back('/');
    }
    std::string tempp = tempvp.viewsopath;
    tempp.append(a);
    try
    {
        if (isso)
        {
            if (clientapi::get().map_value.find(host) != clientapi::get().map_value.end())
            {
                if (clientapi::get().map_value[host].find("viewsopath") != clientapi::get().map_value[host].end())
                {
                    tempvp.viewsopath = clientapi::get().map_value[host]["viewsopath"];
                }
            }
            if (tempvp.viewsopath.empty())
            {
                tempvp.viewsopath = clientapi::get().map_value["default"]["viewsopath"];
            }
            if (tempvp.viewsopath.size() > 0 && tempvp.viewsopath.back() != '/')
            {
                tempvp.viewsopath.push_back('/');
            }
            tempp = tempvp.viewsopath;
            tempp.append(a);
            return (clientapi::get().api_loadview(tempp)(tempvp, b));
        }
        else
        {
            return loadviewso(tempp)(tempvp, b);
        }
    }
    catch (const std::exception &e)
    {
        return "";
    }
#endif
    return "";
}
void httppeer::out_json(obj_val &a)
{
    content_type = "application/json";
    output       = a.to_json();
}
void httppeer::out_json()
{
    content_type = "application/json";
    if (output.size() < 1)
    {
        output = val.to_json();
    }
}
void httppeer::json_type() { content_type = "application/json"; }
void httppeer::cors_domain(const std::string &name, const std::string &header_v)
{
    if (httpv == 2)
    {
        if (http2_header_codes_table["access-control-allow-origin"] > 0)
        {
            http2_send_header[http2_header_codes_table["access-control-allow-origin"]] = name;
        }
        else
        {
            send_header["access-control-allow-origin"] = name;
        }
        if (header_v.size() > 0)
        {
            send_header["access-control-allow-headers"] = header_v;
        }
        else
        {
            send_header["access-control-allow-headers"] = "*";
        }
    }
    else
    {
        send_header["Access-Control-Allow-Origin"] = name;
        if (header_v.size() > 0)
        {
            send_header["Access-Control-Allow-Headers"] = header_v;
        }
        else
        {
            send_header["Access-Control-Allow-Headers"] = "*";
        }
    }
}
void httppeer::cors_method(const std::string &header_v)
{
    if (httpv == 2)
    {
        if (header_v.size() > 0)
        {
            send_header["access-control-expose-headers"] = header_v;
        }
        else
        {
            send_header["access-control-expose-headers"] = "*";
        }
        send_header["access-control-max-age"]       = "86400";
        send_header["access-control-allow-methods"] = "POST, GET, OPTIONS";
    }
    else
    {
        if (header_v.size() > 0)
        {
            send_header["Access-Control-Expose-Headers"] = header_v;
        }
        else
        {
            send_header["Access-Control-Expose-Headers"] = "*";
        }
        send_header["Access-Control-Max-Age"]       = "86400";
        send_header["Access-Control-Allow-Methods"] = "POST, GET, OPTIONS";
    }
}
void httppeer::push_flow(const std::string &m_name)
{
    if (!flow_method)
    {
        flow_method = std::make_unique<std::list<std::string>>();
    }
    flow_method->push_back(m_name);
}
void httppeer::push_front_flow(const std::string &m_name)
{
    if (!flow_method)
    {
        flow_method = std::make_unique<std::list<std::string>>();
    }
    flow_method->push_front(m_name);
}
std::string httppeer::pop_flow()
{
    if (!flow_method)
    {
        return "";
    }
    if (flow_method->empty())
    {
        return "";
    }
    std::string tempmethod = flow_method->back();
    flow_method->pop_back();
    return tempmethod;
}
void httppeer::clsoesend(asio::io_context &ioc)
{
    try
    {
        std::unique_lock<std::mutex> lock(pop_user_handleer_mutex);
        if (user_code_handler_call.size() > 0)
        {
            asio::dispatch(ioc,
                           [handler = std::move(user_code_handler_call.front())]() mutable -> void
                           {
                               /////////////
                               handler(0);
                               //////////
                           });
            user_code_handler_call.pop_front();
            DEBUG_LOG("httppeer user_code_handler_call return");
        }
    }
    catch (const std::exception &e)
    {
        DEBUG_LOG("httppeer user_code_handler_call error");
    }
}

}// namespace http
