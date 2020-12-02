#include <r-tech1/graphics/bitmap.h>
#include <r-tech1/font.h>
#include <r-tech1/funcs.h>
#include <string>
#include <vector>
#include <stdarg.h>
#include "font_render.h"

using namespace std;

FontRender * FontRender::my_render = NULL;
FontRender * FontRender::getInstance(){
	if ( my_render == NULL )
		my_render = new FontRender();
	return my_render;
}

void FontRender::destroy(){
	if ( my_render != NULL )
		delete my_render;
}

void FontRender::render(const Graphics::Bitmap * work, double scaleWidth, double scaleHeight){
    for ( vector<render_message>::iterator it = messages.begin(); it != messages.end(); it++ ){
        const render_message & r = *it;

        if (r.translucency != -1){
            Graphics::Bitmap::transBlender(0, 0, 0, r.translucency);
            r.r_font.printf((int)(r.x * scaleWidth), (int)(r.y * scaleHeight), (int)(r.sizeX * scaleWidth), (int)(r.sizeY * scaleHeight), r.fg, work->translucent(), r.str, 0);
        } else {
            r.r_font.printf((int)(r.x * scaleWidth), (int)(r.y * scaleHeight), (int)(r.sizeX * scaleWidth), (int)(r.sizeY * scaleHeight), r.fg, *work, r.str, 0);
        }

    }
    messages.clear();
}

void FontRender::addMessage( const Font & f, int x, int y, Graphics::Color fg, Graphics::Color bg, int translucency, const string & str ){
    render_message r( f, x, y, f.getSizeX(), f.getSizeY(), fg, bg, translucency, str );
    messages.push_back( r );
}

void FontRender::addMessage( const Font & f, int x, int y, Graphics::Color fg, Graphics::Color bg, const string & str ){
    addMessage(f, x, y, fg, bg, -1, str);
}

void FontRender::addMessage( const Font & f, int x, int y, Graphics::Color fg, Graphics::Color bg, const char * str, ... ){
	char buf[1024];
	va_list ap;

	va_start(ap, str);
        Util::limitPrintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	string mm( buf );
	addMessage( f, x, y, fg, bg, -1, mm );
}
	
void FontRender::addMessage(const Filesystem::RelativePath & font_name, int x, int y, Graphics::Color fg, Graphics::Color bg, const string & str ){
    addMessage(Font::getFont(font_name), x, y, fg, bg, -1, str );
}
