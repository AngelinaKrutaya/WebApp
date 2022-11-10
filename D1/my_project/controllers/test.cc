#include "test.h"

void test::CreateUser(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr&)>&&callback)
{
    // GET
    if (req->getMethod() == Get)
    {
        auto resp = HttpResponse::newHttpJsonResponse("json GET method from /sign-up");
        resp->setStatusCode(k200OK);

        callback(resp);
    }
    // POST
    else if (req->getMethod() == Post)
    {
        std::string bodyResp;
        std::string email;
        std::string password;
        JSONCPP_STRING err;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

        req->setCustomContentTypeString("application/json");
        auto body = req->getBody();
        std::string rawJson{body};
        auto rawJsonLength = static_cast<int>(rawJson.length());
        if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &err))
        {
            bodyResp = "Error parsing from /sign-up path";
            
            LOG_ERROR << bodyResp;

            auto resp = HttpResponse::newHttpJsonResponse(bodyResp);
            resp->setStatusCode(k400BadRequest);
            callback(resp);
        }

        email = root["email"].asString();
        password = root["password"].asString();

        if (email == "")
        {
            bodyResp = "email is required";
            auto resp = HttpResponse::newHttpJsonResponse(bodyResp);
            resp->setStatusCode(k406NotAcceptable);
            callback(resp);
        }
        
        else if (password == "")
        {
            bodyResp = "password is required";
            auto resp = HttpResponse::newHttpJsonResponse(bodyResp);
            resp->setStatusCode(k406NotAcceptable);
            callback(resp);
        }
        else
        {
            auto db = app().getDbClient();

            auto f = db->execSqlAsyncFuture("SELECT email FROM public.user WHERE email=$1;", email);

            try
            {
                auto result = f.get();

                if (result.size() >= 1) 
                {
                    bodyResp = email+" already in use";

                    auto resp = HttpResponse::newHttpJsonResponse(bodyResp);
                    resp->setStatusCode(k400BadRequest);
                    callback(resp);
                }
                else if (result.size() <= 0)
                {
                    bodyResp = email+" registered.";

                    
                    auto dtr = db->execSqlAsyncFuture("INSERT INTO public.user (email, password) VALUES($1, $2);", email, password);

                    try
                    {
                        auto result = dtr.get();
                        auto resp = HttpResponse::newHttpJsonResponse(bodyResp);
                        resp->setStatusCode(k200OK);
                        callback(resp);
                    }
                    catch(const orm::DrogonDbException &e)
                    {
                        auto e_ = e.base().what();
                        std::string error = e_;

                        bodyResp = "#2 test::CreateUser Error: "+error;

                        LOG_ERROR << bodyResp;

                        auto resp = HttpResponse::newHttpJsonResponse(bodyResp);
                        resp->setStatusCode(k500InternalServerError);
                        callback(resp);
                    }
                    
                }
            }
            catch(const orm::DrogonDbException &e)
            {
                auto e_ = e.base().what();
                std::string error = e_;

                bodyResp = "#1 test::CreateUser Error: "+error;

                LOG_ERROR << bodyResp;

                auto resp = HttpResponse::newHttpJsonResponse(bodyResp);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            }
        }
    }
}


void test::GetUser(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr&)>&&callback)
{
    Json::Value root;
    Json::Value data;

    uint32_t id;
    std::string email;
    std::string bodyResp;
    if (req->getMethod() == Get)
    {
        auto resp = HttpResponse::newHttpJsonResponse("json GET method from /get");
        resp->setStatusCode(k200OK);

        callback(resp);
    }
    auto db = drogon::app().getDbClient();

    auto f = db->execSqlAsyncFuture("SELECT * FROM public.user;");
    
    try
    {
        auto result = f.get();

        for (auto row : result)
        {
            auto id_ = row["id"].as<uint32_t>();
            auto email_ = row["email"].as<std::string>();

            data["id"] = id_;
            data["email"] = email_;

            root.append(data);
        }

        auto resp = HttpResponse::newHttpJsonResponse(root);
        resp->setStatusCode(k200OK);
        callback(resp);
    }
    catch(const orm::DrogonDbException &e)
    {
        auto e_ = e.base().what();
        std::string error = e_;
        
        bodyResp = "#1 test::GetUser Error: "+error;

        LOG_ERROR << bodyResp;

        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
    }
}




