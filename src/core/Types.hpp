#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace CineMod {

    // Cinema 4D Visibility modes (Traffic light dots in Object Manager)
    enum class VisibilityMode {
        Default = 0, // Undefined / Inherit from parent (Gray dot)
        Visible = 1, // Forced Visible (Green dot)
        Hidden  = 2  // Forced Hidden (Red dot)
    };

    // Cinema 4D Transform / Tool Modes
    enum class ToolMode {
        LiveSelection = 0, // '9'
        RectangleSelection, // '0'
        Move,              // 'E' or '4'
        Rotate,            // 'R' or '6'
        Scale              // 'T' or '5'
    };

    // Cinema 4D Coordinate System
    enum class CoordSystem {
        World,
        Object
    };

    // Cinema 4D Component Modes
    enum class EditMode {
        Model,
        Points,
        Edges,
        Polygons
    };

    // Primitive Object Types
    enum class ObjectType {
        Null,
        Cube,
        Sphere,
        Cylinder,
        Plane,
        Camera,
        Light
    };

    // Helper functions for HPB Euler angles (Cinema 4D Heading, Pitch, Bank in degrees)
    // H (Heading) = Rotation around Y axis
    // P (Pitch)   = Rotation around X axis
    // B (Bank)    = Rotation around Z axis
    inline glm::mat4 HPBToMatrix(const glm::vec3& hpbDegrees) {
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(hpbDegrees.x), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(hpbDegrees.y), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(hpbDegrees.z), glm::vec3(0.0f, 0.0f, 1.0f));
        return rotY * rotX * rotZ;
    }

} // namespace CineMod
