#pragma once
#include <set>

class FunctionDef;
struct Globals;

class FunctionKeys
    {
    public:
        static std::vector<std::string> trig;
        static std::vector<std::string> arithm;
        static std::vector<std::string> date;
        std::vector<std::string> all;
        FunctionKeys();
    };

enum class FunctionType { DATE, TRIG, ARITHM, ALL };

class FunctionsView
    {
    private:
        Globals& globals;
        std::set<std::string> defs;
        static FunctionKeys functionKeys;
        const std::vector<std::string>& getFuncKeyList(FunctionType type);
    public:
        FunctionsView(const FunctionsView&) = default;
        FunctionsView(Globals& globals);
        FunctionDef* get(const std::string& key);
        bool exists(const std::string& key);
        void addFunctions(FunctionType type);
        void removeFunctions(FunctionType type);
        void setDefs(const FunctionsView& uv) { this->defs = uv.defs; }
    };