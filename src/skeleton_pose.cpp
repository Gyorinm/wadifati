#include "aethera/skeleton_pose.hpp"
#include <cmath>
#include <algorithm>
namespace aethera {
namespace { Vec2 rot(Vec2 p,float a){float c=std::cos(a),s=std::sin(a);return {p.x*c-p.y*s,p.x*s+p.y*c};} }
void SkeletonPose::propagate(std::vector<SkeletonJoint>& j){
 for(std::size_t i=0;i<j.size();++i){int p=j[i].parent;if(p<0||p>=(int)j.size()){j[i].current=j[i].bind;continue;} Vec2 local=j[i].current-j[i].bind; j[i].current=j[p].current+rot(local,j[p].current_rotation-j[p].bind_rotation); j[i].current_rotation=j[p].current_rotation+(j[i].current_rotation-j[i].bind_rotation);}
}
std::vector<MeshBone> SkeletonPose::to_mesh_bones(const std::vector<SkeletonJoint>& j){std::vector<MeshBone> b; b.reserve(j.size()); for(std::size_t i=0;i<j.size();++i){MeshBone m; m.index=(std::uint32_t)i;m.bind_position=j[i].bind;m.current_position=j[i].current;m.bind_rotation=j[i].bind_rotation;m.current_rotation=j[i].current_rotation;b.push_back(m);} return b;}
}
