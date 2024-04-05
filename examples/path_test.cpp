
#include <iostream>
#include "../include/PathUtil.hpp"

int main() {

    {
        // Path simplification
        std::cerr << "\n** PATH SIMPLIFICATION: \n";
        std::vector<std::string> paths = {
                "////",
                "////foo",
                "/////foo",
                "../../../././././/////foo",
                "../../",
                "/../../",
                "foo/bar/../",
                "/////foo/////././././bar/../bar/..///../../",
                "./foo/.../........"
        };

        for (const auto &path: paths) {
            std::cout << "  - \"" << path << "\" --> \"" << simgrid::module::fs::PathUtil::simplify_path_string(path)
                      << "\"\n";
        }
    }

    {
        // Path goes up
        std::cerr << "\n** PATH GOES UP: \n";
        std::vector<std::string> paths = {
                "/../../",
                "../foo/",
                "/a/b/c/../../../",
                "/a/b/c/../../../../",
                "/aa/bb/cc/dd/../../ee/ff/"
        };

        for (const auto &path: paths) {
            auto simplified_path = simgrid::module::fs::PathUtil::simplify_path_string(path);
            std::cout << "  - \"" << path << "\" --> \"" << simplified_path << "\" : \"" <<
                      (simgrid::module::fs::PathUtil::goes_up(simplified_path) ? "TRUE" : "FALSE") << "\"\n";
        }
    }


    {
        // Path is absolute
        std::cerr << "\n** PATH IS ABSOLUTE: \n";
        std::vector<std::string> paths = {
                "/../../",
                "../foo/",
                "/a/b/c/../../../",
                "/a/b/c/../../../../",
                "/aa/bb/cc/dd/../../ee/ff/"
        };

        for (const auto &path: paths) {
            auto simplified_path = simgrid::module::fs::PathUtil::simplify_path_string(path);
            std::cout << "  - \"" << path << "\" --> \"" << simplified_path << "\" : \"" <<
                      (simgrid::module::fs::PathUtil::is_absolute(simplified_path) ? "TRUE" : "FALSE") << "\"\n";
        }
    }


    {
        // Path at mount point
        std::cerr << "\n** PATH AT MOUNT POINT: \n";
        std::vector<std::string> paths = {
                "////../",
                "/dev/a/foo/bar/",
                "/dev/a/foo/../bar/",
                "/dev/a////foo/../../",
                "/dev/b/foo/../bar/bar2/",
                "/dev/b/foo/../bar/bar2",
                "/foo/dev/b/foo/bar/../",
                "/dev/a/../a/fioo"
        };
        for (const auto &path: paths) {
            auto simplified_path = simgrid::module::fs::PathUtil::simplify_path_string(path);
            std::string path_at_mount_point;
            try {
                path_at_mount_point = simgrid::module::fs::PathUtil::path_at_mount_point(simplified_path, "/dev/a");
            } catch (std::logic_error &e) {
                path_at_mount_point = "Exception";
            }
            std::cout << "  - \"" << path << "\" --> \"" << simplified_path << R"(" at mount point "/dev/a": ")" << path_at_mount_point << "\"\n";
        }
    }


}
