cxx.std = latest
using cxx

hxx{*}: extension = hpp
cxx{*}: extension = cpp

test.target = $cxx.target

libs =
import libs += glfw3%lib{glfw3}
import libs += glbinding%lib{glbinding}
import libs += glm%lib{glm}

exe{opengl-test}: {hxx cxx}{**} $libs
{
  test = true
}

cxx.poptions =+ "-I$src_root"
