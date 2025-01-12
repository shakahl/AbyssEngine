#include "node.h"
#include "../engine/engine.h"
#include <utility>

namespace AbyssEngine {

int Node::_nodeCount = 0;

Node::Node() : _addChildQueue(), _removeChildQueue() {}

Node::Node(std::string_view name) : Name(name) { _nodeCount++; }

void Node::UpdateCallback(uint32_t ticks) {
    ProcessQueuedActions();

    if (_onUpdateHandler.valid()) {
        auto result = _onUpdateHandler((float)ticks / 1000.f);

        if (!result.valid()) {
            sol::error err = result;
            Engine::Get()->Panic(err.what());
        }
    }

    for (auto &item : Children) {
        if (!item->Active)
            continue;

        item->UpdateCallback(ticks);
    }
}

void Node::RenderCallback(int offsetX, int offsetY) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        item->RenderCallback(X + offsetX, Y + offsetY);
    }
}

void Node::GetEffectiveLayout(int &x, int &y) const {
    x += X;
    y += Y;

    if (Parent != nullptr) {
        Parent->GetEffectiveLayout(x, y);
    }
}

void Node::AppendChild(Node *childNode) {
    if (childNode == nullptr)
        throw std::runtime_error("Attempted to append null node!");

    childNode->Parent = this;
    _addChildQueue.push(childNode);
}

void Node::RemoveChild(Node *nodeRef) {
    if (nodeRef == nullptr)
        throw std::runtime_error("Attempted to remove null node!");

    nodeRef->Parent = nullptr;
    _removeChildQueue.push(nodeRef);
}

void Node::RemoveAllChildren() { _removeAllChildren = true; }

void Node::SetPosition(int x, int y) {
    X = x;
    Y = y;
}

std::tuple<int, int> Node::GetPosition() const { return {X, Y}; }

void Node::SetActive(bool active) { Active = active; }

bool Node::GetActive() const { return Active; }

void Node::SetVisible(bool visible) { Visible = visible; }

bool Node::GetVisible() const { return Visible; }

void Node::MouseEventCallback(const MouseEvent &event) {
    for (auto &item : Children) {
        if (!item->Active || !item->Visible)
            continue;

        if (item != this)
            item->MouseEventCallback(event);
    }
}

void Node::ProcessQueuedActions() {
    if (_removeAllChildren) {
        _removeAllChildren = false;
        Children.clear();
    }

    while (!_addChildQueue.empty()) {
        Children.push_back(_addChildQueue.front());
        _addChildQueue.pop();
    }

    while (!_removeChildQueue.empty()) {
        Children.erase(std::remove(Children.begin(), Children.end(), _removeChildQueue.front()), Children.end());
        _removeChildQueue.pop();
    }
}

void Node::DoInitialize() {
    if (!_initialized) {
        Initialize();
        _initialized = true;
    }

    for (auto &item : Children) {
        item->DoInitialize();
    }
}

void Node::Initialize() {}

void Node::SetLuaOnUpdateHandler(sol::protected_function onUpdateHandler) { _onUpdateHandler = std::move(onUpdateHandler); }

void Node::KeyboardEventCallback(const KeyboardEvent &event) {
    for (auto &item : Children) {
        if (!item->Active)
            continue;

        if (item != this)
            item->KeyboardEventCallback(event);
    }
}
Node::~Node() { _nodeCount--; }
std::vector<Node *> Node::GetChildren() { return Children; }
void Node::SetLuaTable(sol::table table) { _table = std::move(table); }
std::string_view Node::NodeType() const { return "Node"; }
} // namespace AbyssEngine
