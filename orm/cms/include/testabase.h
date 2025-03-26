#ifndef ORM_CMS_TESTABASEMATA_H
#define ORM_CMS_TESTABASEMATA_H
/*
*This file is auto create from cli
*本文件为自动生成 Wed, 26 Mar 2025 11:47:14 GMT
***/
#include <iostream>
#include <cstdio>
#include <sstream>
#include <array>
#include <map> 
#include <string_view> 
#include <string> 
#include <vector>
#include <ctime>
#include <array>
#include "unicode.h"

namespace orm { 
   
     namespace cms { 

struct testabase
{
    struct meta{
     unsigned  int  id = 0; ///**/
 unsigned  int  parentid = 0; ///*父id[id tree]*/
 char  value_id = 0; ///**/
 std::string  content = ""; ///**/
 unsigned  char  deleted = 0; ///**/
 unsigned  int  deletetime = 0; ///**/
 } data;
  
        struct meta_tree{
         unsigned  int  id = 0; ///**/
 unsigned  int  parentid = 0; ///*父id[id tree]*/
 char  value_id = 0; ///**/
 std::string  content = ""; ///**/
 unsigned  char  deleted = 0; ///**/
 unsigned  int  deletetime = 0; ///**/

	std::vector<meta_tree> children;
 };
 std::vector<testabase::meta> record;
std::string _rmstag="cms";//this value must be default or tag value, tag in mysqlconnect config file .
unsigned int _offset=0;
std::vector<testabase::meta>::iterator begin(){     return record.begin(); }
std::vector<testabase::meta>::iterator end(){     return record.end(); }
std::vector<testabase::meta>::const_iterator begin() const{     return record.begin(); }
std::vector<testabase::meta>::const_iterator end() const{     return record.end(); }
static constexpr std::array<std::string_view,6> col_names={"id","parentid","value_id","content","deleted","deletetime"};
static constexpr std::array<unsigned char,6> col_types={3,3,1,253,1,3};
static constexpr std::array<unsigned char,6> col_length={0,0,0,200,0,0};
static constexpr std::array<unsigned char,6> col_decimals={0,0,0,0,0,0};
std::string tablename="testa";
static constexpr std::string_view modelname="Testa";

	  unsigned char findcolpos(const std::string &coln){
            if(coln.size()==0)
            {
                return 255;
            }
		    unsigned char  bi=coln[0];
         

	         if(bi<91&&bi>64){
				bi+=32;
			}
            switch(coln[0]){


         case 'c':
   	 return 3;
break;
case 'd':
 switch(coln.size()){  
case 7:
   	 return 4;
break;
case 10:
   	 return 5;
break;
 }
 break;
case 'i':
   	 return 0;
break;
case 'p':
   	 return 1;
break;
case 'v':
   	 return 2;
break;

             }
             return 255;
           }
         
    int size(){ return record.size(); }   

    std::string getPKname(){ 
       return "id";
}

      void record_reset()
      {
            record.clear();     
      }
      void data_reset(){
     testabase::meta metatemp;    
            data = metatemp; 
      }
      
      std::string soft_remove_sql([[maybe_unused]] const std::string &fieldsql){
          std::string temp;
     
         if(fieldsql.size()<2)
         {
            temp="UPDATE `";
            temp.append(tablename);
            temp.push_back('`');
            temp.append(" SET ");
         }
         else
         {
            temp=_makeupdatesql(fieldsql);
            if(temp.size()>2)
            {
                if(temp.back()==0x20&&temp[temp.size()-2]=='T')
                {

                }
                else
                {
                    temp.push_back(',');
                }
            }
            
         }   
         
      	temp.push_back('`');
		temp.append("deleted");
		temp.push_back('`');
		temp.append("=1 ");
		 if(fieldsql.size()>0){ data.deleted=1; }
		temp.push_back(',');
		temp.push_back('`');
		temp.append("deletetime");
		temp.push_back('`');
		temp.append("=");
		unsigned int t=time((time_t *)NULL);
		temp.append(std::to_string(t));
		if(fieldsql.size()>0){ data.deletetime=t; }
	
         return temp;
     }
     

  inline  std::string stringaddslash(const std::string &content){
        std::string temp;
        for(unsigned int i=0;i<content.size();i++){
            if(content[i]=='\''){
                temp.append("\\'");
                continue;
            }else if(content[i]=='"'){
                temp.append("\\\"");
                continue;
            }else if(content[i]=='\\'){
                temp.append("\\\\");
                continue;
            }
            temp.push_back(content[i]);
        }
        return temp;
   }  
  inline  std::string jsonaddslash(const std::string &content){
        std::string temp;
        for(unsigned int i=0;i<content.size();i++){
            if(content[i]=='"'){
                temp.append("\\\"");
                continue;
            }
            else if(content[i]=='\\'){
                temp.append("\\\\");
                continue;
            }
            temp.push_back(content[i]);
        }
        return temp;
   }  

   std::string _makeinsertsql(){
        unsigned int j=0;
        std::ostringstream tempsql;
        tempsql<<"INSERT INTO ";
        tempsql<<tablename;
        tempsql<<" (";
        for(;j<col_names.size();j++){
                if(j>0){
                    tempsql<<"`,`";
                }else{
                    tempsql<<"`";
                }
                tempsql<<col_names[j];
        }
        if(j>0){
            tempsql<<"`";
        }
        tempsql<<") VALUES (";

        if(data.id==0){
tempsql<<"null";
 }else{ 
	tempsql<<std::to_string(data.id);
}
if(data.parentid==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(data.parentid);
}
if(data.value_id==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(data.value_id);
}
tempsql<<",'"<<stringaddslash(data.content)<<"'";
if(data.deleted==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(data.deleted);
}
if(data.deletetime==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(data.deletetime);
}
tempsql<<")";

     
       return tempsql.str();
   } 
      
      std::string _makerecordinsertsql(const meta &insert_data){
        unsigned int j=0;
        std::ostringstream tempsql;
        tempsql<<"INSERT INTO ";
        tempsql<<tablename;
        tempsql<<" (";
        for(;j<col_names.size();j++){
                if(j>0){
                    tempsql<<"`,`";
                }else{
                    tempsql<<"`";
                }
                tempsql<<col_names[j];
        }
        if(j>0){
            tempsql<<"`";
        }
        tempsql<<") VALUES (";

        if(insert_data.id==0){
tempsql<<"null";
 }else{ 
	tempsql<<std::to_string(insert_data.id);
}
if(insert_data.parentid==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(insert_data.parentid);
}
if(insert_data.value_id==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(insert_data.value_id);
}
tempsql<<",'"<<stringaddslash(insert_data.content)<<"'";
if(insert_data.deleted==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(insert_data.deleted);
}
if(insert_data.deletetime==0){
	tempsql<<",0";
 }else{ 
	tempsql<<","<<std::to_string(insert_data.deletetime);
}
tempsql<<")";

     
       return tempsql.str();
   } 
       
    std::string _makerecordinsertsql(const std::vector<meta> &insert_data){
        unsigned int j=0;
        std::ostringstream tempsql;
        tempsql<<"INSERT INTO ";
        tempsql<<tablename;
        tempsql<<" (";
        for(;j<col_names.size();j++){
                if(j>0){
                    tempsql<<"`,`";
                }else{
                    tempsql<<"`";
                }
                tempsql<<col_names[j];
        }
        if(j>0){
            tempsql<<"`";
        }
        tempsql<<") VALUES ";

        for(unsigned int i=0;i<insert_data.size();i++)
        {
            if(i>0)
            {
                tempsql<<",";	
            }
            tempsql<<"(";


            	if(insert_data[i].id==0){
	tempsql<<"null";
	 }else{ 
	tempsql<<std::to_string(insert_data[i].id);
	}
	if(insert_data[i].parentid==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(insert_data[i].parentid);
	}
	if(insert_data[i].value_id==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(insert_data[i].value_id);
	}
		tempsql<<",'"<<stringaddslash(insert_data[i].content)<<"'";
	if(insert_data[i].deleted==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(insert_data[i].deleted);
	}
	if(insert_data[i].deletetime==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(insert_data[i].deletetime);
	}
		tempsql<<")";
	 } 

     
       return tempsql.str();
   } 
       
    std::string _makeupdatesql(const std::string &fileld){
        std::ostringstream tempsql;
        tempsql<<"UPDATE ";
        tempsql<<tablename;
        tempsql<<" SET ";

        bool isall=false;
        if(fileld.empty()){
            isall=true;
        }
        if(isall){

        if(data.id==0){
	tempsql<<"`id`=0";
 }else{ 
	tempsql<<"`id`="<<std::to_string(data.id);
}
if(data.parentid==0){
	tempsql<<",`parentid`=0";
 }else{ 
	tempsql<<",`parentid`="<<std::to_string(data.parentid);
}
if(data.value_id==0){
	tempsql<<",`value_id`=0";
 }else{ 
	tempsql<<",`value_id`="<<std::to_string(data.value_id);
}
tempsql<<",`content`='"<<stringaddslash(data.content)<<"'";
if(data.deleted==0){
	tempsql<<",`deleted`=0";
 }else{ 
	tempsql<<",`deleted`="<<std::to_string(data.deleted);
}
if(data.deletetime==0){
	tempsql<<",`deletetime`=0";
 }else{ 
	tempsql<<",`deletetime`="<<std::to_string(data.deletetime);
}
 }else{ 

     
  unsigned int jj=0;
                  std::string keyname;
                  std::vector<unsigned char> keypos;
                  for(;jj<fileld.size();jj++){
                        if(fileld[jj]==','){
                                unsigned char bpos_i=findcolpos(keyname);
                               keypos.emplace_back(bpos_i); 
#ifdef DEBUG
                    if (bpos_i == 255)
                    {
                        std::cout << "\033[1m\033[31m-----------\n"
                                  << keyname << " not in " << tablename << " table Field.\n-----------\033[0m"
                                  << std::endl;
                    }
#endif                               
                               keyname.clear();
                             continue;   
                        }
                        if(fileld[jj]==0x20){

                             continue;   
                        }
                        keyname.push_back(fileld[jj]);

                  }  
                 if(keyname.size()>0){
                                unsigned char bpos_i=findcolpos(keyname);
 #ifdef DEBUG
                    if (bpos_i == 255)
                    {
                        std::cout << "\033[1m\033[31m-----------\n"
                                  << keyname << " not in " << tablename << " table Field.\n-----------\033[0m"
                                  << std::endl;
                    }
#endif                                       
                                keypos.emplace_back(bpos_i); 
                                keyname.clear();
                 }
                 for(jj=0;jj<keypos.size();jj++){
                       switch(keypos[jj]){

         case 0:
 if(jj>0){ tempsql<<","; } 
if(data.id==0){
	tempsql<<"`id`=0";
 }else{ 
	tempsql<<"`id`="<<std::to_string(data.id);
}
 break;
 case 1:
 if(jj>0){ tempsql<<","; } 
if(data.parentid==0){
	tempsql<<"`parentid`=0";
 }else{ 
	tempsql<<"`parentid`="<<std::to_string(data.parentid);
}
 break;
 case 2:
 if(jj>0){ tempsql<<","; } 
if(data.value_id==0){
	tempsql<<"`value_id`=0";
 }else{ 
	tempsql<<"`value_id`="<<std::to_string(data.value_id);
}
 break;
 case 3:
 if(jj>0){ tempsql<<","; } 
tempsql<<"`content`='"<<stringaddslash(data.content)<<"'";
 break;
 case 4:
 if(jj>0){ tempsql<<","; } 
if(data.deleted==0){
	tempsql<<"`deleted`=0";
 }else{ 
	tempsql<<"`deleted`="<<std::to_string(data.deleted);
}
 break;
 case 5:
 if(jj>0){ tempsql<<","; } 
if(data.deletetime==0){
	tempsql<<"`deletetime`=0";
 }else{ 
	tempsql<<"`deletetime`="<<std::to_string(data.deletetime);
}
 break;

     
                  default:
                                ;
                     }
                 }   

            }        

        return tempsql.str();
   } 
   
    std::string _make_replace_into_sql()
    {
        unsigned int j = 0;
        std::ostringstream tempsql;
        tempsql << "REPLACE INTO ";
        tempsql << tablename;
        tempsql << " (";
        for (; j < col_names.size(); j++)
        {
            if (j > 0)
            {
                tempsql << "`,`";
            }
            else
            {
                tempsql << "`";
            }
            tempsql << col_names[j];
        }
        if (j > 0)
        {
            tempsql << "`";
        }
        tempsql << ") VALUES ";

        for (unsigned int i = 0; i < record.size(); i++)
        {
            if (i > 0)
            {
                tempsql << ",\n";
            }
            tempsql << "(";
            	if(record[i].id==0){
	tempsql<<"null";
	 }else{ 
	tempsql<<std::to_string(record[i].id);
	}
	if(record[i].parentid==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].parentid);
	}
	if(record[i].value_id==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].value_id);
	}
	tempsql<<",'"<<stringaddslash(record[i].content)<<"'";
	if(record[i].deleted==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].deleted);
	}
	if(record[i].deletetime==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].deletetime);
	}
	tempsql<<")";

 }
 return tempsql.str();
}

    std::string _make_insert_into_sql(const std::string &fileld)
    {
        unsigned int j = 0;
        std::ostringstream tempsql;
        tempsql << "INSERT INTO ";
        tempsql << tablename;
        tempsql << " (";
        for (; j < col_names.size(); j++)
        {
            if (j > 0)
            {
                tempsql << "`,`";
            }
            else
            {
                tempsql << "`";
            }
            tempsql << col_names[j];
        }
        if (j > 0)
        {
            tempsql << "`";
        }
        tempsql << ") VALUES ";

        for (unsigned int i = 0; i < record.size(); i++)
        {
            if (i > 0)
            {
                tempsql << ",\n";
            }
            tempsql << "(";
            	if(record[i].id==0){
	tempsql<<"null";
	 }else{ 
	tempsql<<std::to_string(record[i].id);
	}
	if(record[i].parentid==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].parentid);
	}
	if(record[i].value_id==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].value_id);
	}
	tempsql<<",'"<<stringaddslash(record[i].content)<<"'";
	if(record[i].deleted==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].deleted);
	}
	if(record[i].deletetime==0){
	tempsql<<",0";
	 }else{ 
	tempsql<<","<<std::to_string(record[i].deletetime);
	}
	tempsql<<")";
	 }
	 tempsql<<" as new ON DUPLICATE KEY UPDATE ";

     
        std::string keyname;
        unsigned char jj=0;
        j=0;
        if(fileld.size()>0){
        for(;jj<fileld.size();jj++){
            if(fileld[jj]==','){
                if(findcolpos(keyname)<255)
                {
                    if(j>0)
                    {
                        tempsql<<",";
                    }
                    tempsql<<keyname;
                    tempsql<<"=new.";
                    tempsql<<keyname;
                }
                continue;   
            }
            if(fileld[jj]==0x20){
                continue;   
            }
            keyname.push_back(fileld[jj]);

        }  
        if(keyname.size()>0){
            if(findcolpos(keyname)<255)
            {
                if(j>0)
                {
                    tempsql<<",";
                }
                tempsql<<keyname;
                tempsql<<"=new.";
                tempsql<<keyname;
                
            }
        }

    } 
 
 return tempsql.str();
}

   std::vector<std::string> data_toarray(std::string fileld=""){
        std::vector<std::string> temparray;
        std::string keyname;
        unsigned char jj=0;
        std::vector<unsigned char> keypos;
        if(fileld.size()>1){
            for(;jj<fileld.size();jj++){
                if(fileld[jj]==','){
                    keypos.emplace_back(findcolpos(keyname)); 
                    keyname.clear();
                    continue;   
                }
                if(fileld[jj]==0x20){

                    continue;   
                }
                keyname.push_back(fileld[jj]);

            }  
            if(keyname.size()>0){
                keypos.emplace_back(findcolpos(keyname)); 
                keyname.clear();
            }
        }else{
            for(jj=0;jj<col_names.size();jj++){
                keypos.emplace_back(jj); 
            }
        }
               
            for(jj=0;jj<keypos.size();jj++){
                switch(keypos[jj]){
         case 0:
if(data.id==0){
	temparray.push_back("0");
 }else{ 
	temparray.push_back(std::to_string(data.id));
}
 break;
 case 1:
if(data.parentid==0){
	temparray.push_back("0");
 }else{ 
	temparray.push_back(std::to_string(data.parentid));
}
 break;
 case 2:
if(data.value_id==0){
	temparray.push_back("0");
 }else{ 
	temparray.push_back(std::to_string(data.value_id));
}
 break;
 case 3:
	temparray.push_back(data.content);
 break;
 case 4:
if(data.deleted==0){
	temparray.push_back("0");
 }else{ 
	temparray.push_back(std::to_string(data.deleted));
}
 break;
 case 5:
if(data.deletetime==0){
	temparray.push_back("0");
 }else{ 
	temparray.push_back(std::to_string(data.deletetime));
}
 break;

                             default:
                                ;
                     }
                 }   
   
     return temparray;             
   }   
   
   std::map<std::string,std::string> data_tomap(std::string fileld=""){
       std::map<std::string,std::string> tempsql;
        std::string keyname;
        unsigned char jj=0;
        std::vector<unsigned char> keypos;
        if(fileld.size()>1){
        for(;jj<fileld.size();jj++){
            if(fileld[jj]==','){
                keypos.emplace_back(findcolpos(keyname)); 
                keyname.clear();
                continue;   
            }
            if(fileld[jj]==0x20){

                continue;   
            }
            keyname.push_back(fileld[jj]);

        }  
        if(keyname.size()>0){
            keypos.emplace_back(findcolpos(keyname)); 
            keyname.clear();
        }
        }else{
            for(jj=0;jj<col_names.size();jj++){
                keypos.emplace_back(jj); 
            }
        }
    
        for(jj=0;jj<keypos.size();jj++){
            switch(keypos[jj]){
         case 0:
if(data.id==0){
	tempsql.insert({"id","0"});
 }else{ 
	tempsql.insert({"id",std::to_string(data.id)});
}
 break;
 case 1:
if(data.parentid==0){
	tempsql.insert({"parentid","0"});
 }else{ 
	tempsql.insert({"parentid",std::to_string(data.parentid)});
}
 break;
 case 2:
if(data.value_id==0){
	tempsql.insert({"value_id","0"});
 }else{ 
	tempsql.insert({"value_id",std::to_string(data.value_id)});
}
 break;
 case 3:
	tempsql.insert({"content",data.content});
 break;
 case 4:
if(data.deleted==0){
	tempsql.insert({"deleted","0"});
 }else{ 
	tempsql.insert({"deleted",std::to_string(data.deleted)});
}
 break;
 case 5:
if(data.deletetime==0){
	tempsql.insert({"deletetime","0"});
 }else{ 
	tempsql.insert({"deletetime",std::to_string(data.deletetime)});
}
 break;

                             default:
                                ;
                     }
                 }   
    
     return tempsql;             
   }   
   
   std::string data_tojson(){
       std::ostringstream tempsql;

        tempsql<<"{";
if(data.id==0){
	tempsql<<"\"id\":0";
 }else{ 
	tempsql<<"\"id\":"<<std::to_string(data.id);
}
if(data.parentid==0){
	tempsql<<",\"parentid\":0";
 }else{ 
	tempsql<<",\"parentid\":"<<std::to_string(data.parentid);
}
if(data.value_id==0){
	tempsql<<",\"value_id\":0";
 }else{ 
	tempsql<<",\"value_id\":"<<std::to_string(data.value_id);
}
tempsql<<",\"content\":\""<<http::utf8_to_jsonstring(data.content);
tempsql<<"\"";
if(data.deleted==0){
	tempsql<<",\"deleted\":0";
 }else{ 
	tempsql<<",\"deleted\":"<<std::to_string(data.deleted);
}
if(data.deletetime==0){
	tempsql<<",\"deletetime\":0";
 }else{ 
	tempsql<<",\"deletetime\":"<<std::to_string(data.deletetime);
}
tempsql<<"}";

     
     return tempsql.str();             
   }   
   
   std::string data_tojson(std::string fileld){
        std::ostringstream tempsql;
        std::string keyname;
        unsigned char jj=0;
        std::vector<unsigned char> keypos;
        if(fileld.size()>0){
        for(;jj<fileld.size();jj++){
            if(fileld[jj]==','){
                keypos.emplace_back(findcolpos(keyname)); 
                keyname.clear();
                continue;   
            }
            if(fileld[jj]==0x20){

                continue;   
            }
            keyname.push_back(fileld[jj]);

        }  
        if(keyname.size()>0){
            keypos.emplace_back(findcolpos(keyname)); 
            keyname.clear();
        }
        }else{
            for(jj=0;jj<col_names.size();jj++){
                keypos.emplace_back(jj); 
            }
        }
        tempsql<<"{";
        for(jj=0;jj<keypos.size();jj++){
            switch(keypos[jj]){
         case 0:
 if(jj>0){ tempsql<<","; } 
if(data.id==0){
	tempsql<<"\"id\":0";
 }else{ 
	tempsql<<"\"id\":"<<std::to_string(data.id);
}
 break;
 case 1:
 if(jj>0){ tempsql<<","; } 
if(data.parentid==0){
	tempsql<<"\"parentid\":0";
 }else{ 
	tempsql<<"\"parentid\":"<<std::to_string(data.parentid);
}
 break;
 case 2:
 if(jj>0){ tempsql<<","; } 
if(data.value_id==0){
	tempsql<<"\"value_id\":0";
 }else{ 
	tempsql<<"\"value_id\":"<<std::to_string(data.value_id);
}
 break;
 case 3:
 if(jj>0){ tempsql<<","; } 
tempsql<<"\"content\":\""<<http::utf8_to_jsonstring(data.content)<<"\"";
 break;
 case 4:
 if(jj>0){ tempsql<<","; } 
if(data.deleted==0){
	tempsql<<"\"deleted\":0";
 }else{ 
	tempsql<<"\"deleted\":"<<std::to_string(data.deleted);
}
 break;
 case 5:
 if(jj>0){ tempsql<<","; } 
if(data.deletetime==0){
	tempsql<<"\"deletetime\":0";
 }else{ 
	tempsql<<"\"deletetime\":"<<std::to_string(data.deletetime);
}
 break;

                             default:
                                ;
                     }
                 }   
      tempsql<<"}";  
     return tempsql.str();             
   }   
   
    void from_json(const std::string &json_content)
   {
        record.clear();
        testabase::meta metatemp; 
        data=metatemp;
        unsigned int json_offset=0;
        bool isarray=false;
        //std::vector<std::string> list_content;
        for(;json_offset<json_content.size();json_offset++)
        {
            if(json_content[json_offset]=='{')
            {
                break;
            }
            if(json_content[json_offset]=='[')
            {
                isarray=true;
                break;
            }
        }
        if(isarray)
        {
            json_offset+=1; 
            std::string json_key_name,json_value_name; 
            for(;json_offset<json_content.size();json_offset++)
            {
                for(;json_offset<json_content.size();json_offset++)
                {
                    if(json_content[json_offset]=='{')
                    {
                        json_offset+=1;
                        break;
                    }
                }
                if(record.size()>0)
                {
                    data=metatemp;
                }
                if(json_offset>=json_content.size())
                {
                    break;
                }
                for(;json_offset<json_content.size();json_offset++)
                {
    
                            if(json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                            {
                                continue;
                            }
                            else
                            {
                                if(json_content[json_offset]==0x22)
                                {
                                    unsigned int temp_offset=json_offset;
                                    json_key_name=http::jsonstring_to_utf8(&json_content[json_offset],json_content.size()-json_offset,temp_offset);
                                    json_offset=temp_offset;
                                    if(json_content[json_offset]==0x22)
                                    {
                                        json_offset+=1;
                                    }
                                    for(;json_offset<json_content.size();json_offset++)
                                    {
                                    
                                        if(json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                                        {
                                            continue;
                                        }
                                        break;
                                    }       
                                    if(json_content[json_offset]!=':')
                                    {
                                        break;
                                    }
                                    for(;json_offset<json_content.size();json_offset++)
                                    {
                                        if(json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                                        {
                                            continue;
                                        }
                                        break;
                                    } 
                                    json_offset+=1;
                                    if(json_offset>=json_content.size())
                                    {
                                        break;
                                    }
                                    json_value_name.clear();
                                    if(json_content[json_offset]==0x22)
                                    {
                                        
                                        temp_offset=json_offset;
                                        json_value_name=http::jsonstring_to_utf8(&json_content[json_offset],json_content.size()-json_offset,temp_offset);
                                        json_offset=temp_offset;
                                        if(json_content[json_offset]==0x22)
                                        {
                                            json_offset+=1;
                                        }
                                    }
                                    else
                                    {
                                        if(json_content[json_offset]!='{'&&json_content[json_offset]!=']')
                                        {
                                            for(;json_offset<json_content.size();json_offset++)
                                            {
                                                if(json_content[json_offset]==0x5D||json_content[json_offset]==0x7D||json_content[json_offset]==0x22||json_content[json_offset]==0x2C||json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                                                {
                                                    if(json_content[json_offset]==0x7D)
                                                    {
                                                        json_offset-=1;
                                                    } 
                                                    break;
                                                }
                                                json_value_name.push_back(json_content[json_offset]);
                                            }   
                                        }
                                    }
                                    //////////////////////////
                                    set_val(json_key_name,json_value_name);
                                    continue;
                                }
                                else
                                {
                                    break;
                                }
                            }
    
                }
                record.emplace_back(data);
                
                json_offset+=1;
            }
            if(record.size()>1)
            {
                data=record[0];
            }
        }
        else
        {
           if(json_content[json_offset]=='{')
            {
                json_offset+=1; 
                std::string json_key_name,json_value_name; 
                 
                
                for(;json_offset<json_content.size();json_offset++)
                {
 
                        if(json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                        {
                            continue;
                        }
                        else
                        {
                            if(json_content[json_offset]==0x22)
                            {
                                 unsigned int temp_offset=json_offset;
                                 json_key_name=http::jsonstring_to_utf8(&json_content[json_offset],json_content.size()-json_offset,temp_offset);
                                 json_offset=temp_offset;
                                 if(json_content[json_offset]==0x22)
                                 {
                                    json_offset+=1;
                                 }
                                for(;json_offset<json_content.size();json_offset++)
                                {
                                
                                    if(json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                                    {
                                        continue;
                                    }
                                    break;
                                }       
                                if(json_content[json_offset]!=':')
                                {
                                    break;
                                }
                                for(;json_offset<json_content.size();json_offset++)
                                {
                                    if(json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                                    {
                                        continue;
                                    }
                                    break;
                                } 
                                json_offset+=1;
                                if(json_offset>=json_content.size())
                                {
                                    break;
                                }
                                json_value_name.clear();
                                if(json_content[json_offset]==0x22)
                                {
                                    
                                    temp_offset=json_offset;
                                    json_value_name=http::jsonstring_to_utf8(&json_content[json_offset],json_content.size()-json_offset,temp_offset);
                                    json_offset=temp_offset;
                                    if(json_content[json_offset]==0x22)
                                    {
                                        json_offset+=1;
                                    }
                                }
                                else
                                {
                                    if(json_content[json_offset]!='{'&&json_content[json_offset]!=']')
                                    {
                                        for(;json_offset<json_content.size();json_offset++)
                                        {
                                            if(json_content[json_offset]==0x5D||json_content[json_offset]==0x7D||json_content[json_offset]==0x22||json_content[json_offset]==0x2C||json_content[json_offset]==0x20||json_content[json_offset]==0x0A||json_content[json_offset]==0x0D||json_content[json_offset]=='\t')
                                            {
                                               if(json_content[json_offset]==0x7D)
                                               {
                                                   json_offset-=1;
                                               } 
                                               break;
                                            }
                                            json_value_name.push_back(json_content[json_offset]);
                                        }   
                                    }
                                }
                                //////////////////////////
                                set_val(json_key_name,json_value_name);
                                continue;
                            }
                            else
                            {
                                break;
                            }
                        }
 
                }
                record.emplace_back(data);
            } 
        }
   }   
    
    void set_val(const std::string& set_key_name,const std::string& set_value_name)
    {
        switch(findcolpos(set_key_name))
        {
    		case 0:
		 try{
			data.id=std::stoul(set_value_name);
		}catch (...) { 
			data.id=0;
			 }
			break;
		case 1:
		 try{
			data.parentid=std::stoul(set_value_name);
		}catch (...) { 
			data.parentid=0;
			 }
			break;
		case 2:
		 try{
			data.value_id=std::stoi(set_value_name);
		}catch (...) { 
			data.value_id=0;
			 }
			break;
		case 3:
		 try{
			data.content.append(set_value_name);
		}catch (...) { 
			data.content.clear();
			 }
			break;
		case 4:
		 try{
			data.deleted=std::stoi(set_value_name);
		}catch (...) { 
			data.deleted=0;
			 }
			break;
		case 5:
		 try{
			data.deletetime=std::stoul(set_value_name);
		}catch (...) { 
			data.deletetime=0;
			 }
			break;
	default:
		 { }
			


        }
   } 
    
    void set_val(const std::string& set_key_name,const long long set_value_name)
    {
        switch(findcolpos(set_key_name))
        {
    		case 0:
		 try{
			data.id=set_value_name;
		}catch (...) { 
			data.id=0;
			 }
			break;
		case 1:
		 try{
			data.parentid=set_value_name;
		}catch (...) { 
			data.parentid=0;
			 }
			break;
		case 2:
		 try{
			data.value_id=set_value_name;
		}catch (...) { 
			data.value_id=0;
			 }
			break;
		case 3:
		 try{
			data.content=std::to_string(set_value_name);
		}catch (...) { 
			data.content.clear();
			 }
			break;
		case 4:
		 try{
			data.deleted=set_value_name;
		}catch (...) { 
			data.deleted=0;
			 }
			break;
		case 5:
		 try{
			data.deletetime=set_value_name;
		}catch (...) { 
			data.deletetime=0;
			 }
			break;
	default:
		 { }
			


        }
   } 
    
    void set_val(const std::string& set_key_name,const double set_value_name)
    {
        switch(findcolpos(set_key_name))
        {
    		case 0:
		 try{
			data.id=(unsigned int)set_value_name;
		}catch (...) { 
			data.id=0;
			 }
			break;
		case 1:
		 try{
			data.parentid=(unsigned int)set_value_name;
		}catch (...) { 
			data.parentid=0;
			 }
			break;
		case 2:
		 try{
			data.value_id=(int)set_value_name;
		}catch (...) { 
			data.value_id=0;
			 }
			break;
		case 3:
		 try{
			data.content=std::to_string(set_value_name);
		}catch (...) { 
			data.content.clear();
			 }
			break;
		case 4:
		 try{
			data.deleted=(int)set_value_name;
		}catch (...) { 
			data.deleted=0;
			 }
			break;
		case 5:
		 try{
			data.deletetime=(unsigned int)set_value_name;
		}catch (...) { 
			data.deletetime=0;
			 }
			break;
	default:
		 { }
			


        }
   } 
    
   std::string to_json(std::string fileld=""){
    std::ostringstream tempsql;
    std::string keyname;
    unsigned char jj=0;
    std::vector<unsigned char> keypos;
    if(fileld.size()>0){
        for(;jj<fileld.size();jj++){
            if(fileld[jj]==','){
                keypos.emplace_back(findcolpos(keyname)); 
                keyname.clear();
                continue;   
            }
            if(fileld[jj]==0x20){

                continue;   
            }
            keyname.push_back(fileld[jj]);

        }  
        if(keyname.size()>0){
            keypos.emplace_back(findcolpos(keyname)); 
            keyname.clear();
        }
    }else{
        for(jj=0;jj<col_names.size();jj++){
            keypos.emplace_back(jj); 
        }
    }
    tempsql<<"[";
    for(size_t n=0;n<record.size();n++){
        if(n>0){
            tempsql<<",{";
        }else{
            tempsql<<"{";
        }  
    
        for(jj=0;jj<keypos.size();jj++){
            switch(keypos[jj]){
         case 0:
 if(jj>0){ tempsql<<","; } 
if(record[n].id==0){
	tempsql<<"\"id\":0";
 }else{ 
	tempsql<<"\"id\":"<<std::to_string(record[n].id);
}
 break;
 case 1:
 if(jj>0){ tempsql<<","; } 
if(record[n].parentid==0){
	tempsql<<"\"parentid\":0";
 }else{ 
	tempsql<<"\"parentid\":"<<std::to_string(record[n].parentid);
}
 break;
 case 2:
 if(jj>0){ tempsql<<","; } 
if(record[n].value_id==0){
	tempsql<<"\"value_id\":0";
 }else{ 
	tempsql<<"\"value_id\":"<<std::to_string(record[n].value_id);
}
 break;
 case 3:
 if(jj>0){ tempsql<<","; } 
tempsql<<"\"content\":\""<<http::utf8_to_jsonstring(record[n].content)<<"\"";
 break;
 case 4:
 if(jj>0){ tempsql<<","; } 
if(record[n].deleted==0){
	tempsql<<"\"deleted\":0";
 }else{ 
	tempsql<<"\"deleted\":"<<std::to_string(record[n].deleted);
}
 break;
 case 5:
 if(jj>0){ tempsql<<","; } 
if(record[n].deletetime==0){
	tempsql<<"\"deletetime\":0";
 }else{ 
	tempsql<<"\"deletetime\":"<<std::to_string(record[n].deletetime);
}
 break;

                             default:
                                ;
                     }
                 }   
      tempsql<<"}";  
            }
      tempsql<<"]";
     return tempsql.str();             
   }   
   
   std::string to_json(std::function<bool(std::string&,meta&)> func,std::string fileld=""){
       std::ostringstream tempsql;
        std::string keyname;
        unsigned char jj=0;
        std::vector<unsigned char> keypos;
        if(fileld.size()>0){
            for(;jj<fileld.size();jj++){
                if(fileld[jj]==','){
                    keypos.emplace_back(findcolpos(keyname)); 
                    keyname.clear();
                    continue;   
                }
                if(fileld[jj]==0x20){

                    continue;   
                }
                keyname.push_back(fileld[jj]);

            }  
            if(keyname.size()>0){
                keypos.emplace_back(findcolpos(keyname)); 
                keyname.clear();
            }
        }else{
            for(jj=0;jj<col_names.size();jj++){
                keypos.emplace_back(jj); 
            }
        }
        tempsql<<"[";
        for(size_t n=0;n<record.size();n++){
            keyname.clear();
            if(func(keyname,record[n])){ 
                if(n>0){
                    tempsql<<",{";
                }else{
                    tempsql<<"{";
                } 
                tempsql<<keyname;
            }else{
            continue;
            } 
        
        for(jj=0;jj<keypos.size();jj++){
            
            switch(keypos[jj]){
         case 0:
 if(jj>0){ tempsql<<","; } 
if(record[n].id==0){
	tempsql<<"\"id\":0";
 }else{ 
	tempsql<<"\"id\":"<<std::to_string(record[n].id);
}
 break;
 case 1:
 if(jj>0){ tempsql<<","; } 
if(record[n].parentid==0){
	tempsql<<"\"parentid\":0";
 }else{ 
	tempsql<<"\"parentid\":"<<std::to_string(record[n].parentid);
}
 break;
 case 2:
 if(jj>0){ tempsql<<","; } 
if(record[n].value_id==0){
	tempsql<<"\"value_id\":0";
 }else{ 
	tempsql<<"\"value_id\":"<<std::to_string(record[n].value_id);
}
 break;
 case 3:
 if(jj>0){ tempsql<<","; } 
tempsql<<"\"content\":\""<<http::utf8_to_jsonstring(record[n].content)<<"\"";
 break;
 case 4:
 if(jj>0){ tempsql<<","; } 
if(record[n].deleted==0){
	tempsql<<"\"deleted\":0";
 }else{ 
	tempsql<<"\"deleted\":"<<std::to_string(record[n].deleted);
}
 break;
 case 5:
 if(jj>0){ tempsql<<","; } 
if(record[n].deletetime==0){
	tempsql<<"\"deletetime\":0";
 }else{ 
	tempsql<<"\"deletetime\":"<<std::to_string(record[n].deletetime);
}
 break;

                             default:
                                ;
                     }
                 }   
      tempsql<<"}";  
            }
      tempsql<<"]";
     return tempsql.str();             
   }   
   long long getPK(){  return data.id; } 
 void setPK(long long val){  data.id=val;} 
 unsigned  int  getId(){  return data.id; } 
 void setId( unsigned  int  val){  data.id=val;} 

 unsigned  int  getParentid(){  return data.parentid; } 
 void setParentid( unsigned  int  val){  data.parentid=val;} 

 char  getValueId(){  return data.value_id; } 
 void setValueId( char  val){  data.value_id=val;} 

 std::string  getContent(){  return data.content; } 
 std::string & getRefContent(){  return std::ref(data.content); } 
 void setContent( std::string  &val){  data.content=val;} 
 void setContent(std::string_view val){  data.content=val;} 

 unsigned  char  getDeleted(){  return data.deleted; } 
 void setDeleted( unsigned  char  val){  data.deleted=val;} 

 unsigned  int  getDeletetime(){  return data.deletetime; } 
 void setDeletetime( unsigned  int  val){  data.deletetime=val;} 

testabase::meta getnewData(){
 	 struct meta newdata;
	 return newdata; 
} 
testabase::meta getData(){
 	 return data; 
} 
std::vector<testabase::meta> getRecord(){
 	 return record; 
} 

   std::string tree_tojson(const std::vector<meta_tree> &tree_data, std::string fileld=""){
       std::ostringstream tempsql;
        std::string keyname;
        unsigned char jj=0;
        std::vector<unsigned char> keypos;
        if(fileld.size()>0){
            for(;jj<fileld.size();jj++){
                if(fileld[jj]==','){
                    keypos.emplace_back(findcolpos(keyname)); 
                    keyname.clear();
                    continue;   
                }
                if(fileld[jj]==0x20){

                    continue;   
                }
                keyname.push_back(fileld[jj]);

            }  
            if(keyname.size()>0){
                            keypos.emplace_back(findcolpos(keyname)); 
                            keyname.clear();
            }
        }else{
            for(jj=0;jj<col_names.size();jj++){
                keypos.emplace_back(jj); 
            }
        }
        tempsql<<"[";
        for(size_t n=0;n<tree_data.size();n++){
            if(n>0){
                tempsql<<",{";
            }else{
                tempsql<<"{";
            }  
        
        for(jj=0;jj<keypos.size();jj++){
            switch(keypos[jj]){
         case 0:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].id==0){
	tempsql<<"\"id\":0";
 }else{ 
	tempsql<<"\"id\":"<<std::to_string(tree_data[n].id);
}
 break;
 case 1:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].parentid==0){
	tempsql<<"\"parentid\":0";
 }else{ 
	tempsql<<"\"parentid\":"<<std::to_string(tree_data[n].parentid);
}
 break;
 case 2:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].value_id==0){
	tempsql<<"\"value_id\":0";
 }else{ 
	tempsql<<"\"value_id\":"<<std::to_string(tree_data[n].value_id);
}
 break;
 case 3:
 if(jj>0){ tempsql<<","; } 
tempsql<<"\"content\":\""<<http::utf8_to_jsonstring(tree_data[n].content)<<"\"";
 break;
 case 4:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].deleted==0){
	tempsql<<"\"deleted\":0";
 }else{ 
	tempsql<<"\"deleted\":"<<std::to_string(tree_data[n].deleted);
}
 break;
 case 5:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].deletetime==0){
	tempsql<<"\"deletetime\":0";
 }else{ 
	tempsql<<"\"deletetime\":"<<std::to_string(tree_data[n].deletetime);
}
 break;

                             default:
                                ;
                     }
                 }

        tempsql<<",\"children\":";
         tempsql<<tree_tojson(tree_data[n].children, fileld);     
      tempsql<<"}";  
            }
      tempsql<<"]";
     return tempsql.str();             
   }   
   
   std::string tree_tojson(const std::vector<meta_tree> &tree_data,std::function<bool(std::string&,const meta_tree&)> func,std::string fileld=""){
       std::ostringstream tempsql;
        std::string keyname;
        unsigned char jj=0;
        std::vector<unsigned char> keypos;
        if(fileld.size()>0){
            for(;jj<fileld.size();jj++){
                if(fileld[jj]==','){
                    keypos.emplace_back(findcolpos(keyname)); 
                    keyname.clear();
                    continue;   
                }
                if(fileld[jj]==0x20){

                    continue;   
                }
                keyname.push_back(fileld[jj]);

            }  
            if(keyname.size()>0){
                            keypos.emplace_back(findcolpos(keyname)); 
                            keyname.clear();
            }
        }else{
            for(jj=0;jj<col_names.size();jj++){
                keypos.emplace_back(jj); 
            }
        }
    tempsql<<"[";
    for(size_t n=0;n<tree_data.size();n++){
        keyname.clear();
        if(func(keyname,tree_data[n])){ 
                if(n>0){
                    tempsql<<",{";
                }else{
                    tempsql<<"{";
                } 
                tempsql<<keyname;
        }else{
        continue;
        } 
        
        for(jj=0;jj<keypos.size();jj++){
            
            switch(keypos[jj]){
         case 0:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].id==0){
	tempsql<<"\"id\":0";
 }else{ 
	tempsql<<"\"id\":"<<std::to_string(tree_data[n].id);
}
 break;
 case 1:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].parentid==0){
	tempsql<<"\"parentid\":0";
 }else{ 
	tempsql<<"\"parentid\":"<<std::to_string(tree_data[n].parentid);
}
 break;
 case 2:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].value_id==0){
	tempsql<<"\"value_id\":0";
 }else{ 
	tempsql<<"\"value_id\":"<<std::to_string(tree_data[n].value_id);
}
 break;
 case 3:
 if(jj>0){ tempsql<<","; } 
tempsql<<"\"content\":\""<<http::utf8_to_jsonstring(tree_data[n].content)<<"\"";
 break;
 case 4:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].deleted==0){
	tempsql<<"\"deleted\":0";
 }else{ 
	tempsql<<"\"deleted\":"<<std::to_string(tree_data[n].deleted);
}
 break;
 case 5:
 if(jj>0){ tempsql<<","; } 
if(tree_data[n].deletetime==0){
	tempsql<<"\"deletetime\":0";
 }else{ 
	tempsql<<"\"deletetime\":"<<std::to_string(tree_data[n].deletetime);
}
 break;

                             default:
                                ;
                     }
                 }   
         tempsql<<",\"children\":";
         tempsql<<tree_tojson(tree_data[n].children,func,fileld);     
      tempsql<<"}";  
            }
      tempsql<<"]";
     return tempsql.str();             
   }   
   
    meta_tree treedata_from_record(unsigned int i=0)
    {
        meta_tree temp_obja;
        if(i>=record.size())
        {
           return  temp_obja;   
        }
        	temp_obja.id=record[i].id;
	temp_obja.parentid=record[i].parentid;
	temp_obja.value_id=record[i].value_id;
	temp_obja.content=record[i].content;
	temp_obja.deleted=record[i].deleted;
	temp_obja.deletetime=record[i].deletetime;

        return  temp_obja;   
    }
    meta_tree treedata_from_data()
    {
        meta_tree temp_obja;

        	temp_obja.id=data.id;
	temp_obja.parentid=data.parentid;
	temp_obja.value_id=data.value_id;
	temp_obja.content=data.content;
	temp_obja.deleted=data.deleted;
	temp_obja.deletetime=data.deletetime;

        return  temp_obja;   
    }      
    meta_tree treedata_from_data(const meta &tempdata)
    {
        meta_tree temp_obja;
        	temp_obja.id=tempdata.id;
	temp_obja.parentid=tempdata.parentid;
	temp_obja.value_id=tempdata.value_id;
	temp_obja.content=tempdata.content;
	temp_obja.deleted=tempdata.deleted;
	temp_obja.deletetime=tempdata.deletetime;

        return  temp_obja;   
    }     
    std::vector<meta_tree> to_tree(unsigned int beginid=0)
    {
       std::vector<meta_tree> temp;
       unsigned int level=0; 
       if(beginid==0)
       {
            for (unsigned int i = 0; i < record.size(); i++)
            {
                if (record[i].parentid == 0)
                {
                    		meta_tree temp_obja;
						temp_obja.id=record[i].id;
						temp_obja.parentid=record[i].parentid;
						temp_obja.value_id=record[i].value_id;
						temp_obja.content=record[i].content;
						temp_obja.deleted=record[i].deleted;
						temp_obja.deletetime=record[i].deletetime;

                    temp.push_back(temp_obja);
                }
            }
       }
       else
       {
           for (unsigned int i = 0; i < record.size(); i++)
            {
                if (record[i].id == beginid)
                {
                    		meta_tree temp_obja;
						temp_obja.id=record[i].id;
						temp_obja.parentid=record[i].parentid;
						temp_obja.value_id=record[i].value_id;
						temp_obja.content=record[i].content;
						temp_obja.deleted=record[i].deleted;
						temp_obja.deletetime=record[i].deletetime;

                    temp.push_back(temp_obja);
                    break;
                }
            }
       }

       if(temp.size()==0)
       {
          return temp; 
       }
       level+=1;
       for (unsigned int j = 0; j < temp.size(); j++)
       {
         record_to_tree(temp[j].children,temp[j].id,level);
       }
       return temp; 
    }    
    void record_to_tree(std::vector<meta_tree> &targetdata,long long t_vid,unsigned int level=0)
    {
        for (unsigned int i = 0; i < record.size(); i++)
        {
            if (record[i].parentid== t_vid)
            {
                		meta_tree temp_obja;
						temp_obja.id=record[i].id;
						temp_obja.parentid=record[i].parentid;
						temp_obja.value_id=record[i].value_id;
						temp_obja.content=record[i].content;
						temp_obja.deleted=record[i].deleted;
						temp_obja.deletetime=record[i].deletetime;

                targetdata.push_back(temp_obja);
            }
        }
        level+=1;
        for (unsigned int j = 0; j < targetdata.size(); j++)
        {
         record_to_tree(targetdata[j].children,targetdata[j].id,level);
        }
    }
    void tree_torecord(const std::vector<meta_tree> &sourcedata,unsigned int level=0)
    {
        for (unsigned int i = 0; i < sourcedata.size(); i++)
        {
		meta temp_obja;
			temp_obja.id=sourcedata[i].id;
			temp_obja.parentid=sourcedata[i].parentid;
			temp_obja.value_id=sourcedata[i].value_id;
			temp_obja.content=sourcedata[i].content;
			temp_obja.deleted=sourcedata[i].deleted;
			temp_obja.deletetime=sourcedata[i].deletetime;

            record.push_back(temp_obja);
            if(sourcedata[i].children.size()>0)
            {
                tree_torecord(sourcedata[i].children,level+1);
            }
        }
    }      
   

    template<typename T, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true>
    T& ref_meta([[maybe_unused]] std::string key_name)
    {
   		 if(key_name=="content")
		{
			return data.content;
		}
		return nullptr; 
	}


    template<typename T, typename std::enable_if<std::is_integral_v<T>,bool>::type = true>
    T& ref_meta([[maybe_unused]] std::string key_name)
    {
   		 if(key_name=="id")
		{
			return data.id;
		}
		 if(key_name=="parentid")
		{
			return data.parentid;
		}
		 if(key_name=="value_id")
		{
			return data.value_id;
		}
		 if(key_name=="deleted")
		{
			return data.deleted;
		}
		 if(key_name=="deletetime")
		{
			return data.deletetime;
		}
		return nullptr; 
	}


    template<typename T, typename std::enable_if<std::is_floating_point_v<T>,bool>::type = true >
    T& ref_meta([[maybe_unused]] std::string key_name)
    {
   		return nullptr; 
	}

            template<typename T, typename std::enable_if<std::is_integral_v<T>,bool>::type = true >  
            std::vector<T> getCol([[maybe_unused]] std::string keyname)
            {
                std::vector<T> a;
                
   
                unsigned char kpos;
                kpos=findcolpos(keyname);               
                for(auto &iter:record)
                {
                    switch(kpos)
                    {
   			case 0: 
 				 a.emplace_back(iter.id);
				 break;
			case 1: 
 				 a.emplace_back(iter.parentid);
				 break;
			case 2: 
 				 a.emplace_back(iter.value_id);
				 break;
			case 4: 
 				 a.emplace_back(iter.deleted);
				 break;
			case 5: 
 				 a.emplace_back(iter.deletetime);
				 break;

                    }
                }
    
                return a;
            }
    
            template<typename T, typename std::enable_if<std::is_floating_point_v<T>,bool>::type = true >    
			std::vector<T> getCol([[maybe_unused]] std::string keyname)
			{
				std::vector<T> a;
				

                return a;
            }
    
            template<typename T, typename std::enable_if<std::is_integral_v<T>,bool>::type = true >   
            T getVal([[maybe_unused]] std::string keyname)
            {
   
                    unsigned char kpos;
                    kpos=findcolpos(keyname);                   
                    switch(kpos)
                    {

   			case 0: 
 				 return data.id;
				 break;
			case 1: 
 				 return data.parentid;
				 break;
			case 2: 
 				 return data.value_id;
				 break;
			case 4: 
 				 return data.deleted;
				 break;
			case 5: 
 				 return data.deletetime;
				 break;
			}
                return 0;
            }  
    
        template<typename T, typename std::enable_if<std::is_integral_v<T>,bool>::type = true > 
        T getVal([[maybe_unused]] testabase::meta & iter,[[maybe_unused]] std::string keyname)
        {

          
            unsigned char kpos;
            kpos=findcolpos(keyname);   
            switch(kpos)
            {
   			case 0: 
 				 return iter.id;
				 break;
			case 1: 
 				 return iter.parentid;
				 break;
			case 2: 
 				 return iter.value_id;
				 break;
			case 4: 
 				 return iter.deleted;
				 break;
			case 5: 
 				 return iter.deletetime;
				 break;

			}

            return 0;
        }  
    
            template<typename T, typename std::enable_if<std::is_floating_point_v<T>,bool>::type = true > 
            T getVal(std::string keyname)
            {
                unsigned char kpos;
                kpos=findcolpos(keyname);
            
                switch(kpos)
                {

    
                    }
                   
    
                    return 0.0;
            }  
    
            template<typename T, typename std::enable_if<std::is_floating_point_v<T>,bool>::type = true > 
            T getVal([[maybe_unused]] testabase::meta & iter,std::string keyname)
            {
                unsigned char kpos;
                kpos=findcolpos(keyname);
                switch(kpos)
                {
   
                }
                 
    
            
                return 0.0;
            }  
    
            template<typename T, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true > 
            std::string getVal(std::string keyname)
            {
                unsigned char kpos;
                kpos=findcolpos(keyname);
        
                switch(kpos)
                {

   			case 3: 
 				 return data.content;
				 break;

                }
                return "";
            }  
   
            template<typename T, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true > 
            std::string getVal([[maybe_unused]] testabase::meta & iter,std::string keyname)
            {
         
                unsigned char kpos;
                kpos=findcolpos(keyname);
       
                switch(kpos)
                {

   			case 3: 
 				 return iter.content;
				 break;

                }
                
    
                 
                return "";
            }  
     
            template<typename T, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true >   
            std::vector<std::string> getCol([[maybe_unused]] std::string keyname)
            {
                std::vector<std::string> a;

           
                unsigned char kpos;
                kpos=findcolpos(keyname);                    
                for(auto &iter:record)
                {
                    switch(kpos)
                    {

    			case 3: 
 				 a.emplace_back(iter.content);
					 break;
					}
				}

        return a;
    }
     
        std::string getstrCol(std::string keyname,[[maybe_unused]] bool isyinhao=false)
        {
            std::ostringstream a;
    
            unsigned char kpos;
            kpos=findcolpos(keyname);   
            int j=0;
            if(isyinhao&&record.size()>0)
            {
                a<<'"';
            }
            for(auto &iter:record)
            {
                    if(j>0)
                    {
                        if(isyinhao)
                        {
                            a<<"\",\"";
                        }else{
                            a<<',';    
                        }
                    }
                    switch(kpos)
                    {

   			case 0: 
 				 a<<std::to_string(iter.id);
				 break;
			case 1: 
 				 a<<std::to_string(iter.parentid);
				 break;
			case 2: 
 				 a<<std::to_string(iter.value_id);
				 break;
			case 3: 
 				 if(isyinhao){ a<<jsonaddslash(iter.content); 
				 }else{
				 a<<iter.content;
				 }
				 break;
			case 4: 
 				 a<<std::to_string(iter.deleted);
				 break;
			case 5: 
 				 a<<std::to_string(iter.deletetime);
				 break;

                    }
                    j++;
            } 
            if(isyinhao&&j>0){
                a<<'"';
            }      
    
                return a.str();
        }
    
    template<typename T,typename U,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true>     
    std::map<std::string,std::string> getCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
    {
        std::map<std::string,std::string> a;
    
        unsigned char kpos,vpos;
        kpos=findcolpos(keyname);
        vpos=findcolpos(valname);        
         std::string ktemp,vtemp;
         for(auto &iter:record)
         {
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
				 } 
			switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;

                }
                if(ktemp.size()>0)
                {
                    a.emplace(ktemp,vtemp);
                }
            }       

        
            return a;
        } 
    

        template<typename T,typename U,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true, typename std::enable_if<std::is_floating_point<U>::value,bool>::type = true>    
        std::map<std::string,U> getCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
                std::map<std::string,U> a;
      
                unsigned char kpos,vpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);            
                std::string ktemp;
                U vtemp;
                for(auto &iter:record)
                {    
                    switch(kpos)
                    {
 
       			case 3: 
 				 ktemp=iter.content;
				 break;
			 } 
		 switch(vpos){

                    }
                    if(ktemp.size()>0)
                    {
                        a.emplace(ktemp,vtemp);
                    }
                }       
        
            return a;
        } 
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_floating_point<U>::value,bool>::type = true>       
        std::map<T,U> getCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,U> a;
       
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);        
            T ktemp;
            U vtemp;
            for(auto &iter:record)
            {
                switch(kpos)
                {
 
       case 0: 
 	 ktemp=iter.id;
	 break;
case 1: 
 	 ktemp=iter.parentid;
	 break;
case 2: 
 	 ktemp=iter.value_id;
	 break;
case 4: 
 	 ktemp=iter.deleted;
	 break;
case 5: 
 	 ktemp=iter.deletetime;
	 break;
	 } 
 		  switch(vpos){

                }
                if(ktemp.size()>0)
                {
                    a.emplace(ktemp,vtemp);
                }
            }       
     
        return a;
    }  
            template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true>      
            std::map<T,std::string> getCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
            {
                std::map<T,std::string> a;
   
                unsigned char kpos,vpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);         
                T ktemp;
                std::string vtemp;
                for(auto &iter:record)
                {
                    switch(kpos)
                    {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }
 			switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;

                    }
                    if(ktemp.size()>0)
                    {
                        a.emplace(ktemp,vtemp);
                    }
                } 
         
                return a;
            }     
        
        template<typename T,typename U, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>       
        std::map<std::string,U> getCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<std::string,U> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);            
            std::string  ktemp;
            U  vtemp;
            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			  }
 			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;

                }
                if(ktemp.size()>0)
                {
                    a.emplace(ktemp,vtemp);
                }
            }       
      
        return a;
    }  
    
        template<typename T,typename U, typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>   
        std::map<T,U> getCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,U> a;
    
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);            
            T ktemp;
            U vtemp;
            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }
 			switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;

                }
                if(ktemp.size()>0)
                {
                    a.emplace(ktemp,vtemp);
                }
            }       
    
            return a;
        }   
    
        template<typename T, typename std::enable_if<std::is_integral_v<T>,bool>::type = true >         
        std::map<T,meta> getmapRows([[maybe_unused]] std::string keyname)
        {
            std::map<T,meta> a;
    
            unsigned char kpos;
            kpos=findcolpos(keyname);                        
            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 0: 
 				 a.emplace(iter.id,iter);
				 break;
			case 1: 
 				 a.emplace(iter.parentid,iter);
				 break;
			case 2: 
 				 a.emplace(iter.value_id,iter);
				 break;
			case 4: 
 				 a.emplace(iter.deleted,iter);
				 break;
			case 5: 
 				 a.emplace(iter.deletetime,iter);
				 break;

                }
            }       
     
            return a;
        }     
    
        template<typename T, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true >    
        std::map<std::string,meta> getmapRows([[maybe_unused]] std::string keyname)
        {
            std::map<std::string,meta> a;

    
            unsigned char kpos;
            kpos=findcolpos(keyname);            
            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 3: 
 				 a.emplace(iter.content,iter);
			 break;

                }
                //a.emplace(ktemp,iter);
            }       
    

        return a;
    }
    
        template<typename T,typename U,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true, typename std::enable_if<std::is_floating_point<U>::value,bool>::type = true>  
        std::vector<std::pair<std::string,U>> getvecCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::vector<std::pair<std::string,U>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);                   
            std::string ktemp;
            U vtemp;
            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
	 		 }
 			switch(vpos){

                   }

                 a.emplace_back(ktemp,vtemp);
            }       

     

            return a;
        }   
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_floating_point<U>::value,bool>::type = true>    
        std::vector<std::pair<T,U>> getvecCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
                std::vector<std::pair<T,U>> a;
   
                unsigned char kpos,vpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);
                T ktemp;
                U vtemp;
                for(auto &iter:record)
                {
                    switch(kpos)
                    {

   			case 0: 
 				 ktemp=iter.id;
			 break;
			case 1: 
 				 ktemp=iter.parentid;
			 break;
			case 2: 
 				 ktemp=iter.value_id;
			 break;
			case 4: 
 				 ktemp=iter.deleted;
			 break;
			case 5: 
 				 ktemp=iter.deletetime;
			 break;
			  }
			 switch(vpos){

                   }

                   a.emplace_back(ktemp,vtemp);
                }       

    

            return a;
        }   
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true>    
        std::vector<std::pair<T,U>> getvecCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
                std::vector<std::pair<T,U>> a;

   
                unsigned char kpos,vpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);
                T ktemp;
                U vtemp;
                for(auto &iter:record)
                {
                    switch(kpos)
                    {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }
 			switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;

                   }

                    a.emplace_back(ktemp,vtemp);
                }       
    
            return a;
        }  
    
        template<typename T,typename U, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>     
        std::vector<std::pair<T,U>> getvecCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
                std::vector<std::pair<T,U>> a;
   
                unsigned char kpos,vpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);                
                T ktemp;
                U vtemp;
                for(auto &iter:record)
                {
                    
                    switch(kpos)
                    {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			  }
 			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;

                   }
                    a.emplace_back(ktemp,vtemp);
                }       
    
            return a;
        }  
    
        template<typename T,typename U, typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>    
        std::vector<std::pair<T,U>> getvecCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
                std::vector<std::pair<T,U>> a;
   
                unsigned char kpos,vpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);
                T ktemp;
                U vtemp;
                for(auto &iter:record)
                {
                    switch(kpos)
                    {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }
			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;

                   }
                    a.emplace_back(ktemp,vtemp);
                }       
      
            return a;
        }   
    
        template<typename T,typename U, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true>     
        std::vector<std::pair<T,U>> getvecCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
                std::vector<std::pair<T,U>> a;
   
                unsigned char kpos,vpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);
                T ktemp;
                U vtemp;
                for(auto &iter:record)
                {
                    switch(kpos)
                    {

   case 3: 
 	 ktemp=iter.content;
	 break;
	  }
 switch(vpos){
case 3: 
 	 vtemp=iter.content;
	 break;

                   }

                    a.emplace_back(ktemp,vtemp);
                }       
    
            return a;
        }  
    
        template<typename T, typename std::enable_if<std::is_integral_v<T>,bool>::type = true >   
        std::vector<std::pair<T,meta>> getvecRows([[maybe_unused]] std::string keyname)
        {
            std::vector<std::pair<T,meta>> a;
     
            unsigned char kpos;
            kpos=findcolpos(keyname);                  
            for(auto &iter:record)
            { 
                switch(kpos)
                {

   case 0: 
 	 a.emplace_back(iter.id,iter);
	 break;
case 1: 
 	 a.emplace_back(iter.parentid,iter);
	 break;
case 2: 
 	 a.emplace_back(iter.value_id,iter);
	 break;
case 4: 
 	 a.emplace_back(iter.deleted,iter);
	 break;
case 5: 
 	 a.emplace_back(iter.deletetime,iter);
	 break;

                }
            }       
    
        return a;
    }
        template<typename T, typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true >  
        std::vector<std::pair<std::string,meta>> getvecRows([[maybe_unused]] std::string keyname)
        {
            std::vector<std::pair<std::string,meta>> a;
      
            unsigned char kpos;
            kpos=findcolpos(keyname);                     
            for(auto &iter:record)
            {
                switch(kpos)
                {

   case 3: 
 	 a.emplace_back(iter.content,iter);
	 break;

                }
            }       
    

        return a;
    }
        template<typename T,typename U,typename D,typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true, typename std::enable_if<std::is_floating_point<D>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
    
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);      
            T ktemp;
            U vtemp;
            for(auto &iter:record)
            { 
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }

			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;
			  }

			 switch(dpos){

                }
            }       

    
            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true, typename std::enable_if<std::is_integral_v<D>,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
    
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);          
            T ktemp;
            U vtemp;
            //D vtemp;

            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }

			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;
			  }

			 switch(dpos){
			case 0: 
 				 a[ktemp][vtemp].emplace_back(iter.id);
				 break;
			case 1: 
 				 a[ktemp][vtemp].emplace_back(iter.parentid);
				 break;
			case 2: 
 				 a[ktemp][vtemp].emplace_back(iter.value_id);
				 break;
			case 4: 
 				 a[ktemp][vtemp].emplace_back(iter.deleted);
				 break;
			case 5: 
 				 a[ktemp][vtemp].emplace_back(iter.deletetime);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true, typename std::enable_if<std::is_same<D,std::string>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
   
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);       
            T ktemp;
            U vtemp;
            // D dtemp;

            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
				  }

			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;
			 }

			 switch(dpos){
			case 3: 
 				 a[ktemp][vtemp].emplace_back(iter.content);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true, typename std::enable_if<std::is_floating_point<D>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
                std::map<T,std::map<U,std::vector<D>>> a;
   
                unsigned char kpos,vpos,dpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);
                dpos=findcolpos(dataname);
                T ktemp;
                U vtemp;
            // D dtemp;

                for(auto &iter:record)
                {
                    
                    switch(kpos)
                    {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			 }

			 switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;
			  }

			 switch(dpos){

                   }
                }       
    
            return a;
        }
    
    template<typename T,typename U,typename D,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true, typename std::enable_if<std::is_integral_v<D>,bool>::type = true>    
    std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
    {
        std::map<T,std::map<U,std::vector<D>>> a;

   
        unsigned char kpos,vpos,dpos;
        kpos=findcolpos(keyname);
        vpos=findcolpos(valname);
        dpos=findcolpos(dataname);             
        T ktemp;
        U vtemp;
       // D dtemp;

         for(auto &iter:record)
         {
            switch(kpos)
            {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			 }

			 switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;
			 }

			 switch(dpos){
			case 0: 
 				 a[ktemp][vtemp].emplace_back(iter.id);
				 break;
			case 1: 
 				 a[ktemp][vtemp].emplace_back(iter.parentid);
				 break;
			case 2: 
 				 a[ktemp][vtemp].emplace_back(iter.value_id);
				 break;
			case 4: 
 				 a[ktemp][vtemp].emplace_back(iter.deleted);
				 break;
			case 5: 
 				 a[ktemp][vtemp].emplace_back(iter.deletetime);
				 break;

            }
         }       
    
        return a;
    }
        template<typename T,typename U,typename D,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true, typename std::enable_if<std::is_same<D,std::string>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
   
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);
            T ktemp;
            U vtemp;
            // D dtemp;

            for(auto &iter:record)
            { 
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }

			 switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;
			  }

			 switch(dpos){
			case 3: 
 				 a[ktemp][vtemp].emplace_back(iter.content);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true, typename std::enable_if<std::is_floating_point<D>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
                std::map<T,std::map<U,std::vector<D>>> a;
   
                unsigned char kpos,vpos,dpos;
                kpos=findcolpos(keyname);
                vpos=findcolpos(valname);
                dpos=findcolpos(dataname);               
                T ktemp;
                U vtemp;
                //D vtemp;
                for(auto &iter:record)
                {
                    switch(kpos)
                    {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			 }

			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;
			  }

			 switch(dpos){

                    }
                }       
    

            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true, typename std::enable_if<std::is_integral_v<D>,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
   
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);            
            T ktemp;
            U vtemp;
            //D vtemp;

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			  }

			 switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;
			 }

			 switch(dpos){
			case 0: 
 				 a[ktemp][vtemp].emplace_back(iter.id);
				 break;
			case 1: 
 				 a[ktemp][vtemp].emplace_back(iter.parentid);
				 break;
			case 2: 
 				 a[ktemp][vtemp].emplace_back(iter.value_id);
				 break;
			case 4: 
 				 a[ktemp][vtemp].emplace_back(iter.deleted);
				 break;
			case 5: 
 				 a[ktemp][vtemp].emplace_back(iter.deletetime);
				 break;

                }
            }       
    
            return a;
        }
    
    template<typename T,typename U,typename D,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true, typename std::enable_if<std::is_same<D,std::string>::value,bool>::type = true>    
    std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
    {
        std::map<T,std::map<U,std::vector<D>>> a;

   
        unsigned char kpos,vpos,dpos;
        kpos=findcolpos(keyname);
        vpos=findcolpos(valname);
        dpos=findcolpos(dataname);
        T ktemp;
        U vtemp;
        // D dtemp;

         for(auto &iter:record)
         {
             
            switch(kpos)
            {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			 }

			switch(vpos){
			case 0: 
 				 vtemp=iter.id;
				 break;
			case 1: 
 				 vtemp=iter.parentid;
				 break;
			case 2: 
 				 vtemp=iter.value_id;
				 break;
			case 4: 
 				 vtemp=iter.deleted;
				 break;
			case 5: 
 				 vtemp=iter.deletetime;
				 break;
			 }

			switch(dpos){
			case 3: 
 				 a[ktemp][vtemp].emplace_back(iter.content);
				 break;

            }
         }       
    
            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true, typename std::enable_if<std::is_floating_point<D>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
   
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);        
            T ktemp;
            U vtemp;
            // D dtemp;

            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			 }

			 switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;
			  }

			 switch(dpos){

                }
            }       

    
            return a;
        }
        template<typename T,typename U,typename D,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true, typename std::enable_if<std::is_integral_v<D>,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
   
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);
            T ktemp;
            U vtemp;
            // D dtemp;

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			  }

			 switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;
			 }

			 switch(dpos){
			case 0: 
 				 a[ktemp][vtemp].emplace_back(iter.id);
				 break;
			case 1: 
 				 a[ktemp][vtemp].emplace_back(iter.parentid);
				 break;
			case 2: 
 				 a[ktemp][vtemp].emplace_back(iter.value_id);
				 break;
			case 4: 
 				 a[ktemp][vtemp].emplace_back(iter.deleted);
				 break;
			case 5: 
 				 a[ktemp][vtemp].emplace_back(iter.deletetime);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true, typename std::enable_if<std::is_same<D,std::string>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<D>>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname,[[maybe_unused]] std::string dataname)
        {
            std::map<T,std::map<U,std::vector<D>>> a;
   
            unsigned char kpos,vpos,dpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            dpos=findcolpos(dataname);        
            T ktemp;
            U vtemp;
            // D dtemp;

            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			  }

			 switch(vpos){
			case 3: 
 				 vtemp=iter.content;
				 break;
			  }

			 switch(dpos){
			case 3: 
 				 a[ktemp][vtemp].emplace_back(iter.content);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true>    
        std::map<T,std::vector<U>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::vector<U>> a;

   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            T ktemp;
            //U vtemp;

            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			  }

			 switch(vpos){
			case 3: 
 				 a[ktemp].emplace_back(iter.content);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_floating_point<U>::value,bool>::type = true>    
        std::map<T,std::vector<U>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::vector<U>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            T ktemp;
            //U vtemp;

            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			 }

			 switch(vpos){

                }
            }       

    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>    
        std::map<T,std::vector<U>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::vector<U>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            T ktemp;
            //U vtemp;

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			  }

			 switch(vpos){
			case 0: 
 				 a[ktemp].emplace_back(iter.id);
				 break;
			case 1: 
 				 a[ktemp].emplace_back(iter.parentid);
				 break;
			case 2: 
 				 a[ktemp].emplace_back(iter.value_id);
				 break;
			case 4: 
 				 a[ktemp].emplace_back(iter.deleted);
				 break;
			case 5: 
 				 a[ktemp].emplace_back(iter.deletetime);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true>    
        std::map<T,std::vector<U>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::vector<U>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            T ktemp;
            //U vtemp;

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			 }

			 switch(vpos){
			case 3: 
 				 a[ktemp].emplace_back(iter.content);
				 break;

                }
            }       

    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_floating_point<U>::value,bool>::type = true>    
        std::map<T,std::vector<U>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::vector<U>> a;
               
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            T ktemp;
            //U vtemp;

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }

			 switch(vpos){

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>    
        std::map<T,std::vector<U>> getgroupCols([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::vector<U>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            T ktemp;
            //U vtemp;

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			 }

			 switch(vpos){
			case 0: 
 				 a[ktemp].emplace_back(iter.id);
				 break;
			case 1: 
 				 a[ktemp].emplace_back(iter.parentid);
				 break;
			case 2: 
 				 a[ktemp].emplace_back(iter.value_id);
				 break;
			case 4: 
 				 a[ktemp].emplace_back(iter.deleted);
				 break;
			case 5: 
 				 a[ktemp].emplace_back(iter.deletetime);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename std::enable_if<std::is_integral_v<T>,bool>::type = true>    
        std::map<T,std::vector<meta>> getgroupRows([[maybe_unused]] std::string keyname)
        {
            std::map<T,std::vector<meta>> a;
   
            unsigned char kpos;
            kpos=findcolpos(keyname);

            for(auto &iter:record)
            {
                switch(kpos)
                {

   			case 0: 
 				 a[iter.id].emplace_back(iter);
				 break;
			case 1: 
 				 a[iter.parentid].emplace_back(iter);
				 break;
			case 2: 
 				 a[iter.value_id].emplace_back(iter);
				 break;
			case 4: 
 				 a[iter.deleted].emplace_back(iter);
				 break;
			case 5: 
 				 a[iter.deletetime].emplace_back(iter);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true>    
        std::map<T,std::vector<meta>> getgroupRows([[maybe_unused]] std::string keyname)
        {
            std::map<T,std::vector<meta>> a;
   
            unsigned char kpos;
            kpos=findcolpos(keyname);

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 3: 
 				 a[iter.content].emplace_back(iter);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename D,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true, typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<meta>>> getgroupRows([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::map<U,std::vector<meta>>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);
            T ktemp;

            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
			 }

			 switch(vpos){
			case 3: 
 				 a[ktemp][iter.content].emplace_back(iter);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_same<T,std::string>::value,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>    
        std::map<T,std::map<U,std::vector<meta>>> getgroupRows([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::map<U,std::vector<meta>>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);            
            T ktemp;
            
            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 3: 
 				 ktemp=iter.content;
				 break;
	  }

 switch(vpos){
			case 0: 
 				 a[ktemp][iter.id].emplace_back(iter);
				 break;
			case 1: 
 				 a[ktemp][iter.parentid].emplace_back(iter);
				 break;
			case 2: 
 				 a[ktemp][iter.value_id].emplace_back(iter);
				 break;
			case 4: 
 				 a[ktemp][iter.deleted].emplace_back(iter);
				 break;
			case 5: 
 				 a[ktemp][iter.deletetime].emplace_back(iter);
				 break;

                }
            }       

    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<U>,bool>::type = true,typename std::enable_if<std::is_integral_v<U>,bool>::type = true>    
        std::map<T,std::map<U,std::vector<meta>>> getgroupRows([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::map<U,std::vector<meta>>> a;
   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);        
        T ktemp;
        
            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			 }

			 switch(vpos){
			case 0: 
 				 a[ktemp][iter.id].emplace_back(iter);
				 break;
			case 1: 
 				 a[ktemp][iter.parentid].emplace_back(iter);
				 break;
			case 2: 
 				 a[ktemp][iter.value_id].emplace_back(iter);
				 break;
			case 4: 
 				 a[ktemp][iter.deleted].emplace_back(iter);
				 break;
			case 5: 
 				 a[ktemp][iter.deletetime].emplace_back(iter);
				 break;

                }
            }       
    
            return a;
        }
    
        template<typename T,typename U,typename std::enable_if<std::is_integral_v<T>,bool>::type = true,typename std::enable_if<std::is_same<U,std::string>::value,bool>::type = true>    
        std::map<T,std::map<U,std::vector<meta>>> getgroupRows([[maybe_unused]] std::string keyname,[[maybe_unused]] std::string valname)
        {
            std::map<T,std::map<U,std::vector<meta>>> a;

   
            unsigned char kpos,vpos;
            kpos=findcolpos(keyname);
            vpos=findcolpos(valname);            
            T ktemp;
            
            for(auto &iter:record)
            {
                
                switch(kpos)
                {

   			case 0: 
 				 ktemp=iter.id;
				 break;
			case 1: 
 				 ktemp=iter.parentid;
				 break;
			case 2: 
 				 ktemp=iter.value_id;
				 break;
			case 4: 
 				 ktemp=iter.deleted;
				 break;
			case 5: 
 				 ktemp=iter.deletetime;
				 break;
			  }

			 switch(vpos){
			case 3: 
 				 a[ktemp][iter.content].emplace_back(iter);
				 break;

                }
            }       
    
            return a;
        }
    
  };
    
} 
}
#endif
   