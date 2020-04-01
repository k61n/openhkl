# CMake module to search for the library libQCustomPlot
#
# If the library is found, then QCustomPlot_FOUND is set to TRUE,
# and following variables are set:
#    QCustomPlot_INCLUDE_DIR
#    QCustomPlot_LIBRARIES

find_path(QCustomPlot_INCLUDE_DIR NAMES qcustomplot.h)
find_library(QCustomPlot_LIBRARIES NAMES qcustomplot libqcustomplot)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QCustomPlot DEFAULT_MSG QCustomPlot_LIBRARIES QCustomPlot_INCLUDE_DIR)
mark_as_advanced(QCustomPlot_INCLUDE_DIR QCustomPlot_LIBRARIES)
