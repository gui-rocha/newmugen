#include <string.h>
#include <string>
#include <sstream>
#include <vector>

static const int DEFAULT_DEBUG = 0;

#include <r-tech1/graphics/bitmap.h>

#include "factory/collector.h"
#include <r-tech1/network/network.h>
#include <r-tech1/token_exception.h>
#include <r-tech1/system.h>
#include <r-tech1/sound/music.h>
#include <r-tech1/menu/menu.h>
#include <r-tech1/menu/menu-exception.h>
#include <r-tech1/menu/optionfactory.h>
#include <r-tech1/input/input-manager.h>
#include <r-tech1/exceptions/shutdown_exception.h>
#include <r-tech1/exceptions/exception.h>
#include <r-tech1/exceptions/load_exception.h>
#include <r-tech1/timedifference.h>
#include <r-tech1/funcs.h>
#include <r-tech1/ftalleg.h>
#include <r-tech1/file-system.h>
#include <r-tech1/tokenreader.h>
#include <r-tech1/token.h>
#include <r-tech1/parameter.h>
#include <r-tech1/version.h>
#include <r-tech1/debug.h>
#include <r-tech1/configuration.h>
#include <r-tech1/init.h>
#include <r-tech1/main.h>
#include <r-tech1/argument.h>

#include "mugen/options.h"
#include "mugen/argument.h"

#include <iostream>

using std::vector;
using std::endl;
using std::string;
using std::istringstream;
using std::cout;

Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}

static void showOptions(const vector<Util::ReferenceCount<Argument::Parameter> > & arguments){
    Global::debug(0) << "Mugen by Jon Rafkind" << endl;
    Global::debug(0) << "Command line options" << endl;

    for (vector<Util::ReferenceCount<Argument::Parameter> >::const_iterator it = arguments.begin(); it != arguments.end(); it++){
        Util::ReferenceCount<Argument::Parameter> argument = *it;
        Global::debug(0) << " " << Util::join(argument->keywords(), ", ") << argument->description() << endl;
    }

    Global::debug(0) << endl;
}

class WindowedArgument: public Argument::Parameter {
public:
    WindowedArgument(Global::InitConditions * conditions):
    conditions(conditions){
    }

    Global::InitConditions * conditions;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-w");
        out.push_back("fullscreen");
        out.push_back("nowindowed");
        out.push_back("no-windowed");
        return out;
    }

    string description() const {
        return " : Start in fullscreen mode";
    }
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        conditions->graphics = Global::InitConditions::Fullscreen;
        return current;
    }
};

class SoundArgument: public Argument::Parameter {
public:
    SoundArgument(Global::InitConditions * conditions):
    conditions(conditions){
    }

    Global::InitConditions * conditions;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-s");
        out.push_back("nosound");
        return out;
    }

    string description() const {
        return " : Disable sound";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        conditions->sound = false;
        return current;
    }
};

class HelpArgument: public Argument::Parameter {
public:
    HelpArgument(const vector<Util::ReferenceCount<Argument::Parameter> > & arguments):
        arguments(arguments){
        }

    const vector<Util::ReferenceCount<Argument::Parameter> > & arguments;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("help");
        out.push_back("--help");
        return out;
    }
    
    string description() const {
        return " : Print the command line options and exit.";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        showOptions(arguments);
        exit(0);
    }
};

class VersionArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("version");
        out.push_back("--version");
        out.push_back("-v");
        return out;
    }
    
    string description() const {
        return " : Print the version of Paintown and exit.";
    }
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        Global::debug(0) << "Version " << Version::getVersionString() << endl;
        exit(0);
    }
};

class DataPathArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-d");
        out.push_back("--data");
        out.push_back("data");
        out.push_back("datapath");
        out.push_back("data-path");
        out.push_back("path");
        return out;
    }

    string description() const {
        std::ostringstream out;
        out << " <path> : Use data path of <path>. Default is " << Util::getDataPath2().path();
        return out.str();
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        current++;
        if (current != end){
            Util::setDataPath(*current);
        }
        return current;
    }
};

class DisableQuitArgument: public Argument::Parameter {
public:
    DisableQuitArgument(bool * quit):
    quit(quit){
    }

    bool * quit;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("disable-quit");
        return out;
    }
    
    string description() const {
        return " : Don't allow the game to exit using the normal methods";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        *quit = false;
        return current;
    }
};

class MusicArgument: public Argument::Parameter {
public:
    MusicArgument(bool * enabled):
        enabled(enabled){
        }

    bool * enabled;

    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-m");
        out.push_back("music");
        out.push_back("nomusic");
        out.push_back("no-music");
        return out;
    }

    string description() const {
        return " : Turn off music";
    }
    
    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        *enabled = false;
        return current;
    }
};

class DebugArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("-l");
        out.push_back("--debug");
        out.push_back("debug");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        current++;
        if (current != end){
            istringstream i(*current);
            int f;
            i >> f;
            Global::setDebug(f);
        }
        return current;
    }
    
    string description() const {
        return " # : Enable debug statements. Higher numbers gives more debugging. Default is 0. Negative numbers are allowed. Example: -l 3";
    }
};

class DebugFileArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("debug-log");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        current++;
        Global::logToFile();
        return current;
    }
    
    string description() const {
        return " # : Send debug output to a file.";
    }
};

class RateLimitArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("fps");
        out.push_back("rate-limit");
        return out;
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & actions){
        Global::rateLimit = false;
        return current;
    }

    string description() const {
        return " : Don't rate limit the game to the default fps (40). This is mainly useful for benchmarking graphics capabilities.";
    }
};

class JoystickArgument: public Argument::Parameter {
public:
    vector<string> keywords() const {
        vector<string> out;
        out.push_back("joystick");
        out.push_back("nojoystick");
        out.push_back("no-joystick");
        return out;
    }

    string description() const {
        return " : Disable joystick input";
    }

    vector<string>::iterator parse(vector<string>::iterator current, vector<string>::iterator end, Argument::ActionRefs & refs){
        Configuration::setJoystickEnabled(false);
        return current;
    }
};

static int startMain(const vector<Util::ReferenceCount<Argument::Action> > & actions, bool allow_quit){
    while (true){
        bool normal_quit = false;
        try{
            for (vector<Util::ReferenceCount<Argument::Action> >::const_iterator it = actions.begin(); it != actions.end(); it++){
                Util::ReferenceCount<Argument::Action> action = *it;
                action->act();
            }
            normal_quit = true;
        } catch (const Filesystem::Exception & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const LoadException & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const Exception::Return & ex){
        } catch (const ShutdownException & shutdown){
            Global::debug(1) << "Forced a shutdown. Cya!" << endl;
        } catch (const MugenException & m){
            Global::debug(0) << "Mugen exception: " << m.getReason() << endl;
        } catch (const ReloadMenuException & ex){
            Global::debug(1) << "Menu Reload Requested. Restarting...." << endl;
            continue;
        } catch (const ftalleg::Exception & ex){
            Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << endl;
        } catch (const Exception::Base & base){
            Global::debug(0) << "Base exception: " << base.getTrace() << endl;
        } catch (const std::bad_alloc & fail){
            Global::debug(0) << "Failed to allocate memory. Usage is " << System::memoryUsage() << endl;
        } catch (...){
            Global::debug(0) << "Uncaught exception!" << endl;
        }

        if (allow_quit && normal_quit){
            break;
        } else if (normal_quit && !allow_quit){
        } else if (!normal_quit){
            break;
        }
    }
    return 0;
}

class DefaultGame: public Argument::Action {
    public:
        void act(){
            Mugen::run();
        }
};

template <class X>
static void appendVector(vector<X> & to, const vector<X> & from){
    to.insert(to.end(), from.begin(), from.end());
}

/* Sort arguments based on the first letter of the keywords */
static bool argumentSorter(const Util::ReferenceCount<Argument::Parameter> & arg1, const Util::ReferenceCount<Argument::Parameter> & arg2){
    vector<string> keywords1 = arg1->keywords();
    vector<string> keywords2 = arg2->keywords();

    if (keywords1.size() > 0 && keywords2.size() > 0){
        /* Check first letter in the first string */
        return keywords1[0][0] < keywords2[0][0];
    }
    return false;
}

static void sortArguments(vector<Util::ReferenceCount<Argument::Parameter> > & arguments){
    sort(arguments.begin(), arguments.end(), argumentSorter);
}

static void setUpTouch(const Util::ReferenceCount<DeviceInput::Touch> & touch){
    int screenWidth = Graphics::Bitmap::getScreenWidth();
    int screenHeight = Graphics::Bitmap::getScreenHeight();
    int buttonSize = (int) (0.06 * sqrt(screenWidth * screenWidth + screenHeight * screenHeight));

    int center_x = buttonSize + buttonSize / 2;
    int center_y = screenHeight - buttonSize - buttonSize / 2;

    int x, y;

    x = center_x;
    y = center_y - buttonSize;

    touch->setZone(DeviceInput::Touch::Up, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = center_x;
    y = center_y + buttonSize;
    touch->setZone(DeviceInput::Touch::Down, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = center_x - buttonSize;
    y = center_y;
    touch->setZone(DeviceInput::Touch::Left, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x + buttonSize;
    y = center_y;
    touch->setZone(DeviceInput::Touch::Right, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    int spacing = (int) (buttonSize * 0.1);
    center_x = screenWidth - buttonSize * 2 - spacing;
    center_y = screenHeight - buttonSize - buttonSize / 2;

    x = center_x - buttonSize - buttonSize / 2;
    y = center_y - buttonSize / 2 - spacing;
    touch->setZone(DeviceInput::Touch::Button1, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x;
    y = center_y - buttonSize / 2 - spacing;
    touch->setZone(DeviceInput::Touch::Button2, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x + buttonSize + buttonSize / 2;
    y = center_y - buttonSize / 2 - spacing;
    touch->setZone(DeviceInput::Touch::Button3, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = center_x - buttonSize - buttonSize / 2;
    y = center_y + buttonSize / 2 + spacing;
    touch->setZone(DeviceInput::Touch::Button4, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x;
    y = center_y + buttonSize / 2 + spacing;
    touch->setZone(DeviceInput::Touch::Button5, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
    
    x = center_x + buttonSize + buttonSize / 2;
    y = center_y + buttonSize / 2 + spacing;
    touch->setZone(DeviceInput::Touch::Button6, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);

    x = screenWidth - buttonSize / 2 - spacing;
    y = buttonSize;
    touch->setZone(DeviceInput::Touch::Quit, x - buttonSize / 2, y - buttonSize / 2, x + buttonSize / 2, y + buttonSize / 2);
}

int rtech_main(int argc, char ** argv){
    Util::setDataPath(DATA_PATH);

    /* -1 means use whatever is in the configuration */
    Global::InitConditions conditions;

    bool music_on = true;
    // bool joystick_on = true;
    // bool mugen = false;
    // bool just_network_server = false;
    bool allow_quit = true;
    //Collector janitor;
    // NetworkJoin networkJoin;
    // MugenInstant mugenInstant;

    Version::setVersion(3, 6, 1);

    System::startMemoryUsage();

    Global::setDebug(DEFAULT_DEBUG);
    Global::setDefaultDebugContext("mugen");
    //vector<const char *> all_args;

    vector<string> stringArgs;
    for (int q = 1; q < argc; q++){
        stringArgs.push_back(argv[q]);
    }

    vector<Util::ReferenceCount<Argument::Parameter> > arguments;
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new WindowedArgument(&conditions)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DataPathArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new SoundArgument(&conditions)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new MusicArgument(&music_on)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DebugArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DebugFileArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new RateLimitArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new JoystickArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new VersionArgument()));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new DisableQuitArgument(&allow_quit)));
    arguments.push_back(Util::ReferenceCount<Argument::Parameter>(new HelpArgument(arguments)));

    appendVector(arguments, Mugen::arguments());

    sortArguments(arguments);

    vector<Util::ReferenceCount<Argument::Action> > actions;

    /* Sort of a hack but if we are already at the end of the argument list (because some
     * argument already reached the end) then we don't increase the argument iterator
     */
    for (vector<string>::iterator it = stringArgs.begin(); it != stringArgs.end(); (it != stringArgs.end()) ? it++ : it){
        for (vector<Util::ReferenceCount<Argument::Parameter> >::iterator arg = arguments.begin(); arg != arguments.end(); arg++){
            Util::ReferenceCount<Argument::Parameter> argument = *arg;
            if (argument->isArg(*it)){
                it = argument->parse(it, stringArgs.end(), actions);

                /* Only parse one argument */
                break;
            }
        }
    }

    Global::debug(0) << "Give 'help' to see all the command line options." << endl;
    //showOptions(arguments);

    Global::debug(0) << "Debug level: " << Global::getDebug() << endl;
    
    //Global::debug(0) << "Paintown version " << Version::getVersionString() << endl;
    
    if (! Global::init(conditions)){
        Global::debug(0) << "Could not initialize system" << endl;
        return -1;
    }

    if (! Global::dataCheck()){
        Global::debug(0) << "Could not find data" << std::endl;
        return -1;
    }

    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, Graphics::getScreenBuffer());
    
    InputManager input;
    Music music(music_on);

    setUpTouch(input.getTouch());

    /* If there are no actions then start the Mugen menu */
    if (actions.size() == 0){
        actions.push_back(Util::ReferenceCount<Argument::Action>(new DefaultGame()));
    }

    Util::Parameter<Util::ReferenceCount<Graphics::ShaderManager> > defaultShaderManager(Graphics::shaderManager, Util::ReferenceCount<Graphics::ShaderManager>(new Graphics::ShaderManager()));

    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/LiberationSans-Regular.ttf")));

    Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > defaultMenuFont(Menu::menuFontParameter, Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(*defaultFont.current(), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));

    startMain(actions, allow_quit);

    Configuration::saveConfiguration();

    Global::debug(0) << "Bye!" << endl;
    Global::closeLog();

    return 0;
}