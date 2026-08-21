#include "aethera/scene.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace aethera {
namespace {

void write_string(std::ostream& out, const std::string& value) {
    out << std::quoted(value);
}

bool read_string(std::istream& in, std::string& value) {
    return static_cast<bool>(in >> std::quoted(value));
}

} // namespace

Scene capture_scene(const ImageObject& object, const std::string& name,
                    const std::string& image_asset) {
    Scene scene;
    scene.name = name;
    scene.image_asset = image_asset;
    scene.nodes.reserve(object.nodes().size());

    for (const auto& node : object.nodes()) {
        SceneNodeData data;
        data.name = node.name;
        data.parent = node.parent;
        data.visible = node.visible;
        data.transform = node.local;
        data.visual = node.visual;
        scene.nodes.push_back(std::move(data));
    }
    return scene;
}

bool apply_scene(const Scene& scene, ImageObject& object) {
    if (scene.nodes.size() != object.nodes().size()) return false;

    for (std::size_t i = 0; i < scene.nodes.size(); ++i) {
        const auto& data = scene.nodes[i];
        auto& node = object.nodes()[i];
        if (node.name != data.name) return false;
        node.parent = data.parent;
        node.visible = data.visible;
        node.local = data.transform;
        node.visual = data.visual;
    }
    object.update_world_transforms();
    return true;
}

bool SceneSerializer::save(const Scene& scene, const std::string& path,
                           std::string* error) const {
    std::ofstream out(path);
    if (!out) {
        if (error) *error = "cannot open scene for writing";
        return false;
    }

    out << "AETHERA_SCENE 1\n";
    out << "name "; write_string(out, scene.name); out << '\n';
    out << "image "; write_string(out, scene.image_asset); out << '\n';
    out << "nodes " << scene.nodes.size() << '\n';

    for (const auto& node : scene.nodes) {
        out << "node "; write_string(out, node.name);
        out << ' ' << node.parent << ' ' << (node.visible ? 1 : 0);
        out << ' ' << node.transform.position.x << ' ' << node.transform.position.y;
        out << ' ' << node.transform.rotation;
        out << ' ' << node.transform.scale.x << ' ' << node.transform.scale.y;
        out << ' ' << node.visual.source.x << ' ' << node.visual.source.y;
        out << ' ' << node.visual.source.width << ' ' << node.visual.source.height;
        out << ' ' << node.visual.pivot.x << ' ' << node.visual.pivot.y;
        out << ' ' << node.visual.opacity;
        out << '\n';
    }

    if (!out) {
        if (error) *error = "scene write failed";
        return false;
    }
    return true;
}

bool SceneSerializer::load(const std::string& path, Scene& scene,
                           std::string* error) const {
    std::ifstream in(path);
    if (!in) {
        if (error) *error = "cannot open scene for reading";
        return false;
    }

    std::string magic;
    int version = 0;
    if (!(in >> magic >> version) || magic != "AETHERA_SCENE" || version != 1) {
        if (error) *error = "unsupported or invalid scene header";
        return false;
    }

    scene = {};
    std::string key;
    if (!(in >> key) || key != "name" || !read_string(in, scene.name)) {
        if (error) *error = "invalid scene name";
        return false;
    }
    if (!(in >> key) || key != "image" || !read_string(in, scene.image_asset)) {
        if (error) *error = "invalid scene image";
        return false;
    }

    std::size_t count = 0;
    if (!(in >> key >> count) || key != "nodes") {
        if (error) *error = "invalid node count";
        return false;
    }

    scene.nodes.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        SceneNodeData data;
        int visible = 0;
        std::string node_key;
        if (!(in >> node_key) || node_key != "node") {
            if (error) *error = "invalid node record";
            return false;
        }
        if (!read_string(in, data.name) || !(in >> data.parent >> visible)) return false;
        data.visible = visible != 0;
        if (!(in >> data.transform.position.x >> data.transform.position.y
                >> data.transform.rotation
                >> data.transform.scale.x >> data.transform.scale.y
                >> data.visual.source.x >> data.visual.source.y
                >> data.visual.source.width >> data.visual.source.height
                >> data.visual.pivot.x >> data.visual.pivot.y
                >> data.visual.opacity)) {
            if (error) *error = "invalid node transform";
            return false;
        }
        data.visual.name = data.name;
        scene.nodes.push_back(std::move(data));
    }

    return true;
}

} // namespace aethera
