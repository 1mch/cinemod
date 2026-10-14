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

    struct Ray {
        glm::vec3 origin{ 0.0f };
        glm::vec3 direction{ 0.0f, 0.0f, -1.0f };

        // Ray vs Axis-Aligned Bounding Box (AABB) intersection test
        bool IntersectsAABB(const glm::vec3& minBounds, const glm::vec3& maxBounds, float& tOut) const {
            float tmin = (minBounds.x - origin.x) / (direction.x != 0.0f ? direction.x : 1e-6f);
            float tmax = (maxBounds.x - origin.x) / (direction.x != 0.0f ? direction.x : 1e-6f);
            if (tmin > tmax) std::swap(tmin, tmax);

            float tymin = (minBounds.y - origin.y) / (direction.y != 0.0f ? direction.y : 1e-6f);
            float tymax = (maxBounds.y - origin.y) / (direction.y != 0.0f ? direction.y : 1e-6f);
            if (tymin > tymax) std::swap(tymin, tymax);

            if ((tmin > tymax) || (tymin > tmax)) return false;
            if (tymin > tmin) tmin = tymin;
            if (tymax < tmax) tmax = tymax;

            float tzmin = (minBounds.z - origin.z) / (direction.z != 0.0f ? direction.z : 1e-6f);
            float tzmax = (maxBounds.z - origin.z) / (direction.z != 0.0f ? direction.z : 1e-6f);
            if (tzmin > tzmax) std::swap(tzmin, tzmax);

            if ((tmin > tzmax) || (tzmin > tmax)) return false;
            if (tzmin > tmin) tmin = tzmin;
            if (tzmax < tmax) tmax = tzmax;

            if (tmax < 0.0f) return false;
            tOut = (tmin >= 0.0f) ? tmin : tmax;
            return true;
        }
    };

} // namespace CineMod
