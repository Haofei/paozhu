#include <cstdarg>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include "base64.h"
#include "http_socket.h"
#include "send_email.h"
#include "func.h"
#include "client_context.h"
namespace http
{

bool send_email::addattachments(std::string file_name)
{
    FILE *ffile = fopen(file_name.c_str(), "rb");
    if (!ffile)
    {
        return false;
    }
    fclose(ffile);
    attachments.push_back(file_name);
    return true;
}
bool send_email::send()
{
    if (isssl)
    {
        return sendssldata();
    }
    else
    {
        return senddata();
    }
}
bool send_email::sendssldata()
{
    try
    {
        // asio::io_context io_context;
        client_context &temp_io_context = get_client_context_obj();
        asio::ssl::context ssl_context(asio::ssl::context::sslv23);

        asio::ssl::stream<asio::ip::tcp::socket> socket(temp_io_context.get_ctx(), ssl_context);
        ssl_context.set_default_verify_paths();
        asio::ip::tcp::resolver resolver(temp_io_context.get_ctx());
        auto endpoints = resolver.resolve(smpturl.c_str(), std::to_string(port));

        SSL_set_tlsext_host_name(socket.native_handle(), smpturl.c_str());

        asio::connect(socket.lowest_layer(), endpoints,ec);

        if (ec)
        {
            errormsg.append("ssl connect error!");
            return false;
        }
        ssl_context.set_verify_mode(asio::ssl::verify_peer);
        ssl_context.set_verify_callback(asio::ssl::host_name_verification(smpturl));

        try
        {
            socket.handshake(asio::ssl::stream_base::client, ec);
        }
        catch (const std::exception &e)
        {
            errormsg.append(e.what());
        }

        if (ec)
        {
            errormsg.append("ssl handshake error!");
            return false;
        }

        state = SEND_START;
        unsigned int n;
        //  unsigned int ci_data=0;

        //     memset(data, 0x00, 2048);
        //     n = socket.read_some(asio::buffer(data, 1024), ec);
        //     sendcommand.clear();
        //     sendcommand.append("EHLO ");
        //     sendcommand.append(smpturl);
        //     sendcommand.append("\r\n");

        //     socket.write_some(asio::buffer(sendcommand));
        //     memset(data, 0x00, 2048);
        //     n = socket.read_some(asio::buffer(data, 1024), ec);

        //     sendcommand.clear();
        //     sendcommand.append("AUTH LOGIN\r\n");
        //     //sendcommand.append("AUTH CRAM-MD5\r\n");
        //     socket.write_some(asio::buffer(sendcommand));
        //     memset(data, 0x00, 2048);
        //     n = socket.read_some(asio::buffer(data, 1024), ec);

        //     // std::string backseret;
        //     // for(unsigned int i=4;i<n;i++)
        //     // {
        //     //     backseret.push_back(data[i]);
        //     // }

        //     // backseret=base64_decode((const char*)&backseret[0],backseret.size());

        //     // unsigned char sSHA[EVP_MAX_MD_SIZE] = {0};
        //     // unsigned int nSHALen = EVP_MAX_MD_SIZE;
        //     // unsigned char* ret = HMAC(EVP_md5(),(const char*)&password[0],password.size(), (const unsigned char*)&backseret[0], backseret.size(), sSHA, &nSHALen);

        //     // backseret=username+' '+char2str(&sSHA[0],nSHALen);
        //     // sendcommand=base64_encode((const char*)&backseret[0],backseret.size());
        //     //sendcommand.clear();
        //     sendcommand.clear();
        //     sendcommand.append(base64_encode(username.data(), username.size()));
        //     sendcommand.append("\r\n");

        //     socket.write_some(asio::buffer(sendcommand));
        //     memset(data, 0x00, 2048);
        //     n = socket.read_some(asio::buffer(data, 1024), ec);

        memset(data, 0x00, 2048);
        n = socket.read_some(asio::buffer(data, 1024), ec);

        if (state == SEND_START)
        {
            state = SEND_EHLO;
            while (state != SEND_END)
            {
                sendcommand.clear();
                switch (state)
                {
                case SEND_EHLO:
                    sendloop();
                    break;
                case SEND_AUTH_LOGIN:
                    sendloop();
                    break;
                case SEND_USER:
                    sendloop();
                    break;
                case SEND_PASSWORD:
                    sendloop();
                    break;
                case SEND_MAIL_FROM:
                    sendloop();
                    break;
                case SEND_RCPT_TO:
                    sendloop();
                    break;
                case SEND_DATA:
                    sendloop();
                    break;
                case SEND_EMAIL:
                    sendloop();
                    break;
                default:
                    break;
                }

                if (sendcommand.size() > 0)
                {

                    if (state == SEND_EMAIL)
                    {

                        if (sendfiles.size() == 0)
                        {
                            socket.write_some(asio::buffer(sendcommand));
                        }
                        else
                        {
                            asio::write(socket, asio::buffer(sendcommand));
                            unsigned int sendsize = 0;
                            data[2048]            = 0;
                            std::string filecontent;
                            std::string sendcontent;
                            for (unsigned int j = 0; j < sendfiles.size(); j++)
                            {
                                if (sendfiles[j].predata.empty())
                                {
                                    break;
                                }
                                socket.write_some(asio::buffer(sendfiles[j].predata));
                                FILE *ff = fopen(sendfiles[j].filename.c_str(), "rb");
                                if (ff)
                                {
                                    fseek(ff, 0, SEEK_END);
                                    unsigned int size = ftell(ff);
                                    fseek(ff, 0, SEEK_SET);
                                    sendsize = 0;
                                    filecontent.clear();
                                    filecontent.resize(size);
                                    unsigned int nread = fread(filecontent.data(), 1, size, ff);
                                    filecontent.resize(nread);
                                    filecontent = base64_encode(filecontent.data(), filecontent.size(), false);
                                    sendcontent.clear();
                                    for (; sendsize < filecontent.size(); sendsize++)
                                    {
                                        if (sendsize % 76 == 0 && sendsize > 0)
                                        {
                                            sendcontent.append("\r\n");
                                        }
                                        sendcontent.push_back(filecontent[sendsize]);
                                    }
                                    asio::write(socket, asio::buffer(sendcontent.data(), sendcontent.size()));
                                    // while(sendsize<size){
                                    //         memset(data,0x00,2048);
                                    //         unsigned int nread =fread(data, 1,2048, ff);

                                    //         asio::write(socket,asio::buffer(data,nread));
                                    //         //socket.write(asio::buffer(data,nread));
                                    //         sendsize+=nread;
                                    // }
                                }
                                fclose(ff);
                            }
                            sendcommand.clear();
                            sendcommand.append("\r\n--");
                            sendcommand.append(boundarystsr);
                            sendcommand.append("--\r\n");
                            sendcommand.append("\r\n.\r\n");
                            asio::write(socket, asio::buffer(sendcommand));
                        }
                    }
                    else
                    {
                        socket.write_some(asio::buffer(sendcommand));
                    }
                    memset(data, 0x00, 2048);
                    n = socket.read_some(asio::buffer(data, 1024), ec);
                }

                if (data[0] == '2' || data[0] == '3' || data[0] == '4' || data[0] == '5')
                {
                    unsigned int temprenum = 0;
                    if (n > 2)
                    {
                        for (unsigned int i = 0; i < 3; i++)
                        {
                            if (data[i] <= '9' && data[i] >= '0')
                            {
                                temprenum = temprenum * 10 + (data[i] - '0');
                            }
                        }
                    }
                    switch (state)
                    {
                    case SEND_EHLO:
                        state = SEND_AUTH_LOGIN;
                        break;
                    case SEND_AUTH_LOGIN:
                        if (temprenum == 334 || temprenum == 250)
                        {
                            state = SEND_USER;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_USER:
                        if (temprenum == 334 || temprenum == 250)
                        {
                            state = SEND_PASSWORD;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_PASSWORD:
                        if (temprenum == 334 || temprenum == 235 || temprenum == 250)
                        {
                            state = SEND_MAIL_FROM;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_MAIL_FROM:
                        if (temprenum == 250)
                        {
                            state = SEND_RCPT_TO;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_RCPT_TO:
                        if (temprenum == 250)
                        {
                            state = SEND_DATA;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_DATA:
                        if (temprenum == 354)
                        {
                            state = SEND_EMAIL;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_EMAIL:
                        if (temprenum == 250)
                        {
                            state = SEND_END;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    state = SEND_ERROR;
                    errormsg.clear();
                    for (unsigned int j = 0; j < 100; j++)
                    {
                        if (data[j] != 0x00)
                        {
                            errormsg.push_back(data[j]);
                        }
                    }
                }

                if (state == SEND_END || state == SEND_ERROR)
                {
                    try
                    {
                        sendcommand.clear();
                        sendcommand.append("QUIT");
                        sendcommand.append("\r\n");

                        socket.write_some(asio::buffer(sendcommand));
                        break;
                    }
                    catch (std::exception &e)
                    {
                        errormsg.append(e.what());
                        break;
                    }
                }
            }
        }
        if (state == SEND_END)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch (std::exception &e)
    {
        errormsg.append(e.what());
        return false;
    }
    return true;
}
bool send_email::senddata()
{

    try
    {
        //asio::io_context clientio_context(1);
        client_context &temp_io_context = get_client_context_obj();
        // asio::signal_set signals(temp_io_context, SIGINT, SIGTERM);
        // signals.async_wait([&](auto, auto)
        //                    { clientio_context.stop(); });

        asio::ip::tcp::socket socket(temp_io_context.get_ctx());
        asio::ip::tcp::resolver resolver(temp_io_context.get_ctx());
        auto endpoints = resolver.resolve(smpturl, std::to_string(port));
        asio::connect(socket,endpoints,ec);

        //asio::ip::tcp::resolver::query checkquery(smpturl, std::to_string(port));
        // asio::ip::tcp::resolver::iterator iter = resolver.resolve(checkquery);
        // asio::ip::tcp::resolver::iterator end;
        // asio::ip::tcp::endpoint endpoint;

        // while (iter != end)
        // {
        //     endpoint = *iter++;
        //     socket.connect(endpoint, ec);
        //     if (ec)
        //     {
        //         continue;
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }
        // asio::error_code ec;
        if (ec)
        {
            errormsg.append("host connect error!");
            return false;
        }
        unsigned int n;
        state = SEND_START;
        memset(data, 0x00, 2048);
        n = socket.read_some(asio::buffer(data, 1024), ec);

        if (state == SEND_START)
        {
            state = SEND_EHLO;
            while (state != SEND_END)
            {
                sendcommand.clear();
                switch (state)
                {
                case SEND_EHLO:
                    sendloop();
                    break;
                case SEND_AUTH_LOGIN:
                    sendloop();
                    break;
                case SEND_USER:
                    sendloop();
                    break;
                case SEND_PASSWORD:
                    sendloop();
                    break;
                case SEND_MAIL_FROM:
                    sendloop();
                    break;
                case SEND_RCPT_TO:
                    sendloop();
                    break;
                case SEND_DATA:
                    sendloop();
                    break;
                case SEND_EMAIL:
                    sendloop();
                    break;
                default:
                    break;
                }

                if (sendcommand.size() > 0)
                {

                    if (state == SEND_EMAIL)
                    {

                        if (sendfiles.size() == 0)
                        {
                            socket.write_some(asio::buffer(sendcommand));
                        }
                        else
                        {
                            asio::write(socket, asio::buffer(sendcommand));
                            unsigned int sendsize = 0;
                            data[2048]            = 0;
                            std::string filecontent;
                            std::string sendcontent;
                            for (unsigned int j = 0; j < sendfiles.size(); j++)
                            {
                                if (sendfiles[j].predata.empty())
                                {
                                    break;
                                }
                                socket.write_some(asio::buffer(sendfiles[j].predata));
                                FILE *ff = fopen(sendfiles[j].filename.c_str(), "rb");
                                if (ff)
                                {
                                    fseek(ff, 0, SEEK_END);
                                    unsigned int size = ftell(ff);
                                    fseek(ff, 0, SEEK_SET);
                                    sendsize = 0;
                                    filecontent.clear();
                                    filecontent.resize(size);
                                    unsigned int nread = fread(filecontent.data(), 1, size, ff);
                                    filecontent.resize(nread);
                                    filecontent = base64_encode(filecontent.data(), filecontent.size(), false);
                                    sendcontent.clear();
                                    for (; sendsize < filecontent.size(); sendsize++)
                                    {
                                        if (sendsize % 76 == 0 && sendsize > 0)
                                        {
                                            sendcontent.append("\r\n");
                                        }
                                        sendcontent.push_back(filecontent[sendsize]);
                                    }
                                    asio::write(socket, asio::buffer(sendcontent.data(), sendcontent.size()));
                                    // while(sendsize<size){
                                    //         memset(data,0x00,2048);
                                    //         unsigned int nread =fread(data, 1,2048, ff);

                                    //         asio::write(socket,asio::buffer(data,nread));
                                    //         //socket.write(asio::buffer(data,nread));
                                    //         sendsize+=nread;
                                    // }
                                }
                                fclose(ff);
                            }
                            sendcommand.clear();
                            sendcommand.append("\r\n--");
                            sendcommand.append(boundarystsr);
                            sendcommand.append("--\r\n");
                            sendcommand.append("\r\n.\r\n");
                            asio::write(socket, asio::buffer(sendcommand));
                        }
                    }
                    else
                    {
                        socket.write_some(asio::buffer(sendcommand));
                    }
                    memset(data, 0x00, 2048);
                    n = socket.read_some(asio::buffer(data, 1024), ec);
                }

                if (data[0] == '2' || data[0] == '3' || data[0] == '4' || data[0] == '5')
                {
                    unsigned int temprenum = 0;
                    if (n > 2)
                    {
                        for (unsigned int i = 0; i < 3; i++)
                        {
                            if (data[i] <= '9' && data[i] >= '0')
                            {
                                temprenum = temprenum * 10 + (data[i] - '0');
                            }
                        }
                    }

                    switch (state)
                    {
                    case SEND_EHLO:
                        state = SEND_AUTH_LOGIN;
                        break;
                    case SEND_AUTH_LOGIN:
                        if (temprenum == 334 || temprenum == 235 || temprenum == 250)
                        {
                            state = SEND_USER;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_USER:
                        if (temprenum == 334 || temprenum == 235 || temprenum == 250)
                        {
                            state = SEND_PASSWORD;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_PASSWORD:
                        if (temprenum == 334 || temprenum == 235 || temprenum == 250)
                        {
                            state = SEND_MAIL_FROM;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_MAIL_FROM:
                        if (temprenum == 250)
                        {
                            state = SEND_RCPT_TO;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_RCPT_TO:
                        if (temprenum == 250)
                        {
                            state = SEND_DATA;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_DATA:
                        if (temprenum == 354)
                        {
                            state = SEND_EMAIL;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    case SEND_EMAIL:
                        if (temprenum == 250)
                        {
                            state = SEND_END;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < n; i++)
                            {
                                errormsg.push_back(data[n]);
                            }
                            return false;
                        }
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    state = SEND_ERROR;
                    errormsg.clear();
                    for (unsigned int j = 0; j < 100; j++)
                    {
                        if (data[j] != 0x00)
                        {
                            errormsg.push_back(data[j]);
                        }
                    }
                }

                if (state == SEND_END || state == SEND_ERROR)
                {
                    try
                    {
                        sendcommand.clear();
                        sendcommand.append("QUIT");
                        sendcommand.append("\r\n");

                        socket.write_some(asio::buffer(sendcommand));
                        break;
                    }
                    catch (std::exception &e)
                    {
                        errormsg.append(e.what());
                        break;
                    }
                }
            }
        }
        if (state == SEND_END)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch (std::exception &e)
    {
        errormsg.append(e.what());
        return false;
    }
}
std::string send_email::getgmttime()
{
    time_t curr_time;
    curr_time = time((time_t *)NULL);
    tm *timeInfo;
    char timestr[30] = {'\0'};
    timeInfo         = gmtime(&curr_time);
    strftime(timestr, sizeof(timestr), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);
    std::string temp(timestr);
    return temp;
}
bool send_email::sendloop()
{
    unsigned int n;
    switch (state)
    {
    case SEND_EHLO:
        sendcommand.append("HELO ");
        sendcommand.append(smpturl);
        sendcommand.append("\r\n");

        break;
    case SEND_AUTH_LOGIN:
        sendcommand.append("AUTH LOGIN\r\n");

        break;
    case SEND_USER:
        sendcommand.clear();
        sendcommand.append(base64_encode(username.data(), username.size()));
        sendcommand.append("\r\n");

        break;
    case SEND_PASSWORD:
        sendcommand.clear();
        sendcommand.append(base64_encode(password.data(), password.size()));
        sendcommand.append("\r\n");

        break;
    case SEND_MAIL_FROM:
        sendcommand.clear();
        sendcommand.append("MAIL FROM:<");
        sendcommand.append(username);
        sendcommand.append(">\r\n");

        break;
    case SEND_RCPT_TO:
        sendcommand.append("RCPT TO:<");
        sendcommand.append(toemail);
        sendcommand.append(">\r\n");

        break;
    case SEND_DATA:
        sendcommand.append("DATA\r\n");
        break;
    case SEND_EMAIL:

        if (attachments.size() > 0)
        {
            // attachments_content();

            sendcommand.clear();
            sendcommand.append("MIME-Version: 1.0\r\n");
            sendcommand.append("From: =?UTF-8?B?");
            sendcommand.append(base64_encode(fromname.data(), fromname.size(), 0));
            sendcommand.append("?= <");
            sendcommand.append(username);
            sendcommand.append(">\r\n");

            sendcommand.append("To: =?UTF-8?B?");
            sendcommand.append(base64_encode(toname.data(), toname.size(), 0));
            sendcommand.append("?= <");
            sendcommand.append(toemail);
            sendcommand.append(">\r\n");

            sendcommand.append("Date: ");
            sendcommand.append(getgmttime());
            sendcommand.append("\r\n");

            sendcommand.append("Subject: ");
            sendcommand.append(subject);
            sendcommand.append("\r\n");

            sendcommand.append("Reply-To: <");
            sendcommand.append(replyemail);
            sendcommand.append(">\r\n");

            srand(time(0));
            // std::string boundarystsr;
            boundarystsr.clear();
            std::stringstream ostr;
            unsigned int c = rand() * 10000000;
            ostr << c;
            c = rand() * 10000000;
            ostr << c;
            boundarystsr.append(ostr.str());

            sendcommand.append("Message-ID: <");
            sendcommand.append(boundarystsr);
            sendcommand.append(replyemail);
            sendcommand.append(">\r\n");

            boundarystsr.append("_paozhu_");

            sendcommand.append("Content-Type: multipart/mixed; boundary=\"");
            sendcommand.append(boundarystsr);
            sendcommand.append("\"\r\n\r\n--");
            sendcommand.append(boundarystsr);
            sendcommand.append("\r\n");
            sendcommand.append("Content-Type: text/html; charset=UTF-8\r\n\r\n");
            sendcommand.append(content);

            // std::string buffer;
            sendcommand.append("\r\n");
            n = 0;
            sendfiles.resize(attachments.size());
            for (; n < attachments.size(); n++)
            {

                FILE *ffile = fopen(attachments[n].c_str(), "rb");
                if (!ffile)
                {
                    n -= 1;
                    continue;
                }
                fclose(ffile);
                unsigned int k = 0;
                std::string filename_temp;
                for (k = attachments[n].size(); k > 0; k--)
                {
                    if (attachments[n][k] == '/' || attachments[n][k] == '\\')
                    {
                        if (k < attachments[n].size())
                        {
                            k += 1;
                        }
                        break;
                    }
                }
                filename_temp.append(attachments[n].substr(k));
                sendfiles[n].predata.append("\r\n--");
                sendfiles[n].predata.append(boundarystsr);
                // sendfiles[n].predata.append("\r\nContent-Type: application/octet-stream;\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=\"=?UTF-8?B?");
                // sendcommand.append(attachments[n]);
                sendfiles[n].predata.append("\r\nContent-Type: application/octet-stream;\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=\"=?UTF-8?B?");
                sendfiles[n].predata.append(base64_encode(filename_temp.data(), filename_temp.size()));
                sendfiles[n].predata.append("?=\"\r\n\r\n");
                // sendcommand.append("?=\"\r\nContent-Transfer-Encoding: base64\r\n\r\n");

                sendfiles[n].filename = attachments[n];
            }
            sendfiles.resize(n);
        }
        else
        {

            sendcommand.clear();
            sendcommand.append("MIME-Version: 1.0\r\n");
            sendcommand.append("Content-Type: text/html; charset=utf-8\r\n");
            sendcommand.append("From: =?UTF-8?B?");
            sendcommand.append(base64_encode(fromname.data(), fromname.size(), 0));
            sendcommand.append("?= <");
            sendcommand.append(username);
            sendcommand.append(">\r\n");

            sendcommand.append("To: =?UTF-8?B?");
            sendcommand.append(base64_encode(toname.data(), toname.size(), 0));
            sendcommand.append("?= <");
            sendcommand.append(toemail);
            sendcommand.append(">\r\n");

            sendcommand.append("Date: ");
            sendcommand.append(getgmttime());
            sendcommand.append("\r\n");

            sendcommand.append("Subject: ");
            sendcommand.append(subject);
            sendcommand.append("\r\n");

            sendcommand.append("Reply-To: <");
            sendcommand.append(replyemail);
            sendcommand.append(">\r\n");

            srand(time(0));
            // std::string boundarystsr;
            boundarystsr.clear();
            std::stringstream ostr;
            unsigned int c = rand() * 10000000;
            ostr << c;

            sendcommand.append("Message-ID: <");
            sendcommand.append(ostr.str());
            sendcommand.append(replyemail);
            sendcommand.append(">\r\n\r\n");

            sendcommand.append(content);
            sendcommand.append("\r\n\r\n.\r\n");
        }

        break;
    default:
        break;
    }
    return true;
}

}// namespace http
