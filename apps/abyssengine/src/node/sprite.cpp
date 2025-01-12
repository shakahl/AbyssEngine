#include "sprite.h"

#include "../common/overload.h"
#include "../engine/engine.h"
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <utility>

namespace AbyssEngine {

void Sprite::UpdateCallback(uint32_t ticks) {
    if (!Active || !(_playMode == ePlayMode::Backwards || _playMode == ePlayMode::Forwards)) {
        Node::UpdateCallback(ticks);
        return;
    }

    Animate((float)ticks / 1000.f);

    Node::UpdateCallback(ticks);
}

void Sprite::RenderCallback(int offsetX, int offsetY) {
    if (!Visible)
        return;

    int imgX = X + offsetX;
    int imgY = Y + offsetY;

    if (_bottomOrigin) {
        uint32_t frameWidth;
        uint32_t frameHeight;

        _image.GetFrameSize(_currentFrame, _cellSizeX, frameWidth, frameHeight);
        imgY -= (int)frameHeight;
    }

    _image.Render(_currentFrame, _cellSizeX, _cellSizeY, imgX, imgY);

    Node::RenderCallback(offsetX, offsetY);
}

void Sprite::MouseEventCallback(const MouseEvent &event) {
    if (!Active)
        return;

    std::visit(Overload{[this](const MouseMoveEvent &evt) {
                            const int mx = evt.X, my = evt.Y;
                            const int sx = X, sy = Y;
                            int sx2 = sx, sy2 = sy;
                            uint32_t fx, fy;

                            _image.GetFrameSize(_currentFrame, _cellSizeX, fx, fy);

                            sx2 += (int)fx;
                            sy2 += (int)fy;

                            if ((mx < sx) || (mx >= sx2) || (my < sy) || (my >= sy2)) {
                                if (!_mouseInSprite)
                                    return;

                                _mouseInSprite = false;

                                if (!_mouseLeaveHandler.valid())
                                    return;

                                auto result1 = _mouseLeaveHandler();

                                if (result1.valid())
                                    return;

                                sol::error err1 = result1;
                                throw std::runtime_error(err1.what());
                            }

                            if (!_mouseInSprite) {
                                _mouseInSprite = true;
                                if (!_mouseEnterHandler.valid())
                                    return;

                                auto result1 = _mouseEnterHandler();

                                if (result1.valid())
                                    return;

                                sol::error err11 = result1;
                                throw std::runtime_error(err11.what());
                            }

                            if (!_mouseMoveHandler.valid())
                                return;

                            auto result = _mouseMoveHandler();

                            if (result.valid())
                                return;

                            sol::error err = result;
                            throw std::runtime_error(err.what());
                        },
                        [this](const MouseButtonEvent &evt) {
                            if (!_mouseInSprite)
                                return;

                            if (evt.IsPressed && _mouseButtonDownHandler.valid()) {
                                auto result = _mouseButtonDownHandler();

                                if (result.valid())
                                    return;

                                sol::error err1 = result;
                                throw std::runtime_error(err1.what());

                            } else if (!evt.IsPressed && _mouseButtonUpHandler.valid()) {
                                auto result = _mouseButtonUpHandler();

                                if (result.valid())
                                    return;

                                sol::error err = result;
                                throw std::runtime_error(err.what());
                            }
                        }},
               event);

    Node::MouseEventCallback(event);
}

void Sprite::Animate(float elapsed) {
    if (_playMode == ePlayMode::Unknown || _playMode == ePlayMode::Paused)
        return;

    const auto frameCount = _image.GetFramesPerAnimation();
    const auto frameLength = _playLength / (float)frameCount;
    _lastFrameTime += elapsed;
    const auto framesAdvanced = (int)(_lastFrameTime / frameLength);
    _lastFrameTime -= (float)framesAdvanced * frameLength;

    for (auto i = 0; i < framesAdvanced; i++)
        AdvanceFrame();
}

void Sprite::AdvanceFrame() {
    if (_playMode == ePlayMode::Unknown || _playMode == ePlayMode::Paused)
        return;

    const auto startIndex = 0;
    const auto endIndex = _image.GetFramesPerAnimation();

    switch (_playMode) {
    case ePlayMode::Forwards: {
        _currentFrame++;

        if (_currentFrame < endIndex)
            return;

        _currentFrame = _loopAnimation ? startIndex : endIndex - 1;

        if (startIndex != endIndex && _onAnimationFinished.valid()) {
            auto result = _onAnimationFinished();

            if (result.valid())
                return;

            sol::error err = result;
            Engine::Get()->Panic(err.what());
        }
    }
        return;
    case ePlayMode::Backwards: {
        if (_currentFrame != 0) {
            _currentFrame--;
            return;
        }

        _currentFrame = _loopAnimation ? endIndex - 1 : startIndex;

        if (startIndex != endIndex && _onAnimationFinished.valid()) {
            auto result = _onAnimationFinished();

            if (result.valid())
                return;

            sol::error err = result;
            Engine::Get()->Panic(err.what());
        }
    }
    default:
        throw std::runtime_error("Unimplemented play mode");
    }
}
std::tuple<uint32_t, uint32_t> Sprite::GetCellSize() { return {_cellSizeX, _cellSizeY}; }
void Sprite::SetCellSize(const int x, const int y) {
    _cellSizeX = x;
    _cellSizeY = y;
}
void Sprite::SetLuaMouseButtonDownHandler(sol::protected_function mouseButtonDownHandler) {
    _mouseButtonDownHandler = std::move(mouseButtonDownHandler);
}

void Sprite::SetLuaMouseButtonUpHandler(sol::protected_function mouseButtonUpHandler) { _mouseButtonUpHandler = std::move(mouseButtonUpHandler); }

std::string_view Sprite::LuaGetBlendMode() { return BlendModeToString(_image.GetBlendMode()); }

void Sprite::LuaSetBlendMode(std::string_view val) { _image.SetBlendMode(StringToBlendMode(val)); }

void Sprite::SetIsBottomOrigin(bool val) { _bottomOrigin = val; }

bool Sprite::GetIsBottomOrigin() const { return _bottomOrigin; }

void Sprite::LuaSetPlayMode(std::string_view mode) {
    auto str = absl::AsciiStrToLower(mode);

    if (str == "paused")
        _playMode = ePlayMode::Paused;
    else if (str == "forwards")
        _playMode = ePlayMode::Forwards;
    else if (str == "backwards")
        _playMode = ePlayMode::Backwards;
    else if (str.empty())
        _playMode = ePlayMode::Unknown;
    else
        throw std::runtime_error(absl::StrCat("Unknown play mode: ", mode));
}
std::string_view Sprite::LuaGetPlayMode() {
    switch (_playMode) {

    case ePlayMode::Unknown:
        return "";
    case ePlayMode::Paused:
        return "paused";
    case ePlayMode::Forwards:
        return "forwards";
    case ePlayMode::Backwards:
        return "backwards";
    }
    return "";
}
void Sprite::SetLuaAnimationFinishedHandler(sol::protected_function animationFinishedHandler) {
    _onAnimationFinished = std::move(animationFinishedHandler);
}

void Sprite::SetLoopAnimation(bool loop) { _loopAnimation = loop; }

bool Sprite::GetLoopAnimation() { return _loopAnimation; }
std::string_view Sprite::NodeType() const { return "Sprite Node"; }
uint32_t Sprite::GetCurrentFrameIndex() const { return _currentFrame; }
void Sprite::SetCurrentFrameIndex(uint32_t frame) { _currentFrame = frame; }

} // namespace AbyssEngine
