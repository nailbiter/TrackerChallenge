PROJECT( tracker_challenge_proj )
FIND_PACKAGE( OpenCV REQUIRED )
ADD_EXECUTABLE( tracker_challenge test.cpp )
TARGET_LINK_LIBRARIES( tracker_challenge ${OpenCV_LIBS} )
