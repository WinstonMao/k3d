PROJECT(k3d-make-module-proxy)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3dsdk_BINARY_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_GLIBMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_SIGC_INCLUDE_DIRS})

LINK_DIRECTORIES(${K3D_GLIBMM_LIB_DIRS})
LINK_DIRECTORIES(${K3D_SIGC_LIB_DIRS})

ADD_EXECUTABLE(k3d-make-module-proxy
  main.cpp
  )

SET_TARGET_PROPERTIES(k3d-make-module-proxy PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${K3D_RUNTIME_OUTPUT_DIRECTORY}
  )

TARGET_LINK_LIBRARIES(k3d-make-module-proxy
  k3dsdk
  )

