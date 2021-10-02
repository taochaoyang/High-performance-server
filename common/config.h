#ifndef _CONFIG_H
#define _CONFIG_H


#include <map>
#include <memory>

// Read config information from the specified path through this object.
// Simple interest model.

class config {

public:
    static void init_config_ptr_();

    static std::shared_ptr<config> get_config_ptr();

    mutable std::map<std::string, std::string> config_;

    bool judge_config();

    ~config() = default;
    
protected:
    config();

private:

    // Storage config information in the object pointed to by this smart pointer.
    static std::shared_ptr<config> config_ptr_;
};


#endif