# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-src")
  file(MAKE_DIRECTORY "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-src")
endif()
file(MAKE_DIRECTORY
  "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-build"
  "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-subbuild/webview-populate-prefix"
  "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-subbuild/webview-populate-prefix/tmp"
  "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-subbuild/webview-populate-prefix/src/webview-populate-stamp"
  "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-subbuild/webview-populate-prefix/src"
  "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-subbuild/webview-populate-prefix/src/webview-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-subbuild/webview-populate-prefix/src/webview-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Ecole/ESGI/deuxieme_annee/Programmation/C/Projet_C_Domotique/build/_deps/webview-subbuild/webview-populate-prefix/src/webview-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
