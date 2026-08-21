#include "aethera/scene_file.hpp"
#include <fstream>
namespace aethera {
bool SceneFile::save(const std::string& path, const ImageActor& actor) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out << "AETHERA_SCENE 1\n";
    out << "image " << actor.image.width << ' ' << actor.image.height << "\n";
    out << "joints " << actor.skeleton.size() << "\n";
    for (const auto& j : actor.skeleton) {
        out << j.parent << ' ' << j.bind.x << ' ' << j.bind.y << ' '
            << j.bind_rotation << ' ' << j.current.x << ' ' << j.current.y << ' '
            << j.current_rotation << '\n';
    }
    out << "mesh " << actor.mesh.size() << ' ' << actor.indices.size() << "\n";
    for (const auto& v : actor.mesh) {
        out << v.position.x << ' ' << v.position.y << ' ' << v.uv.x << ' ' << v.uv.y << ' '
            << v.bone0 << ' ' << v.bone1 << ' ' << v.weight0 << ' ' << v.weight1 << '\n';
    }
    for (auto i : actor.indices) out << i << ' ';
    out << "\n";
    out << "pixels " << actor.image.pixels.size() << "\n";
    out.write(reinterpret_cast<const char*>(actor.image.pixels.data()), static_cast<std::streamsize>(actor.image.pixels.size()));
    return static_cast<bool>(out);
}

bool SceneFile::load(const std::string& path, ImageActor& actor) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    std::string magic; int version = 0;
    if (!(in >> magic >> version) || magic != "AETHERA_SCENE" || version != 1) return false;
    std::string tag; int w=0,h=0; std::size_t joints=0, vertices=0, indices=0;
    if (!(in >> tag >> w >> h) || tag != "image") return false;
    if (!(in >> tag >> joints) || tag != "joints") return false;
    actor = {};
    actor.image.width=w; actor.image.height=h;
    actor.skeleton.resize(joints);
    for (auto& j : actor.skeleton) {
        if (!(in >> j.parent >> j.bind.x >> j.bind.y >> j.bind_rotation >> j.current.x >> j.current.y >> j.current_rotation)) return false;
    }
    if (!(in >> tag >> vertices >> indices) || tag != "mesh") return false;
    actor.mesh.resize(vertices);
    for (auto& v : actor.mesh) {
        if (!(in >> v.position.x >> v.position.y >> v.uv.x >> v.uv.y >> v.bone0 >> v.bone1 >> v.weight0 >> v.weight1)) return false;
    }
    actor.indices.resize(indices);
    for (auto& i : actor.indices) if (!(in >> i)) return false;
    if (!(in >> tag >> vertices) || tag != "pixels") return false;
    in.get();
    actor.image.pixels.resize(vertices);
    in.read(reinterpret_cast<char*>(actor.image.pixels.data()), static_cast<std::streamsize>(vertices));
    if (!actor.image.valid()) return false;
    actor.bones = SkeletonPose::to_mesh_bones(actor.skeleton);
    return true;
}
}
