#ifndef ORM_CMS_SYSPERMS_H
#define ORM_CMS_SYSPERMS_H
#include "sysperms_mysql.h" 
#include "cms/include/sysperms_base.h"

/* 如果此文件存在不会自动覆盖，没有则会自动生成。
*If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

 namespace orm {
	namespace cms { 
		class Sysperms : public sysperms_mysql<Sysperms,sysperms_base>{
		 public:
		 Sysperms(std::string dbtag);
		 Sysperms();
		};
	 }
 }
#endif
