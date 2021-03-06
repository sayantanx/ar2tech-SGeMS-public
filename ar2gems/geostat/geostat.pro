# #####################################################################
# Automatically generated by qmake (2.01a) Sun Nov 1 16:23:15 2009
# #####################################################################
TEMPLATE = lib
TARGET = 
DEPENDPATH += . \
    filtersim_std \
    snesim_std
INCLUDEPATH += . \
    filtersim_std \
    snesim_std

# Input
HEADERS += difference_with_base.h \
    kriging_mean.h \
    Postsim_categorical.h \
    cokriging.h \
    common.h \
    cosgsim.h \
    cosisim.h \
    dssim.h \
    Filtersim_filters.h \
    geostat_algo.h \
    grid_variog_computer.h \
    GsTL_filters.h \
    ImageProcess.h \
    indicator_kriging.h \
    kriging.h \
    library_geostat_init.h \
    LU_sim.h \
    moving_window.h \
    nuTauModel.h \
    parameters_handler.h \
    parameters_handler_impl.h \
    PostKriging.h \
    Postsim.h \
    pset_variog_computer.h \
    sgsim.h \
    sisim.h \
    trans.h \
    transcat.h \
    utilities.h \
    filtersim_std/dev_finder.h \
    filtersim_std/distance.h \
    filtersim_std/distance_kernels.h \
    filtersim_std/filters.h \
    filtersim_std/filtersim.h \
    filtersim_std/filtersim_cate.h \
    filtersim_std/filtersim_cont.h \
    filtersim_std/filtersim_servo_system.h \
    filtersim_std/filtersim_tau_updating.h \
    filtersim_std/is_categorical.h \
    filtersim_std/kernelized_kmeans.h \
    filtersim_std/mapping_pattern_id.h \
    filtersim_std/partition.h \
    filtersim_std/patch_helper.h \
    filtersim_std/pattern.h \
    filtersim_std/pattern_paster.h \
    filtersim_std/prototype.h \
    filtersim_std/prototype_help.h \
    filtersim_std/Prototype_kernelized_kmeans.h \
    filtersim_std/prototype_list.h \
    filtersim_std/sequential_patch_simulation.h \
    filtersim_std/tau_updating.h \
    filtersim_std/TI_manipulation.h \
    snesim_std/is_categorical.h \
    snesim_std/is_categorical.hpp \
    snesim_std/layer_sequential_simulation.h \
    snesim_std/layer_servo_system_sampler.h \
    snesim_std/NodeDropped.h \
    snesim_std/snesim_std.h \
    snesim_std/tau_updating.h \
    snesim_std/tree_list.h \
    filtersim_std/filtersim_servo_system.h \
    snesim_std/layer_servo_system_sampler.h \
    clustering_from_centroids.h \
    kmeans_clustering.h \
    declus.h \
    kriging_cross_validation.h \
    cokriging_cross_validation.h
SOURCES += difference_with_base.cpp \
    kriging_mean.cpp \
    Postsim_categorical.cpp \
    cokriging.cpp \
    cosgsim.cpp \
    cosisim.cpp \
    dssim.cpp \
    grid_variog_computer.cpp \
    ImageProcess.cpp \
    indicator_kriging.cpp \
    kriging.cpp \
    library_geostat_init.cpp \
    LU_sim.cpp \
    moving_window.cpp \
    nuTauModel.cpp \
    parameters_handler_impl.cpp \
    PostKriging.cpp \
    Postsim.cpp \
    pset_variog_computer.cpp \
    sgsim.cpp \
    sisim.cpp \
    trans.cpp \
    transcat.cpp \
    utilities.cpp \
    filtersim_std/dev_finder.cpp \
    filtersim_std/filters.cpp \
    filtersim_std/filtersim.cpp \
    filtersim_std/filtersim_cate.cpp \
    filtersim_std/filtersim_cont.cpp \
    filtersim_std/filtersim_servo_system.cpp \
    filtersim_std/mapping_pattern_id.cpp \
    filtersim_std/partition.cpp \
    filtersim_std/patch_helper.cpp \
    filtersim_std/pattern_paster.cpp \
    filtersim_std/TI_manipulation.cpp \
    snesim_std/layer_servo_system_sampler.cpp \
    snesim_std/NodeDropped.cpp \
    snesim_std/snesim_std.cpp \
    clustering_from_centroids.cpp \
    kmeans_clustering.cpp \
    declus.cpp \    
    kriging_cross_validation.cpp \
    cokriging_cross_validation.cpp

TARGET = ar2tech_geostat
QT += xml
CONFIG += plugin
INCLUDEPATH += $(QTDIR)/tools/designer/interfaces
contains( TEMPLATE, lib ) { 
    DESTDIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/lib/$$CUR_PLATFORM/obj/geostat
}
contains( TEMPLATE, app ) { 
    DESTDIR += $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM
    OBJECTS_DIR = $$GSTLAPPLI_HOME/bin/$$CUR_PLATFORM/obj/geostat
}
include( $$GSTLAPPLI_HOME/config.qmake )
exists( MMakefile ):include( MMakefile )

OTHER_FILES += \
    geostat.vcxproj
