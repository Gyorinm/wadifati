#include "aethera/shape_mesh.hpp"
#include <algorithm>
namespace aethera {
std::vector<MeshVertex> ShapeMeshGenerator::vertices_from_alpha(const ImageRgba8& image, int columns, int rows, std::uint8_t alpha_threshold) {
    columns = std::max(1, columns); rows = std::max(1, rows);
    std::vector<MeshVertex> out;
    out.reserve(static_cast<std::size_t>((columns+1)*(rows+1)));
    const float w = static_cast<float>(std::max(1,image.width));
    const float h = static_cast<float>(std::max(1,image.height));
    for(int y=0;y<=rows;++y) for(int x=0;x<=columns;++x){
        float u=static_cast<float>(x)/columns, v=static_cast<float>(y)/rows;
        int px=std::min(image.width-1,std::max(0,(int)(u*(image.width-1))));
        int py=std::min(image.height-1,std::max(0,(int)(v*(image.height-1))));
        std::size_t idx=(std::size_t)(py*image.width+px)*4;
        bool visible=image.valid() && idx+3<image.pixels.size() && image.pixels[idx+3]>=alpha_threshold;
        float pxu=u, pxv=v;
        if(!visible){
            int radius=1; bool found=false;
            for(int r=1;r<=radius && !found;++r) for(int yy=std::max(0,py-r);yy<=std::min(image.height-1,py+r)&&!found;++yy) for(int xx=std::max(0,px-r);xx<=std::min(image.width-1,px+r);++xx){
                std::size_t q=(std::size_t)(yy*image.width+xx)*4;
                if(q+3<image.pixels.size()&&image.pixels[q+3]>=alpha_threshold){pxu=(float)xx/(image.width-1?image.width-1:1);pxv=(float)yy/(image.height-1?image.height-1:1);found=true;break;}
            }
        }
        MeshVertex mv; mv.position={pxu*w,pxv*h}; mv.uv={u,v}; out.push_back(mv);
    }
    return out;
}
std::vector<std::uint32_t> ShapeMeshGenerator::indices(int columns,int rows){
    columns=std::max(1,columns); rows=std::max(1,rows); std::vector<std::uint32_t> idx; idx.reserve((std::size_t)columns*rows*6); int s=columns+1;
    for(int y=0;y<rows;++y) for(int x=0;x<columns;++x){std::uint32_t a=y*s+x,b=a+1,c=a+s,d=c+1; idx.insert(idx.end(),{a,c,b,b,c,d});}
    return idx;
}
}
