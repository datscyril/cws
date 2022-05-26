#include "connection.hh"

namespace http
{
    void Connection::field_get(http::Request &req)
    {
        std::string line;
        std::string key;
        std::string value;

        req.fields_.erase(req.fields_.begin(), req.fields_.end());

        std::stringstream s(message_);
        std::getline(s, line);
        std::stringstream first(line);
        first >> req.method_ >> req.uri_ >> req.version_;

        while (std::getline(s, line))
        {
            if (line.size() == 1 && line[0] == '\r')
            {
                field_received_ = true;
                break;
            }

            std::stringstream l(line);
            key = "";
            value = "";
            std::getline(l, key, ':');
            std::getline(l, value);
            if (key.compare("") == 0 || value.compare("") == 0)
                break;
            else
            {
                while (value[0] == ' ' || value[0] == '\t')
                    value = value.substr(1, value.size());
                do
                {
                    value = value.substr(0, value.size() - 1);
                } while (value[value.size() - 1] == ' '
                         || value[value.size() - 1] == '\t');
                auto [it, rage] = req.fields_.insert(
                    std::pair<std::string, std::string>(key, value));
                if (key.compare("Content-Length") == 0 && !rage
                    && it->first.compare("Content-Length") == 0)
                {
                    req.mult_content_length = true;
                }
            }
        }

        if (!field_received_)
            return;

        auto it = req.fields_.find("Content-Length");
        if (it != req.fields_.end() && remaining_ == DEFAULT_REMAINING)
        {
            remaining_ = stoi(it->second);
        }

        while (std::getline(s, line) && remaining_ != DEFAULT_REMAINING)
        {
            req.body_ += line;
            remaining_ -= line.size();
        }
    }
} // namespace http
