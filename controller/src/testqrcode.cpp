#include <chrono>
#include <thread>
#include "httppeer.h"

#ifdef ENABLE_GD
#include "qrcode.h"
#endif

#include "testqrcode.h"
namespace http
{
//@urlpath(null,testqrcode)
std::string testqrcode(std::shared_ptr<httppeer> peer)
{
    httppeer &client = peer->get_peer();
    std::string atxt = client.post["qrcontent"].to_string();
    if (atxt.size() == 0)
    {
        atxt = "qrsting hello world !";
    }
    client << "<p>hello world!</p>";

#ifdef ENABLE_GD

    qrcode a(atxt, 5);
    std::string wwwpath;
    wwwpath.append(client.get_sitepath());
    wwwpath.append("/upload");

    fs::path paths  = wwwpath;
    bool is_success = fs::create_directories(paths);
    if (is_success)
    {
        fs::permissions(paths,
                        fs::perms::owner_all | fs::perms::group_all | fs::perms::others_read,
                        fs::perm_options::add);
    }
    wwwpath.append("/test.png");
    // std::string wwwurl = client.get_hosturl();
    client << "<img src=\"" << a.save("/upload/", wwwpath) << "?token=123456\">";

#endif
    client << "<form method=\"post\" action=\"/testqrcode\"><p><textarea name=\"qrcontent\"></textarea></p><p><input type=\"submit\" name=\"submit2\" value=\"Submit\"></p></form>";
    return "";
}

}// namespace http