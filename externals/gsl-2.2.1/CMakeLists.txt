
# linux only!!
add_custom_target(gsl_library)

add_custom_command(TARGET gsl_library
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/configure
  COMMAND make
  )

