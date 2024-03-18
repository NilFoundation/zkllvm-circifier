
install(DIRECTORY templates
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT evmlib
)

install(FILES ecc_tool.rb
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT evmlib
)