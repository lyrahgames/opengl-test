cxx.std = latest
using cxx

hxx{*}: extension = hpp
cxx{*}: extension = cpp

test.target = $cxx.target

libs =
import libs += glfw3%lib{glfw3}
import libs += glbinding%lib{glbinding}

# GLM does not provide a pkg-config file on Windows.
# But by using bdep and appropriate modules,
# we would be able to automatically download it
# from cppget.org package repositories.
# But this was not the intention of the test.
if! ($cxx.target.class == 'windows')
  import libs += glm%lib{glm}

exe{opengl-test}: {hxx cxx}{**} $libs
{
  test = true
}
