/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   sine_png;
    const int            sine_pngSize = 27992;

    extern const char*   triangle_png;
    const int            triangle_pngSize = 48007;

    extern const char*   saw_png;
    const int            saw_pngSize = 59871;

    extern const char*   square_png;
    const int            square_pngSize = 22597;

    extern const char*   square1_png;
    const int            square1_pngSize = 26106;

    extern const char*   square2_png;
    const int            square2_pngSize = 34642;

    extern const char*   RobotoItalic_ttf;
    const int            RobotoItalic_ttfSize = 152208;

    extern const char*   RobotoRegular_ttf;
    const int            RobotoRegular_ttfSize = 146004;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 8;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
