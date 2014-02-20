######################################################################
# Automatically generated by qmake (2.01a) Sa. Mrz 19 17:12:54 2011
######################################################################

# HACK! BIG TIME!
DEFINES += EXTERNAL_USE

DEPENDPATH += src/core src/internals src/mapwidget src/internals/projections
INCLUDEPATH += src/core src/internals src/internals/projections src/mapwidget

# Input
HEADERS += opmapcontrol.h \
           src/core/accessmode.h \
           src/core/alllayersoftype.h \
           src/core/cache.h \
           src/core/cacheitemqueue.h \
           src/core/debugheader.h \
           src/core/diagnostics.h \
           src/core/geodecoderstatus.h \
           src/core/kibertilecache.h \
           src/core/languagetype.h \
           src/core/maptype.h \
           src/core/memorycache.h \
           src/core/opmaps.h \
           src/core/placemark.h \
           src/core/point.h \
           src/core/providerstrings.h \
           src/core/pureimage.h \
           src/core/pureimagecache.h \
           src/core/rawtile.h \
           src/core/size.h \
           src/core/tilecachequeue.h \
           src/core/urlfactory.h \
           src/internals/copyrightstrings.h \
           src/internals/core.h \
           src/internals/debugheader.h \
           src/internals/loadtask.h \
           src/internals/mousewheelzoomtype.h \
           src/internals/pointlatlng.h \
           src/internals/pureprojection.h \
           src/internals/rectangle.h \
           src/internals/rectlatlng.h \
           src/internals/sizelatlng.h \
           src/internals/tile.h \
           src/internals/tilematrix.h \
           src/mapwidget/configuration.h \
           src/mapwidget/gpsitem.h \
           src/mapwidget/homeitem.h \
           src/mapwidget/mapgraphicitem.h \
           src/mapwidget/mapripform.h \
           src/mapwidget/mapripper.h \
           src/mapwidget/opmapwidget.h \
           src/mapwidget/trailitem.h \
           src/mapwidget/traillineitem.h \
           src/mapwidget/uavitem.h \
           src/mapwidget/uavmapfollowtype.h \
           src/mapwidget/uavtrailtype.h \
           src/mapwidget/waypointitem.h \
           src/internals/projections/lks94projection.h \
           src/internals/projections/mercatorprojection.h \
           src/internals/projections/mercatorprojectionyandex.h \
           src/internals/projections/platecarreeprojection.h \
           src/internals/projections/platecarreeprojectionpergo.h \
           libs/opmapcontrol/src/mapwidget/waypointlineitem.h
FORMS += src/mapwidget/mapripform.ui
SOURCES += src/core/alllayersoftype.cpp \
           src/core/cache.cpp \
           src/core/cacheitemqueue.cpp \
           src/core/diagnostics.cpp \
           src/core/kibertilecache.cpp \
           src/core/languagetype.cpp \
           src/core/memorycache.cpp \
           src/core/opmaps.cpp \
           src/core/placemark.cpp \
           src/core/point.cpp \
           src/core/providerstrings.cpp \
           src/core/pureimage.cpp \
           src/core/pureimagecache.cpp \
           src/core/rawtile.cpp \
           src/core/size.cpp \
           src/core/tilecachequeue.cpp \
           src/core/urlfactory.cpp \
           src/internals/core.cpp \
           src/internals/loadtask.cpp \
           src/internals/MouseWheelZoomType.cpp \
           src/internals/pointlatlng.cpp \
           src/internals/pureprojection.cpp \
           src/internals/rectangle.cpp \
           src/internals/rectlatlng.cpp \
           src/internals/sizelatlng.cpp \
           src/internals/tile.cpp \
           src/internals/tilematrix.cpp \
           src/mapwidget/configuration.cpp \
           src/mapwidget/gpsitem.cpp \
           src/mapwidget/homeitem.cpp \
           src/mapwidget/mapgraphicitem.cpp \
           src/mapwidget/mapripform.cpp \
           src/mapwidget/mapripper.cpp \
           src/mapwidget/opmapwidget.cpp \
           src/mapwidget/trailitem.cpp \
           src/mapwidget/traillineitem.cpp \
           src/mapwidget/uavitem.cpp \
           src/mapwidget/waypointitem.cpp \
           src/internals/projections/lks94projection.cpp \
           src/internals/projections/mercatorprojection.cpp \
           src/internals/projections/mercatorprojectionyandex.cpp \
           src/internals/projections/platecarreeprojection.cpp \
           src/internals/projections/platecarreeprojectionpergo.cpp \
           libs/opmapcontrol/src/mapwidget/waypointlineitem.cpp
RESOURCES += src/mapwidget/mapresources.qrc
