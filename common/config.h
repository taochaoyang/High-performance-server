#include <map>
#include <memory>

class config {

public:
    static void init_config_ptr_();

    static std::shared_ptr<config> get_config_ptr();

    mutable std::map<std::string, std::string> config_;

protected:
    config();

private:
    static std::shared_ptr<config> config_ptr_;
};