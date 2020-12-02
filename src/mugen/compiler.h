#ifndef _paintown_mugen_compiler_h
#define _paintown_mugen_compiler_h

#include <string>
#include <vector>
#include "common.h"

namespace Ast{
    class Value;
}

namespace Mugen{

namespace Compiler{
    class Value;
}

class Character;
class Stage;

struct Range{
    Range():
    low(0), high(0){
    }

    Range(int low, int high):
    low(low), high(high){
    }

    int low, high;
};

struct RuntimeValue{
private:
    explicit RuntimeValue(Compiler::Value * value);
    explicit RuntimeValue(const char * cstring);
public:
    enum Type{
        Invalid,
        Bool,
        String,
        Double,
        ListOfString,
        RangeType,
        StateType,
        AttackAttribute,
        ListOfInt
    };

    struct StateTypes{
        StateTypes():
            standing(false),
            crouching(false),
            lying(false),
            aerial(false){
            }

        bool standing;
        bool crouching;
        bool lying;
        bool aerial;
    } attribute;

    RuntimeValue():
    type(Invalid),
    bool_value(false),
    double_value(0){
    }

    explicit RuntimeValue(bool b):
    type(Bool),
    bool_value(b),
    double_value(0){
    }

    explicit RuntimeValue(double d):
    type(Double),
    bool_value(false),
    double_value(d){
    }

    explicit RuntimeValue(int i):
    type(Double),
    bool_value(false),
    double_value(i){
    }

    RuntimeValue(const std::string & str):
    type(String),
    string_value(str),
    bool_value(false),
    double_value(0){
    }

    RuntimeValue(const StateTypes & attribute):
    attribute(attribute),
    type(StateType),
    bool_value(false),
    double_value(0){
    }

    RuntimeValue(const std::vector<AttackType::Attribute> & attributes):
    type(AttackAttribute),
    bool_value(false),
    double_value(0),
    attackAttributes(attributes){
    }

    RuntimeValue(const std::vector<std::string> & strings):
    type(ListOfString),
    bool_value(false),
    double_value(0),
    strings_value(strings){
    }

    RuntimeValue(const std::vector<int> & values):
    type(ListOfInt),
    bool_value(false),
    double_value(0),
    ints_value(values){
    }

    RuntimeValue(int low, int high):
    type(RangeType),
    bool_value(false),
    double_value(0),
    range(low, high){
    }

    /* Copy constructor will copy all fields, which is the right thing to do */

    bool operator==(const RuntimeValue & other) const;
    bool operator!=(const RuntimeValue & other) const;
    bool operator<=(const RuntimeValue & other) const;
    bool operator<(const RuntimeValue & other) const;
    bool operator>(const RuntimeValue & other) const;
    bool operator>=(const RuntimeValue & other) const;

    RuntimeValue operator+(const RuntimeValue & other) const;

    inline bool isBool() const {
        return type == Bool;
    }
    
    inline bool isDouble() const {
        return type == Double;
    }
    
    inline bool isString() const {
        return type == String;
    }

    inline bool isRange() const {
        return type == RangeType;
    }

    inline bool getBoolValue() const {
        return bool_value;
    }
    
    inline std::string getStringValue() const {
        return string_value;
    }

    inline double getDoubleValue() const {
        return double_value;
    }

    inline int getRangeLow() const {
        return range.low;
    }
    
    inline int getRangeHigh() const {
        return range.high;
    }

    double toNumber() const;
    bool toBool() const;

    const char * canonicalName() const {
        switch (type){
            case Invalid : return "invalid";
            case Bool : return "bool";
            case String : return "string";
            case Double : return "double";
            case ListOfString : return "list of string";
            case RangeType : return "range";
            case StateType : return "state type";
            case AttackAttribute : return "attack type";
            default : return "???";
        }
    }

    Type getType() const {
        return type;
    }

    Type type;
    std::string string_value;
    bool bool_value;
    double double_value;
    std::vector<std::string> strings_value;
    std::vector<AttackType::Attribute> attackAttributes;
    std::vector<int> ints_value;
    Range range;
};

class Environment{
public:
    Environment(){
    }

    virtual const Character & getCharacter() const = 0;
    virtual const Mugen::Stage & getStage() const = 0;
    virtual const std::vector<std::string> getCommands() const = 0;

    virtual RuntimeValue getArg1() const = 0;

    virtual ~Environment(){
    }
};

class EmptyEnvironment: public Environment {
public:
    EmptyEnvironment(){
    }

    virtual ~EmptyEnvironment(){
    }

    virtual const Character & getCharacter() const;
    virtual const Mugen::Stage & getStage() const;
    virtual const std::vector<std::string> getCommands() const;
    virtual RuntimeValue getArg1() const;
};

class FullEnvironment: public Environment {
public:
    FullEnvironment(const Mugen::Stage & stage, const Character & character, const std::vector<std::string> commands):
    stage(stage),
    character(character),
    commands(commands){
    }

    FullEnvironment(const Mugen::Stage & stage, const Character & character, const std::vector<std::string> commands, const RuntimeValue & arg1):
    stage(stage),
    character(character),
    commands(commands),
    arg1(arg1){
    }

    FullEnvironment(const Mugen::Stage & stage, const Character & character):
    stage(stage),
    character(character){
    }

    /*
    FullEnvironment(const FullEnvironment & copy):
    stage(copy.stage),
    character(copy.character),
    commands(copy.commands){
    }
    */

    virtual ~FullEnvironment(){
    }
    
    virtual inline RuntimeValue getArg1() const {
        return arg1;
    }

    virtual inline const Character & getCharacter() const {
        return character;
    }
    
    virtual inline const Mugen::Stage & getStage() const {
	return stage;
    }

    virtual inline const std::vector<std::string> getCommands() const {
        return commands;
    }

protected:
    const Mugen::Stage & stage;
    const Character & character;
    std::vector<std::string> commands;
    RuntimeValue arg1;
};

double toNumber(const RuntimeValue & value);
bool toBool(const RuntimeValue & value);
    
/*
RuntimeValue evaluate(const Ast::Value * value, const Environment & environment);

*/

}

namespace Ast{
    class Value;
}

namespace Mugen{

namespace Compiler{

    class Value{
    public:
        Value();
        virtual RuntimeValue evaluate(const Environment & environment) const = 0;
        virtual std::string toString() const;
        virtual Value * copy() const = 0;
        virtual ~Value();
    };

    /* deletes `input' */
    Value * compileAndDelete(const Ast::Value * input);

    /* does not delete `input' */
    Value * compile(const Ast::Value * input);
    Value * compile(const Ast::Value & input);
    Value * compile(int immediate);
    Value * compile(double immediate);
    Value * copy(Value * value);
}

}

#endif