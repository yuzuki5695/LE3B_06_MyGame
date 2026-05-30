#pragma once
#include <Vector3.h>
#include <Vector4.h>

namespace MyEngine {
    namespace LineTypes {

        struct LineVertex {
            Vector3 position;
            Vector4 color;
        };

        struct LineData {
            Vector3 start;
            Vector3 end;
            Vector4 color;

            bool isActive = true;
        };
    }
}