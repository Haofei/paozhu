#ifndef ORM_CMS_LOGININFO_H
#define ORM_CMS_LOGININFO_H
#include "logininfo_mysql.h" 
#include "cms/include/logininfo_base.h"

/* 如果此文件存在不会自动覆盖，没有则会自动生成。
*If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

 namespace orm {
	namespace cms { 
		class Logininfo : public logininfo_mysql<Logininfo,logininfo_base>{
		 public:
		 Logininfo(std::string dbtag);
		 Logininfo();
		};
	 }
 }
#endif
