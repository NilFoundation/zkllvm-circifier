
install(DIRECTORY templates
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT evm-stdlib
)

install(FILES ecc_tool.rb
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT evm-stdlib
)