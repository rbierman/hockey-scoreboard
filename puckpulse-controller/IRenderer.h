#pragma once

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void render() const = 0;
};
