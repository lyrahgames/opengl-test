cxx.std = latest
using cxx

libs =
import libs += glfw3%lib{glfw3}
import libs += glbinding%lib{glbinding}

if ($cc.target.class == 'linux')
{
  info 'linux'
  import libs += glm%lib{glm}
}
elif ($cc.target.class == 'windows')
{
  info 'windows'
  if ($cc.target.system == 'mingw32')
  {
    info 'mingw32'
    cc.libs += -lgdi32
  }
}

exe{main}: cxx{main.cpp} $libs