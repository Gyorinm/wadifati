#include <cassert>
#include "aethera/editor.hpp"
#include "aethera/image_pipeline.hpp"
using namespace aethera;
int main(){
    ImageRgba8 image; image.width=32; image.height=32; image.pixels.resize(32*32*4,255);
    Editor editor; editor.set_actor(ImagePipeline::build(image,4,4));
    assert(editor.actor()!=nullptr);
    editor.select_joint(1);
    const Vec2 before=editor.actor()->skeleton[1].current;
    editor.move_selected({3.0f,2.0f});
    assert(editor.actor()->skeleton[1].current.x==before.x+3.0f);
    assert(editor.actor()->skeleton[1].current.y==before.y+2.0f);
    return 0;
}
