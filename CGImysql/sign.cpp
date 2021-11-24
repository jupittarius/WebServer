#include<iostream>
#include<string>
#include<string.h>
#include<cstdio>
#include<map>

#include<mysql/mysql.h>
#include"sql_connection_pool.h"

using namespace std;

int main(int argc, char *argv[]){
    map<string, string> users;

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    //初始化数据库连接池，连接池为静态大小
    //通过主机地址和登录账号，密码进入服务器数据库，选择 * 库
    connection_pool* connPool = connection_pool::GetInstance("localhost", "root", "pwd", "dbname", 3306, 5);

    //在连接池中取一个连接
    MYSQL* mysql = connPool->GetConnection();
    //在user表中检索username，passwd数据，浏览器端输入
    if(mysql_query(mysql, "select username, passwd from user")){
        cout << "mysql_query error: "<< mysql_error(mysql) << endl;
        return -1;
    }
    //从表中检索完整的结果集
    MYSQL_RES* result = mysql_store_result(mysql);
    //返回结果集中的列数
    int num_fields = mysql_num_fields(result);
    //返回所有字段结构的数组
    MYSQL_FIELD* fields = mysql_fetch_fields(result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while(MYSQL_ROW row = mysql_fetch_row(result)){
        string username = row[0];
        string passwd = row[1];
        users[username] = passwd;
    }

    string name(argv[1]);
    const char* namep = name.c_str();
    string passwd(argv[2]);
    const char* passwdp = passwd.c_str();
    char flag = *argv[0];

    //如果是注册，先检测数据库中是否有重名的
    //没有重名的，进行增加数据
    char* sql_insert = (char*) malloc(sizeof(char) * 200);
    strcpy(sql_insert, "insert into user(username, passwd) values(");
    strcat(sql_insert, "'");
    strcat(sql_insert, namep);
    strcat(sql_insert, "', '");
    strcat(sql_insert, passwdp);
    strcat(sql_insert, "')");

    if(flag == '3'){
        if(users.find(name) == users.end()){
            pthread_mutex_lock(&lock);
            int res = mysql_query(mysql, sql_insert);
            pthread_mutex_unlock(&lock);

            if(!res){
                printf("1\n");
            } else {
                printf("0\n");
            }
        }
        else
        {
            printf("0\n");
        }
        
    }

    //如果是登录，直接判断
    //若浏览器端输入的用户名和密码在表中可以查找到，返回1，否则返回0
    else if(flag == '2'){
        if(users.find(name) != users.end() && users[name] == passwd){
            printf("1\n");
        }
        else
        {
            printf("0\n");
        }
    }
    else printf("0\n");
    //释放结果集使用的内存
    mysql_free_result(result);

    //关闭连接
    connPool->DestroyPool();

}