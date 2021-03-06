#file(GLOB_RECURSE asset_files "assets/*")
#install(FILES ${asset_files} DESTINATION "assets")

set(google_noto_font_files "")
list(APPEND google_noto_font_files "assets/fonts/google_noto/NotoSansSC-Regular.otf")
list(APPEND google_noto_font_files "assets/fonts/google_noto/OFL.txt")
install(FILES ${google_noto_font_files} DESTINATION "assets/fonts/google_noto")

install(TARGETS litext litextcmd litext_body
    CONFIGURATIONS Release
    RUNTIME DESTINATION ".")

find_file(dlib_qt5core "Qt5Core.dll")
install(FILES ${dlib_qt5core} DESTINATION ".")

find_file(dlib_qt5gui "Qt5Gui.dll")
install(FILES ${dlib_qt5gui} DESTINATION ".")

find_file(dlib_qt5widgets "Qt5Widgets.dll")
install(FILES ${dlib_qt5widgets} DESTINATION ".")

find_file(dlib_zlib "zlib.dll")
message("Found dll for pack [${dlib_zlib}]")
install(FILES ${dlib_zlib} DESTINATION ".")

find_file(dlib_png "libpng16.dll")
message("Found dll for pack [${dlib_png}]")
install(FILES ${dlib_png} DESTINATION ".")

find_file(dlib_freetype "freetype.dll")
message("Found dll for pack [${dlib_freetype}]")
install(FILES ${dlib_freetype} DESTINATION ".")

find_file(dlib_msvcp140 "MSVCP140.dll")
install(FILES ${dlib_msvcp140} DESTINATION ".")

find_file(dlib_vcruntime140 "VCRUNTIME140.dll")
install(FILES ${dlib_vcruntime140} DESTINATION ".")

find_file(dlib_vcruntime140_1 "VCRUNTIME140_1.dll")
install(FILES ${dlib_vcruntime140_1} DESTINATION ".")

find_file(dlib_vcruntime140d "VCRUNTIME140D.dll")
install(FILES ${dlib_vcruntime140d} DESTINATION ".")

find_file(dlib_ucrtbased "ucrtbased.dll")
install(FILES ${dlib_ucrtbased} DESTINATION ".")

find_file(dlib_qtplugin_platforms_windows "qwindows.dll" PATH_SUFFIXES plugins/platforms)
install(FILES ${dlib_qtplugin_platforms_windows} DESTINATION "platforms")

find_file(dlib_qtplugin_styles_vistastyle "qwindowsvistastyle.dll" PATH_SUFFIXES plugins/styles)
install(FILES ${dlib_qtplugin_styles_vistastyle} DESTINATION "styles")

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME "litext")
set(CPACK_PACKAGE_VERSION ${LITEXT_RELEASE_VERSION})
include(CPack)
