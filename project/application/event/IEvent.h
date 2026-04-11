#pragma once
#include <EventTypes.h>

namespace MyGame {

    class IEvent {
    public:
        virtual ~IEvent() = default;

        virtual void Initialize() = 0;
        virtual void Update() = 0;
        virtual void Draw2D() = 0;

        virtual bool IsFinished() const = 0;
    };
}