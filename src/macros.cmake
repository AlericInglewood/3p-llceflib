if (WIN32)
  MACRO(EMBED_MANIFEST _target type)
    ADD_CUSTOM_COMMAND(
      TARGET ${_target}
      POST_BUILD
      COMMAND "mt.exe"
      ARGS
        -nologo
        -manifest \"${CMAKE_SOURCE_DIR}\\resources\\manifests\\compatibility.manifest\"
        -inputresource:\"$<TARGET_FILE:${_target}>\"\;\#${type}
        -outputresource:\"$<TARGET_FILE:${_target}>\"\;\#${type}
      COMMENT "Adding compatibility manifest to ${_target}"
    )
  ENDMACRO(EMBED_MANIFEST _target type)
endif (WIN32)