#pragma once
#include <vector>
#include "Value.h"
#include "Unit.h"
#include <string>
#include <map>
#include <memory>
#include "CodeBlock.h"

struct Globals;
class FunctionsView;
class FunctionDef
    {
    public:
        Globals& globals;
        std::string name;
        size_t minArgs;
        size_t maxArgs;

        FunctionDef(Globals& globals, const std::string& name, size_t minArgs, size_t maxArgs);

        bool isCorrectArgCount(size_t argCnt);
        Value call(std::vector<Value>& args, const Range& range);
    private:
        virtual Value execute(std::vector<Value>& args, const Range& range) = 0;
    };

class FunctionDefs
    {
    protected:
        std::map<std::string, FunctionDef*> functions;
        Globals& globals;

    public:
        FunctionDefs(Globals& globals) : globals(globals) { init(); }
        void init();
        bool exists(const std::string& functionName);
        void Add(FunctionDef* f);
        FunctionDef* get(const std::string& name);
        friend class FunctionsView;
    };

class Statement;
class OperatorDefs;
class FunctionDefExpr;
class CustomFunction: public FunctionDef
    {
    private:
        FunctionDefExpr& functionDefExpr;

    public:
        CodeBlock codeBlock;
        CustomFunction(FunctionDefExpr& functionDefExpr, CodeBlock&& codeBlock);
        Value execute(std::vector<Value>& args, const Range& range) override;
    };


class FunctionKeys
    {
    public:
        static std::vector<std::string> trig;
        static std::vector<std::string> arithm;
        static std::vector<std::string> date;
        std::vector<std::string> all;
        FunctionKeys();
    };

struct Globals;
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
        bool exists(const std::string& key) { return defs.count(key) != 0; }
        void addFunctions(FunctionType type);
        void removeFunctions(FunctionType type);
        void setDefs(const FunctionsView& uv) { this->defs = uv.defs; }
    };