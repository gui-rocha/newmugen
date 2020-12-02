import os
import scons.utils
import scons.checks

SetOption('num_jobs', scons.utils.detectCPUs())

def isQuiet():
    import sys
    return '-s' in sys.argv

def isPlatform(platform):
    import sys
    return platform in sys.platform

def isWindows():
    return isPlatform("win32")

def isOSX104():
    import platform
    return isPlatform("darwin") and platform.processor() == 'powerpc'

def isOSX():
    return isPlatform("darwin") and not isOSX104()

def file_exists(path):
    return os.path.exists(path)

def makeUseEnvironment(key, default):
    def use():
        import os
        try:
            return int(os.environ[key]) == 1
        except KeyError:
            return default
    return use

def makeUseArgument(key, default):
    def use():
        try:
            return int(ARGUMENTS[key]) == 1
        except KeyError:
            return default
    return use

usePrx = makeUseEnvironment('prx', False)
isVerbose = makeUseArgument('verbose', False)
useIntel = makeUseEnvironment('intel', False)
useMingwCross = makeUseEnvironment('mingwx', False)
nativeCompile = makeUseEnvironment('native', False)
enableProfiled = makeUseEnvironment('PROFILE', False)
showTiming = makeUseEnvironment('timing', False)

def checkAllegro(context):
    context.Message("Checking for Allegro 4... ")

    def testAllegro(context):
        return context.TryLink("""
            #include <allegro.h>
            int main(int argc, char ** argv){
              install_allegro(0, NULL, NULL);
              return 0;
            }
            END_OF_MAIN()
        """, ".c")

    # use pkg-config
    def allegro44(context):
        tmp = context.env.Clone()
        env = context.env
        ok = 1
        try:
            scons.utils.safeParseConfig(env, 'pkg-config allegro --cflags --libs')
            env.Append(CPPDEFINES = ['USE_ALLEGRO'])
            ok = testAllegro(context)
        except OSError:
            ok = 0 

        if not ok:
            context.sconf.env = tmp
        else:
            context.Message('found 4.4')

        return ok

    # use allegro-config
    def allegro42(context):
        tmp = context.env.Clone()
        env = context.env
        ok = 1
        try:
            def enableAllegro(env2):
                scons.utils.safeParseConfig(env2, 'allegro-config --cflags --libs')
                env2.Append(CPPDEFINES = ['USE_ALLEGRO'])
            scons.utils.safeParseConfig(env, 'allegro-config --cflags --libs')
            env['paintown_enableAllegro'] = enableAllegro
            env.Append(CPPDEFINES = ['USE_ALLEGRO'])
            ok = testAllegro(context)
        except OSError:
            ok = 0 

        if not ok:
            context.sconf.env = tmp
        else:
            context.Message('found 4.2')

        return ok

    ok = allegro44(context) or allegro42(context)

    context.Result(scons.utils.colorResult(ok))
    return ok

# find freetype in windows since we dont have freetype-config
def checkWindowsFreeType(context):
    context.Message("Checking for Freetype... ")
    def build():
        return context.TryCompile("""
            #include <ft2build.h>
            #include FT_FREETYPE_H
            int main(int argc, char ** argv){
                int major = FREETYPE_MAJOR; 
                return 0;
            }
            """, ".c")
    if not build():
        if useMingw():
            import os
            tmp = context.env.Clone()
            mingw = os.environ['MINGDIR']
            context.env.Append(CPPPATH = ["%s/include/freetype2" % mingw])
            if not build():
                context.env = tmp
                context.Result(scons.utils.colorResult(0))
                return 0
        else:
            context.Message("don't know how to find freetype for a non-mingw compiler")
            context.Result(scons.utils.colorResult(0))
            return 0

    context.Result(scons.utils.colorResult(1))
    return 1

def isCygwin():
    try:
        return os.environ['CYGWIN'] == '1'
    except:
        return False    

def useMingw():
    try:
        return "mingw" in ARGUMENTS[ 'env' ]
    except KeyError:
        return False

def readExec( program ):
    try:
        return os.popen(program).readline().replace("\n",'')
    except OSError:
        return ""

def useDistcc():
    try:
        return int(os.environ['USE_DISTCC']) == 1
    except KeyError:
        return False
        
def getDebug():
    try:
        return int(os.environ['DEBUG'])
    except KeyError:
        return 0

def less_verbose(env):
    link_color = 'light-red'
    ar_color = 'yellow'
    ranlib_color = 'light-purple'
    peg_color = 'light-cyan'
    env['CCCOMSTR'] = "%s %s" % (scons.utils.colorize('Compiling c file', 'light-green'), scons.utils.colorize('$SOURCE', 'light-blue'))
    env['SHCCCOMSTR'] = "%s %s" % (scons.utils.colorize('Compiling c file', 'light-green'), scons.utils.colorize('$SOURCE', 'light-blue'))
    env['CXXCOMSTR'] = "%s %s" % (scons.utils.colorize('Compiling c++ file', 'light-green'), scons.utils.colorize('$SOURCE', 'light-blue'))
    env['SHCXXCOMSTR'] = "%s %s" % (scons.utils.colorize('Compiling c++ file', 'light-green'), scons.utils.colorize('$SOURCE', 'light-blue'))
    env['LINKCOMSTR'] = "%s %s" % (scons.utils.colorize('Linking', link_color), scons.utils.colorize('$TARGET', 'light-blue'))
    env['SHLINKCOMSTR'] = "%s %s" % (scons.utils.colorize('Linking', link_color), scons.utils.colorize('$TARGET', 'light-blue'))
    env['ARCOMSTR'] = "%s %s" % (scons.utils.colorize('Building library', ar_color), scons.utils.colorize('$TARGET', 'light-blue'))
    env['RANLIBCOMSTR'] = "%s %s" % (scons.utils.colorize('Indexing library', ranlib_color), scons.utils.colorize('$TARGET', 'light-blue'))
    env['PEG_MAKE'] = "%s %s" % (scons.utils.colorize('Creating peg parser', peg_color), scons.utils.colorize('$TARGET', 'light-blue'))
    return env

def getEnvironment(debug):
    def intel(env):
        env['CC'] = 'icc'
        env['CXX'] = 'icpc'
        # '-Werror-all', '-Weffc++'
        # TODO: replace these flags with descriptive names
        env.Append(CCFLAGS = ['-wd981', '-wd2259'],
                   CXXFLAGS = ['-wd981', '-wd271',
                               '-wd383', '-wd869',
                               '-wd1599'])
        return env

    def mingwCross(env):
        import os
        print "Environment is Mingw Cross Compiler"
        prefix = 'i686-w64-mingw32-'
        def setup(pre, x):
            return '%s%s' % (pre, x)
        env['CC'] = setup(prefix, 'gcc')
        env['LD'] = setup(prefix, 'ld')
        env['CXX'] = setup(prefix, 'g++')
        env['AS'] = setup(prefix, 'as')
        env['AR'] = setup(prefix, 'ar')
        env['RANLIB'] = setup(prefix, 'ranlib')
        env['OBJCOPY'] = setup(prefix, 'objcopy')
        # Probably don't want to hard code all these libraries here..
        env.Append(LIBS = Split("wsock32 vorbis vorbisfile ogg glu32 winmm psapi shlwapi opengl32 gdi32 ole32 jpeg png15"))
        env['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES -Wl,--start-group $ARCHIVES $_LIBDIRFLAGS $_LIBFLAGS -Wl,--end-group -o $TARGET'
        env.Append(CPPPATH = ['/opt/mingw/include'])
        env.Append(LIBPATH = ['/opt/mingw/lib'])
        env.Append(LINKFLAGS = ['-static-libstdc++', '-static-libgcc', '-mwindows', '-Wl,-subsystem,windows'])
        env.PrependENVPath('PKG_CONFIG_PATH', '/opt/mingw/lib/pkgconfig')
        os.environ['ALLEGRO5'] = '1'
        return env
    
    def gcw(env):
        root = '/opt/gcw0-toolchain'
        env.PrependENVPath('PATH', '%s/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/bin' % root)
        env.PrependENVPath('PKG_CONFIG_PATH', '%s/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/lib/pkgconfig' % root)
        env.Append(CPPPATH = '%s/usr/mipsel-gcw0-linux-uclibc/sysroot/usr/include' % root)
        env.PrependENVPath('PATH', '%s/usr/bin' % root)
        env.Append(CPPDEFINES = ['UCLIBC', 'GCW0', '_FILE_OFFSET_BITS=64'])
        env['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES -Wl,--start-group $ARCHIVES $_LIBDIRFLAGS $_LIBFLAGS -Wl,--end-group -o $TARGET'
        def setup(x):
            return 'mipsel-linux-%s' % x
        env['CC'] = setup('gcc')
        env['CXX'] = setup('g++')
        env['AR'] = setup('ar')
        env['LD'] = setup('ld')
        return env

    def gcc(env):
        try:
            env['CC'] = os.environ['CC']
        except:
            pass
        try:
            env['CXX'] = os.environ['CXX']
        except:
            pass

        if useDistcc():
            env['CC'] = 'distcc'
            env['CXX'] = 'distcc g++'
        if isOSX():
            env.Append(CCFLAGS = Split("""-arch i386 -arch x86_64"""))
        if isOSX104():
            env['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES $_FRAMEWORKS -Wl,-all_load $_LIBDIRFLAGS $_LIBFLAGS $ARCHIVES -o $TARGET'
        else:
            env['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES -Wl,--start-group $ARCHIVES -Wl,--end-group $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'
        return env

    def llvm(env):
        env['CXX'] = 'clang++'
        env['CC'] = 'clang'
        env['LINKCOM'] = '$CXX $LINKFLAGS $SOURCES -Wl,--start-group $ARCHIVES -Wl,--end-group $_LIBDIRFLAGS $_LIBFLAGS -o $TARGET'

        if getDebug() and False:
            sanitize_flags = ['-fsanitize=undefined', '-fsanitize=integer']
            env.Append(CCFLAGS = sanitize_flags)
            env.Append(CXXFLAGS = sanitize_flags)
            env.Append(LINKFLAGS = sanitize_flags)

        return env

    def raw():
        defines = []
        if isOSX() or isOSX104():
            defines.append('MACOSX')
        cflags = []
        if debug:
            defines.append('DEBUG')
            
            cflags.extend(['-g3', '-O0', '-Wfatal-errors'])
        else:
            
            if nativeCompile():
                cflags.extend(['-O2', '-g', '-pipe', '-march=native'])
                if not enableProfiled():
                    cflags.extend(['-fomit-frame-pointer'])
            else:
                cflags.extend(['-O2'])

        if isCygwin():
            import SCons.Tool.zip
            env = Environment(ENV = os.environ, tools = ['mingw'])
            env['CXX'] = 'C:\\MinGW\\bin\\g++.exe'
            env['CC'] = 'C:\\MinGW\\bin\\gcc.exe'
            env['AR'] = 'C:\\MinGW\\bin\\ar.exe'
            SCons.Tool.zip.generate(env)
            return env
        elif useMingw():
            return gcc(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags, tools = ['mingw', 'zip']))
        else:
            if useIntel():
                print "Using the intel compiler"
                return intel(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            elif useMingwCross():
                return mingwCross(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
            else:
                return gcc(Environment(ENV = os.environ, CPPDEFINES = defines, CCFLAGS = cflags))
    
    def add_peg(env):
        env['PEG_MAKE'] = 'Creating peg parser $TARGET'
        return env
    if not debug and not isVerbose():
        return less_verbose(raw())
    else:
        return add_peg(raw())
        
if isWindows():
    print "Try 'scons env=mingw' if you want to use mingw's gcc instead of visual studio or borland"
    if not isCygwin():
        print "Cygwin not detected. If are you using cygwin set"
        print "export CYGWIN=1"
    else:
        print "Cygwin detected"

def peg_to_cpp(target, source, env):
    import sys
    sys.path.append("src/mugen/parser")
    sys.path.append(".")
    import peg, re, cpp_generator
    name = source[0].name
    parser = peg.make_peg_parser(re.sub('\..*', '', name))
    fout = open(target[0].path, 'w')
    fout.write(cpp_generator.generate(parser(source[0].path)))
    fout.write('\n')
    fout.close()

def peg_to_cpp_multi(target, source, env):
    import sys
    sys.path.append("src/mugen/parser")
    sys.path.append(".")
    import peg, re, os
    name = re.sub('\..*', '', source[0].name)
    parser = peg.make_peg_parser(name)
    fout = open(target[0].path, 'w')
    directory = os.path.dirname(target[0].path)
    fout.write(parser(source[0].path).generate_cpp(separate = name, directory = directory))
    fout.write('\n')
    fout.close()

def peg_emitter(target, source, env):
    import sys
    import peg, re, os
    
    name = re.sub('\..*', '', str(os.path.basename(str(source[0].path))))
    directory = os.path.dirname(str(target[0].path))
    parser = peg.make_peg_parser(name)
    where = '/tmp/scanner.tmp'
    stuff = open(where, 'w')
    stuff.write(source[0].get_contents())
    stuff.close()
    final = parser(where)
    
    files = final.list_files(name)
    
    def isHeader(file):
        return '.h' in file
    return (target + [file for file in files if not isHeader(file)], source)

import sys
sys.path.append('src/mugen/parser/vembyr')
import peg

env = getEnvironment(getDebug())

env['ARCHIVES'] = []
if scons.utils.useSDL():
    env['MUGEN_BACKEND'] = 'sdl'
elif scons.utils.useAllegro():
    env['MUGEN_BACKEND'] = 'allegro'
elif scons.utils.useAllegro5():
    env['MUGEN_BACKEND'] = 'allegro5'
else:
    env['MUGEN_BACKEND'] = 'unknown'

# Generates a single .cpp file from a .peg description
peg_builder = Builder(action = Action(peg_to_cpp, env['PEG_MAKE']),
                      suffix = '.cpp',
                      src_suffix = '.peg')

# Generates a .cpp file for each rule in the .peg description
peg_multiple_builder = Builder(action = Action(peg_to_cpp_multi, env['PEG_MAKE']),
                               suffix = '.cpp',
                               src_suffix = '.peg',
                               emitter = peg_emitter
                               )

env.Append(BUILDERS = {'Peg' : peg_builder})
env.Append(BUILDERS = {'PegMulti' : peg_multiple_builder})

if showTiming():
    cxxcom = env['CXXCOM']
    cccom = env['CCCOM']
    env.Replace(CXXCOM = 'misc/show-current-time %s' % cxxcom)
    env.Replace(CCCOM = 'misc/show-current-time %s' % cccom)

env['MUGEN_USE_PRX'] = usePrx()
env['MUGEN_NETWORKING'] = True
env.Append(CPPDEFINES = ['HAVE_NETWORKING'])

def getDataPath():
    try:
        return ARGUMENTS['data']
    except KeyError:
            return 'data'

debug = getDebug()
dataPath = getDataPath()

# This stuff should be moved to getEnvironment()
cflags = ['-Wall', '-Wno-unused-variable', '-Wno-unused-function']
cppflags = ['-Woverloaded-virtual']
cdefines = ['DATA_PATH=\\\"%s\\\"' % dataPath]

env.Append(CCFLAGS = cflags,
           CXXFLAGS = cppflags,
           CPPPATH = ["#src"],
           CPPDEFINES = cdefines)

if env['MUGEN_NETWORKING']:
    env.Append(CPPPATH = ['#src/util/network/hawknl'])

def buildType(env):
    properties = []
    if useMingwCross():
        properties.append('mingw-x')    
    if scons.utils.useSDL():
        properties.append('sdl')
    if scons.utils.useAllegro():
        properties.append('allegro')
    if scons.utils.useAllegro5():
        properties.append('allegro5')
    if getDebug():
        properties.append('debug')
    return '-'.join(properties)

import os.path
buildDir = os.path.join('build', buildType(env))
buildDirStatic = os.path.join('build/static', buildType(env))

# change this to if True if you want to profile
if enableProfiled():
    env.Append(CCFLAGS = '-pg')
    env.Append(LINKFLAGS = '-pg')

custom_tests = {"CheckPython" : scons.checks.checkPython,
                "CheckRuby" : scons.checks.checkRuby,
                "CheckRTTI" : scons.checks.checkRTTI,
                "CheckAllegro" : checkAllegro,
                "CheckAllegro5" : scons.checks.checkAllegro5(getDebug()),
                "CheckPthreads" : scons.checks.checkPthreads,
                "CheckSDL" : scons.checks.checkSDL,
                "CheckSDLMain" : scons.checks.checkSDLMain,
                "CheckOgg" : scons.checks.checkNativeOgg,
                "CheckMpg123" : scons.checks.checkMpg123,
                "CheckMad" : scons.checks.checkMad}

def display_build_properties(env):
    color = 'light-green'
    properties = []
    if useMingwCross():
        properties.append(scons.utils.colorize("Mingw-Cross-Compiler", color))
    if scons.utils.useAllegro():
        properties.append(scons.utils.colorize("Allegro", color))
    if scons.utils.useAllegro5():
        properties.append(scons.utils.colorize('Allegro5', color))
    if scons.utils.useSDL():
        properties.append(scons.utils.colorize("SDL", color))
    if getDebug():
        properties.append(scons.utils.colorize("Debug", color))
    type = ' '.join(properties)
    if not isQuiet():
        print "Build type: %s" % type

display_build_properties(env)

env['MUGEN_TESTS'] = custom_tests
env['MUGEN_COLORIZE'] = scons.utils.colorize

if isWindows():
    staticEnv = env.Clone()

    windows_tests = {"CheckWindowsFreeType" : checkWindowsFreeType}
    config = env.Configure(custom_tests = windows_tests)
    config.CheckWindowsFreeType()
    
    env = config.Finish()

    # Removed regex.dll
    if scons.utils.useAllegro():
        env.Append(CPPDEFINES = ['USE_ALLEGRO'])
        staticEnv.Append(CPPDEFINES = ['USE_ALLEGRO'])
        env.Append( LIBS = ['alleg', 'pthread', 'png', 'freetype', 'z', 'wsock32', ] )
    elif scons.utils.useAllegro5():
        env.Append(CPPDEFINES = ['USE_ALLEGRO5'])
        staticEnv.Append(CPPDEFINES = ['USE_ALLEGRO5'])
        env.Append(LIBS = ['allegro-5.0.3-monolith-md', 'wsock32', 'z', 'freetype', 'png', 'psapi'])
    
    if scons.utils.useSDL():
        staticEnv.Append(LIBS = ['SDL', 'pthread', 'png', 'freetype', 'z', 'wsock32'] )
    elif scons.utils.useAllegro():
        staticEnv.Append(LIBS = [ 'alleg', 'pthread', 'png', 'freetype', 'z', 'wsock32'] )
    
    staticEnv.Append(CPPDEFINES = 'WINDOWS')
else:
    staticEnv = env.Clone()

    import sys
    # find the system tool path by attaching SCons/Tool to everything
    def fix(q):
        return q + "/SCons/Tool"
    if scons.utils.useGch():
        env.Tool('gch', toolpath = ['misc'] + [fix(e) for e in sys.path if os.path.isdir(e)])
        if not getDebug() and not isVerbose():
            env['GCHFROMHCOMSTR'] = "%s %s" % (scons.utils.colorize('Compiling header', 'green'), scons.utils.colorize('$SOURCE', 'cyan'))

    if isOSX104():
        # Build a universal binary
        staticEnv['CXX'] = 'misc/g++'
        staticEnv['CC'] = 'misc/gcc'
    elif scons.utils.isLinux():
        if useMingwCross():
            staticEnv.Append(CPPDEFINES = ['WINDOWS', 'WIN32'])
            env.Append(CPPDEFINES = ['WINDOWS', 'WIN32'])
        else:
            staticEnv.Append(CPPDEFINES = 'LINUX')
            env.Append(CPPDEFINES = 'LINUX')
    

    def buildRtech(env):
        root = 'r-tech1'
        env.Append(CXXFLAGS = ['-fPIC'])
        env.Append(CCFLAGS = ['-fPIC'])
        build_dir_root = 'out'
        return env.SConscript('r-tech1/SConscript', variant_dir = buildDir + '/r-tech1', exports = ['env', 'root', 'build_dir_root'])

    if os.path.exists('r-tech1'):
        rtech1Env = env.Clone()
        env.Append(ARCHIVES = [buildRtech(rtech1Env)])
        env.Prepend(CPPPATH = rtech1Env['RTECH1_HEADERS'])
    else:
        scons.utils.safeParseConfig(env, 'pkg-config r-tech1 --cflags --libs')

    # Always need libz
    env.Append(LIBS = ['z'])

    config = env.Configure(custom_tests = custom_tests)
    try:
        if scons.utils.useAllegro() and not config.CheckAllegro():
            print "You need the development files for Allegro. Visit Allegro's website at http://alleg.sf.net or use your package manager to install them."
            Exit(1)

        if scons.utils.useAllegro5() and not config.CheckAllegro5():
            print "Allegro5 not found"
            Exit(1)

        config.CheckPthreads()

        if scons.utils.useSDL():
            if not config.CheckSDL():
                print "Install libsdl 1.2"
                Exit(1)
            config.CheckSDLMain()
        
        if not useMingwCross():
            scons.utils.safeParseConfig(config.env, 'freetype-config --libs --cflags')
            scons.utils.safeParseConfig(config.env, 'libpng-config --libs --ldflags --cflags')
        
            # staticEnv.ParseConfig( 'allegro-config --static --libs --cflags' )
            scons.utils.safeParseConfig(staticEnv, 'freetype-config --cflags')
            scons.utils.safeParseConfig(staticEnv, 'libpng-config --cflags')

        if useMingwCross():
            scons.utils.safeParseConfig(config.env, 'pkg-config freetype2 --libs --cflags')
            scons.utils.safeParseConfig(config.env, 'pkg-config libpng15 --libs --cflags')
    except OSError:
        pass
    
    png = staticEnv.Install('misc', readExec('libpng-config --libdir' ) + '/libpng.a')
    staticEnv.Append(LIBS = [png])
    staticEnv.Append(LIBS = ['z','m'])

    if True or not isOSX():
        path = readExec('freetype-config --prefix') + '/lib/libfreetype.a'
        if file_exists(path):
            freetype = staticEnv.Install('misc', path)
            staticEnv.Append(LIBS = freetype)
        else:
            scons.utils.safeParseConfig(staticEnv, 'freetype-config --libs')

    if not config.TryCompile("int main(){ return 0; }\n", ".c"):
        print "You need a C compiler such as gcc installed"
        Exit( 1 )
    if not config.TryCompile("class a{public: a(){} }; int main(){ a * x = new a(); delete x; return 0; }\n", ".cpp" ):
        print "You need a C++ compiler such as g++ installed"
        Exit( 1 )
    if not config.CheckHeader('ft2build.h'):
        print "You need freetype. Install freetype and/or X11"
        Exit( 1 )
    if not config.CheckHeader('png.h'):
        print "You need libpng. Get it from http://www.libpng.org/pub/png/libpng.html"
        Exit(1)
    config.CheckRTTI()
    config.CheckOgg()
    
    env = config.Finish()

    static_custom_tests = {"CheckPython" : scons.checks.checkPython,
                           "CheckRuby" : scons.checks.checkStaticRuby,
                           "CheckAllegro" : checkAllegro,
                           "CheckSDL" : scons.checks.checkStaticSDL,
                           "CheckSDLMain" : scons.checks.checkSDLMain,
                           "CheckPthreads" : scons.checks.checkPthreads,
                           "CheckRTTI" : scons.checks.checkRTTI}
    staticEnv['MUGEN_TESTS'] = static_custom_tests
    static_config = staticEnv.Configure(custom_tests = static_custom_tests)
    
    if scons.utils.useAllegro():
        static_config.CheckAllegro()
    if scons.utils.useSDL():
        static_config.CheckSDL()
        static_config.CheckSDLMain()

    static_config.CheckPthreads()

    static_config.CheckRTTI()
    staticEnv = static_config.Finish()

env['MUGEN_BUILD_TESTS'] = True
use = env
shared = SConscript('src/SConscript', variant_dir = buildDir, exports = ['use'] );

staticEnv['MUGEN_BUILD_TESTS'] = False
use = staticEnv
static = SConscript('src/SConscript', variant_dir = buildDirStatic, exports = ['use'] )

for i in shared:
    safe = env.Install('.', i)
    Default(safe)

for i in static:
    installed = staticEnv.InstallAs(i[0].name + '-static', i)
    Alias('static', installed)