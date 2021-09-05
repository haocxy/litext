file(GLOB_RECURSE asset_files "assets/*")
install(FILES ${asset_files} DESTINATION assets)

install(TARGETS litext litextcmd litext_body
    CONFIGURATIONS Release
    RUNTIME DESTINATION bin)

find_file(dlib_qt5core "Qt5Core.dll")
install(FILES ${dlib_qt5core} DESTINATION bin)

find_file(dlib_qt5gui "Qt5Gui.dll")
install(FILES ${dlib_qt5gui} DESTINATION bin)

find_file(dlib_qt5widgets "Qt5Widgets.dll")
install(FILES ${dlib_qt5widgets} DESTINATION bin)

find_file(dlib_zlibd "zlibd.dll")
install(FILES ${dlib_zlibd} DESTINATION bin)

find_file(dlib_png "libpng16d.dll")
install(FILES ${dlib_png} DESTINATION bin)

find_file(dlib_msvcp140 "MSVCP140.dll")
install(FILES ${dlib_msvcp140} DESTINATION bin)

find_file(dlib_vcruntime140 "VCRUNTIME140.dll")
install(FILES ${dlib_vcruntime140} DESTINATION bin)

find_file(dlib_vcruntime140_1 "VCRUNTIME140_1.dll")
install(FILES ${dlib_vcruntime140_1} DESTINATION bin)

find_file(dlib_vcruntime140d "VCRUNTIME140D.dll")
install(FILES ${dlib_vcruntime140d} DESTINATION bin)

find_file(dlib_ucrtbased "ucrtbased.dll")
install(FILES ${dlib_ucrtbased} DESTINATION bin)

find_file(dlib_qtplugin_platforms_windows "qwindows.dll" PATH_SUFFIXES plugins/platforms)
install(FILES ${dlib_qtplugin_platforms_windows} DESTINATION bin/platforms)

find_file(dlib_qtplugin_styles_vistastyle "qwindowsvistastyle.dll" PATH_SUFFIXES plugins/styles)
install(FILES ${dlib_qtplugin_styles_vistastyle} DESTINATION bin/styles)

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME "litext")
set(CPACK_PACKAGE_VERSION ${LITEXT_RELEASE_VERSION})
include(CPack)
