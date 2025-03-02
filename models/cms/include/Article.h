#ifndef ORM_CMS_ARTICLE_H
#define ORM_CMS_ARTICLE_H
#include "article_mysql.h" 
#include "cms/include/articlebase.h"

/* 如果此文件存在不会自动覆盖，没有则会自动生成。
*If this file exists, it will not be overwritten automatically. If not, it will be generated automatically. */

 namespace orm {
	namespace cms { 
		class Article : public article_mysql<Article,articlebase>{
		 public:
		 Article(std::string dbtag);
		 Article();
		};
	 }
 }
#endif
