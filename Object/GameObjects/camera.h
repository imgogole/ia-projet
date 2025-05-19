#pragma once

#include <cmath>

class Camera : public Object
{
public:
    Vector2D WorldToScreen(const Vector2D& worldPos) const
    {
        Vector2D position = this->GetWorldPosition();
        float dx = worldPos.x - position.x;
        float dy = worldPos.y - position.y;
        float rad = -GetRotation() * PI / 180.0f;
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);
        float rx = dx * cosR - dy * sinR;
        float ry = dx * sinR + dy * cosR;
        return Vector2D{
            viewport.x * 0.5f + rx,
            viewport.y * 0.5f + ry
        };
    }

    void SetViewportSize(float screenWidth, float screenHeight)
    {
        viewport = {screenWidth, screenHeight};
    }

    Vector2D GetViewportSize()
    {
        return viewport;
    }

    void Update(float deltaTime) {}

    void Render(SDL_Renderer* renderer, const Vector2D& givenPosition) {}

private:
    Vector2D viewport;
};
