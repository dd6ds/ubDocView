# Building from source

The app can be built from source using CMake.

## Dependencies

A complete list of dependencies for the project can be found in debian/control.

# Building click packages

click packages can be built using
[clickable](https://clickable-ut.dev/en/latest/).

## LibreOfficeKit

The project depends on LibreOfficeKit which is very resource-intensive to
build.  The clickable build recipe to built the LibreOfficeKit libaries thus
exists in a [separate project][1].  By default clickable will download the
build results of that project from GitLab CI when building
lomiri-docviewer-app.  Alternatively, LibreOfficeKit can be built from source
using above project in which case the build result need to be placed into the
clickable library build directory of lomiri-docviewer-app before starting a
clickable build.

[1]: https://gitlab.com/ubports/development/apps/lomiri-docviewer-app-libreoffice
