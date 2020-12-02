#ifndef _paintown_ast_hitdef_h_
#define _paintown_ast_hitdef_h_

#include "Value.h"
#include <string>
#include <sstream>

namespace Ast{

class HitDefAttribute: public Value {
public:
    HitDefAttribute(int line, int column, const std::string & value):
    Value(line, column),
    value(value){
    }

    class HitDefAttributeView: public ViewImplementation {
    public:
        HitDefAttributeView(const HitDefAttribute * owner):
        owner(owner){
        }

        const HitDefAttribute * owner;

        virtual std::string getType() const {
            return owner->getType();
        }
        
        virtual const Value * self() const {
            return owner;
        }
        
        virtual std::string toString() const {
            return owner->toString();
        }

        using ViewImplementation::operator>>;
        virtual HitDefAttributeView & operator>>(std::string & str){
            str = owner->value;
            return *this;
        }
    };

    using Value::view;
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new HitDefAttributeView(this)));
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const HitDefAttribute & him) const {
        return value == him.value;
    }

    virtual void walk(Walker & walker) const {
        walker.onHitDefAttribute(*this);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_HITDEF_ATTRIBUTE << getLine() << getColumn() << value;
        return token;
    }

    static HitDefAttribute * deserialize(const Token * token){
        std::string value;
        int line, column;
        token->view() >> line >> column >> value;
        return new HitDefAttribute(line, column, value);
    }
    
    virtual Element * copy() const {
        return new HitDefAttribute(getLine(), getColumn(), value);
    }
          
    virtual std::string getType() const {
        return "hitdef attribute";
    }

    virtual std::string toString() const {
        return value;
    }

    virtual ~HitDefAttribute(){
    }

protected:
    std::string value;
};

class HitDefAttackAttribute: public Value {
public:
    HitDefAttackAttribute(int line, int column):
    Value(line, column){
    }

    class HitDefAttackAttributeView: public ViewImplementation {
    public:
        HitDefAttackAttributeView(const HitDefAttackAttribute * owner):
        owner(owner){
            current = owner->values.begin();
        }

        const HitDefAttackAttribute * owner;
        std::vector<std::string>::const_iterator current;

        virtual std::string getType() const {
            return owner->getType();
        }
        
        virtual const Value * self() const {
            return owner;
        }
        
        virtual std::string toString() const {
            return owner->toString();
        }

        using ViewImplementation::operator>>;
        virtual HitDefAttackAttributeView & operator>>(std::string & str){
            if (current != owner->values.end()){
                str = *current;
                current++;
            } else {
                throw Exception("No more values in this hitdef attack attribute: " + owner->toString());
            }
            return *this;
        }
    };

    using Value::view;
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new HitDefAttackAttributeView(this)));
    }

    virtual void walk(Walker & walker) const {
        walker.onHitDefAttackAttribute(*this);
    }

    virtual void addAttribute(const std::string & value){
        values.push_back(value);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_HITDEF_ATTACK_ATTRIBUTE << getLine() << getColumn();
        for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++){
            *token << *it;
        }
        return token;
    }

    static HitDefAttackAttribute * deserialize(const Token * token){
        HitDefAttackAttribute * attribute = NULL;
        try{
            TokenView view = token->view();
            int line, column;
            view >> line >> column;
            attribute = new HitDefAttackAttribute(line, column);
            while (true){
                std::string value;
                view >> value;
                attribute->addAttribute(value);
            }
        } catch (const TokenException & e){
        }
        return attribute;
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const HitDefAttackAttribute & him) const {
        std::vector<std::string>::const_iterator my_it = values.begin();
        std::vector<std::string>::const_iterator him_it = him.values.begin();
        while (true){
            if (my_it == values.end() || him_it == him.values.end()){
                break;
            }

            if (*my_it != *him_it){
                return false;
            }

            my_it++;
            him_it++;
        }

        return my_it == values.end() && him_it == him.values.end();
    }
    
    virtual Element * copy() const {
        HitDefAttackAttribute * attribute = new HitDefAttackAttribute(getLine(), getColumn());
        for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++){
            attribute->addAttribute(*it);
        }
        return attribute;
    }
          
    virtual std::string getType() const {
        return "hitdef attack attribute";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        bool first = true;
        for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++){
            if (!first){
                out << ", ";
            }
            out << *it;
            first = false;
        }
        return out.str();
    }

    virtual ~HitDefAttackAttribute(){
    }

protected:
    std::vector<std::string> values;
};

}

#endif
