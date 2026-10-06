#include "CustomFont.h"

using namespace juce;

const Font& CustomFont::getRegularFont(float size)
{
    static Font font = Font(FontOptions(Typeface::createSystemTypefaceFor(BinaryData::RobotoRegular_ttf, BinaryData::RobotoRegular_ttfSize)));
    font.setHeight(size);
    
    return font;
}

const Font& CustomFont::getItalicFont(float size)
{
    static Font font = Font(FontOptions(Typeface::createSystemTypefaceFor(BinaryData::RobotoItalic_ttf, BinaryData::RobotoItalic_ttfSize)));
    font.setHeight(size);
    
    return font;
}

/*
const Font& CustomFont::getBoldFont(float size)
{
    static Font font = Font(FontOptions(Typeface::createSystemTypefaceFor(BinaryData::RobotoSemiBold_ttf, BinaryData::RobotoSemiBold_ttfSize)));
    font.setHeight(size);
    
    return font;
}
*/

const String CustomFont::TYPEFACE_NAME = "Roboto";
const float CustomFont::DEFAULT_SIZE = 15.5;

const Font& CustomFont::REGULAR = getRegularFont(DEFAULT_SIZE);
const Font& CustomFont::ITALIC = getItalicFont(DEFAULT_SIZE);
//const Font& CustomFont::BOLD = getBoldFont(DEFAULT_SIZE);
