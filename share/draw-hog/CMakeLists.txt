set( APP_NAME draw-hog)

add_executable(${APP_NAME} 
  main.cpp
  )

target_link_libraries(${APP_NAME} 
  ${DLIB_LIBRARY}  
  blas
  lapack
  ${OpenCV_LIBS}
  track
  syllo
  sonar-sdk  
  ${Boost_LIBRARIES}  
  workbench-utils
  wb  
  )
