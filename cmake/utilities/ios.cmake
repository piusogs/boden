macro(use_boden_template_info_plist TARGETNAME)
	if(BDN_PLATFORM_IOS)
		get_target_property(BODEN_CMAKE_SOURCE_DIR boden_cmake SOURCE_DIR)
		set_target_properties(${TARGETNAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${BODEN_CMAKE_SOURCE_DIR}/templates/IOSInfo.plist.in)
	endif()
endmacro()