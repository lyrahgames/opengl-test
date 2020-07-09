cxx.std = latest
using cxx

libs =
import libs += glfw3%lib{glfw3}
import libs += glbinding%lib{glbinding}
import libs += glm%lib{glm}

exe{main}: cxx{main.cpp} $libs