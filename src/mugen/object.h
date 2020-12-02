#ifndef _paintown_mugen_object_h
#define _paintown_mugen_object_h

#include "animation.h"
#include "common.h"

namespace Graphics{
    class Bitmap;
}

namespace Mugen{

class Stage;
struct HitDefinition;

class Object{
private:
    Object();
public:
    Object(int alliance);
    Object(const Object & copy);

    virtual ~Object();

    /*
        */

    // virtual void unbind(Object * who) = 0;
    
    virtual double getHealth() const = 0;
        
    // virtual void drawReflection(Graphics::Bitmap * work, int rel_x, int rel_y, int intensity) = 0;
    virtual void drawMugenShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh) = 0;
    // virtual void draw(Graphics::Bitmap * work, int rel_x, int rel_y) = 0;
    
    // virtual void act(std::vector<Mugen::Object*>*, Stage*, std::vector<Mugen::Object*>*) = 0;
    
    virtual int getAlliance() const;
    virtual void setAlliance(int alliance);
    /*
        */
    virtual int getHeight() const = 0;
	
    /*
    virtual int getObjectId() const;
    virtual void setObjectId(int id);
    */

    /*
    virtual unsigned int getTicket() const;
    virtual void nextTicket();
    */
    
    // virtual void doMovement(const std::vector<Object*> & objects, Stage & stage);

    virtual int getSpritePriority() const = 0;

    /* called when this object is about to be destroyed. clean up any
     * lingering references into the stage
     */
    virtual void destroyed(Stage & stage) = 0;

    /* paused from an attack */
    virtual bool isPaused() const = 0;
    virtual double getGravity() const = 0;
    virtual void setYVelocity(double y) = 0;
    virtual double getYVelocity() const = 0;
    // virtual void changeState(Mugen::Stage & stage, int state, const std::vector<std::string> & inputs) = 0;
    virtual double getXVelocity() const = 0;
    virtual void setXVelocity(double x) = 0;
    virtual bool canTurn() const = 0;
    virtual void doTurn(Mugen::Stage & stage) = 0;
    virtual double getGroundFriction() const = 0;
    virtual const std::string & getMoveType() const = 0;
    virtual HitDefinition & getHit() = 0;
    virtual const HitDefinition & getHit() const = 0;
    virtual int getCurrentJuggle() const = 0;
    virtual bool isAttacking() const = 0;
    // virtual const std::vector<Area> getAttackBoxes() const = 0;
    // virtual const std::vector<Area> getDefenseBoxes() const = 0;
    virtual ResourceEffect getDefaultSpark() const = 0;
    virtual ResourceEffect getDefaultGuardSpark() const = 0;
    virtual int getAttackDistance() const = 0;
    virtual void guarded(Mugen::Stage & stage, Object * enemy, const HitDefinition & hit) = 0;
    virtual void addPower(double d) = 0;
    // virtual void wasHit(Mugen::Stage & stage, Object * enemy, const HitDefinition & hit) = 0;
    // virtual void didHit(Object * enemy, Mugen::Stage & stage) = 0;
    // virtual void didHitGuarded(Object * enemy, Mugen::Stage & stage) = 0;
protected:
    unsigned int attack_ticket;
    int alliance;
    // Facing facing;
    // int objectId;
    unsigned int ticket;
};

}

#endif