#include <iostream>
#include <vector>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Array.h>

#include <iostream>
#include <fstream>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;

using Poco::Util::Application;
using Poco::Util::ServerApplication;


#define SERVER_PORT 8001
class TimeRequestHandler : public HTTPRequestHandler {
private:
	std::string _str;

    std::vector<Poco::JSON::Object> *_people;
    std::vector<Poco::JSON::Object> *_students;

public:

    //用于处理http请求
    TimeRequestHandler(std::string str
                        , std::vector<Poco::JSON::Object> *people
                        , std::vector<Poco::JSON::Object> *students)
                        : _str(str), _people(people), _students(students) {}

    void handleRequest(HTTPServerRequest &req, HTTPServerResponse &res) override {
        Application &app = Application::instance();

        std::ostream &ostr = res.send();
        res.setChunkedTransferEncoding(true);
        res.setContentType("text/json");

        // ostr << req.clientAddress().toString();

        std::string recv_string;
        Poco::StreamCopier::copyToString(req.stream(), recv_string);
        std::cout << _str << "-data-"<< recv_string << std::endl;

        Poco::JSON::Object sendJson;
        try {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(recv_string);

            if (_str == "/search") {
                Poco::JSON::Object::Ptr json_result = result.extract<Poco::JSON::Object::Ptr>();

                int min = json_result->getValue<int>("pos_min");
                int max = json_result->getValue<int>("pos_max");

                if (max > _people->size() || max == -1)
                    max = _people->size();

                Poco::JSON::Array list;
                for (int i = min; i < max; i++) {
                    Poco::JSON::Object obj;
                    obj.set("pos", i);
                    obj.set("name", (*_people)[i].getValue<std::string>("name"));
                    obj.set("age", (*_people)[i].getValue<int>("age"));
                    list.add(obj);
                }

                sendJson.set("status", "OK");
                sendJson.set("students", list);
            }
            
            if (_str == "/admit") {
                Poco::JSON::Object::Ptr json_result = result.extract<Poco::JSON::Object::Ptr>();

                Poco::JSON::Array::Ptr pos = json_result->getArray("pos");

                _students->clear();
                for(int i = 0; i < pos->size(); i++) {
                    int index = pos->get(i);
                    Poco::JSON::Object obj = (*_people)[index];

                    _students->push_back(obj);
                }

                sendJson.set("status", "OK");
            }

            if (_str == "/list") {
                Poco::JSON::Object::Ptr json_result = result.extract<Poco::JSON::Object::Ptr>();
                Poco::JSON::Array list;
                for(int i = 0; i < _students->size(); i++) {
                    Poco::JSON::Object obj;
                    obj.set("pos", i);
                    obj.set("name", (*_students)[i].getValue<std::string>("name"));
                    obj.set("age", (*_students)[i].getValue<int>("age"));
                    list.add(obj);
                }

                sendJson.set("status", "OK");
                sendJson.set("students", list);
            }

            if (_str == "/course") {
                Poco::JSON::Object::Ptr json_result = result.extract<Poco::JSON::Object::Ptr>();

                Poco::JSON::Array::Ptr pos = json_result->getArray("pos");
                std::string course = json_result->getValue<std::string>("course");

                std::cout << "course: " << course << std::endl;

                for(int i = 0; i < pos->size(); i++) {
                    int index = pos->get(i);
                    std::cout << "stu: " << (*_students)[index].getValue<std::string>("name") << std::endl;
                }

                sendJson.set("status", "OK");
            }

            if (sendJson.size() == 0)
                sendJson.set("error", "param not equal");
        } catch (Poco::Exception &e) {
            std::cout << e.displayText() << std::endl;
            sendJson.set("error", "json invalid");
        }

        std::ostringstream jsonStream;
        sendJson.stringify(jsonStream);
        std::string str = jsonStream.str();
        ostr << str;
    }
};

class TimeRequestHandlerFactory : public HTTPRequestHandlerFactory {
private:
    int _loadFromFile(std::string filepath)
    {
        std::ifstream savefile(filepath, std::ios::in);
        if (!savefile.is_open())
        {
            std::cout << "read file err: " << filepath << std::endl;
            return 1;
        }
        std::string jsonstr, line;
        while (std::getline(savefile, line))
            jsonstr += line;
        savefile.close();

        try
        {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(jsonstr);
            Poco::JSON::Array::Ptr jsonobj = result.extract<Poco::JSON::Array::Ptr>();
            
            //std::cout << "size: " << jsonobj->size() << std::endl;
            for (int i = 0; i < jsonobj->size(); i++) {
                Poco::JSON::Object::Ptr obj = jsonobj->getObject(i);
                people.push_back(*obj);
            }
        }
        catch (Poco::Exception &ex)
        {
            std::cout << __func__ << ": " << ex.displayText() << std::endl;
            return 1;
        }
        return 0;
    }

public:
    
    std::vector<Poco::JSON::Object> people;
    std::vector<Poco::JSON::Object> students;


    TimeRequestHandlerFactory(std::string filepath) {
        //用来读入json格式的学生名单
        _loadFromFile(filepath);
    }

    HTTPRequestHandler *
    createRequestHandler(const HTTPServerRequest &req) override {
        return new TimeRequestHandler(req.getURI(), &people, &students);
    }
};

class HTTPTimeServer : public ServerApplication {
protected:
    int main(const std::vector<std::string> &args) override {

        //启动HTTP服务器
        ServerSocket svs(SERVER_PORT);
        HTTPServer srv(new TimeRequestHandlerFactory(args[0]), svs,
                        new HTTPServerParams);
        srv.start();
        waitForTerminationRequest();
        srv.stop();

        return Application::EXIT_OK;
    }
};

int main(int argc, char **argv) {
    HTTPTimeServer app;
    return app.run(argc, argv);
}

