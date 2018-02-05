#include <memory>
using namespace std;
class TimentrisGame;
typedef shared_ptr<TimentrisGame> TimentrisGamePtr;
class GLWindow;
typedef shared_ptr<GLWindow> GLWindowPtr;
class Render;
typedef shared_ptr<Render> RenderPtr;
class Camera;
typedef shared_ptr<Camera> CameraPtr;
class Input;
typedef shared_ptr<Input> InputPtr;