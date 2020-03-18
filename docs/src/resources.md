 @page resources Resources

This chapter discusses resources and how to distribute and use them with EGT.

Resources are a way of cataloging, distributing, identifying, and using some
asset data.  For example, assets might include images, icons, audio files, and
video files, xml files, fonts, or any binary blob. Resources may be stored in
the filesystem and loaded on demand, or they may be compiled into the
application binary itself, or they may be installed by default when installing
the EGT library.

@section resources_paths Resource Paths

Resources in EGT are identified with a Universal Resource Identifier (URI).
This is something that can be converted to and from a simple std::string.  It
can be used to identify relative and absolute file paths, EGT icons, network
URLs, and resources registered with ResourceManager.

There general form of a URI, as defined by RFC 3986 is:

    URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

Each URI begins with a scheme name that refers to a specification for assigning
identifiers within that scheme. EGT defines several custom schemes to locate EGT
built-in icons and ResourceManager resources.

@subsection resources_using File Paths

To access a resource with a local file path, specify an absolute or relative
file path.

    file:/absolute/file/path.txt
    file:../relative/file/path.txt
    file:relative/file/path.txt

If an absolute file path is used, nothing extra will happen and it should work
just like you would expect.  That file path will attempt to be accessed as-is.

However, if a relative path is given then a more advanced search will happen by
calling the internal EGT function resolve_file_path().  This function will
search any directory registered with add_search_path(), very similar to how the
`PATH` variable works on UNIX system shells to find binary files.  The default
paths added automatically with add_search_path() by EGT are searched in the
following order:

- Data install path.
- EXE directory.
- EGT icon path.

@code{.cpp}
auto play = egt::Image("file:/some/path/play.png");
auto forward = egt::Image("file:forward.png");
@endcode

@subsection resources_icons EGT Provided Icon and Image Files

The EGT library comes with a set of icons and images installed with EGT by
default.  To access these icons and images, the scheme uses is `icon` like so:

@code{.cpp}
auto bug = egt::Image("icon:bug.png");
@endcode

In the above example, bug.png is picked from a EGT installed default icons
directory (i.e. /usr/share/libegt/icons). If a user wants to pick a different
icon size, use the following syntax as described in the provisional
[IETF draft](http://www.iana.org/go/draft-lafayette-icon-uri-scheme) for the
`icon:` scheme:

@code{.cpp}
auto bug = egt::Image("icon:bug.png;128");
@endcode

The EGT installed default icons directory can be changed by setting an
environment variable EGT_ICONS_DIRECTORY. This value can be an absolute path or
directory name. If a directory name is set, then this directory name will be
appended with an EGT installed default icon's directory path.  See @ref
environ for more information.

@subsection resources_resourcemanager Resources Registered with ResourceManager

Instead of using a filesystem path to a file, the scheme used is `res` to
interpret the path as a resource name that should be read from ResourceManager.

@code{.cpp}
auto play = egt::Image("res:play_png");
@endcode

@section resources_embedded Embedding Resources in an Application at Compile Time

EGT provides several ways to embed a resource into an EGT application binary
itself, and then reference those resources like any other resource through
ResourceManager.

@subsection resources_embed Embed by Including Files Directly in Code

EGT provides a preprocessor macro called EGT_EMBED().  This allows you, in one
line, to directly include any arbitrary file in your source code and then
reference it at runtime with ResourceManager from any compilation unit using the
`res` URI scheme.

@code{.cpp}
EGT_EMBED(my_image, "../images/my_image.png");

egt::Image my_image("res:my_image");
@endcode

@subsection resources_mresg Generate Code with mresg Tool

EGT provides a tool used to generate a C++ code file that contains binary blobs
from filesystem files at compile time that you can link with your application
and reference through ResourceManager from any compilation unit using the `res`
URI scheme. This tool is in the `scripts` directory of the EGT repository and
it's called `mresg`.

This tool is generally run by your application Makefile to create the resulting
`OUTPUT` C++ file that can then be compiled into the application binary.

@code{.cpp}
$ ./mresg --help
./mresg INPUT... -o OUTPUT
@endcode

Note that when using mresg the resource name registered with ResourceManager has
all periods replaced with underscores.
