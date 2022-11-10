#pragma once
#include <main.h>


class test : public drogon::HttpController<test>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(test::CreateUser, "/sign-up", Get, Post);
    ADD_METHOD_TO(test::GetUser, "/get/user", Get);

    METHOD_LIST_END
    void CreateUser(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr&)>&&callback);


    void GetUser(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr&)>&&callback);


};
