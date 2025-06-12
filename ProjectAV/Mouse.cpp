#include "Win.h"
#include "Mouse.h"

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { x,y };
}

int Mouse::GetPosX() const noexcept
{
	return x;
}

int Mouse::GetPosY() const noexcept
{
	return y;
}

bool Mouse::IsInWindow() const noexcept
{
	return isInWindow;
}

bool Mouse::IsLeftPressed() const noexcept
{
	return leftIsPressed;
}

bool Mouse::IsRightPressed() const noexcept
{
	return rightIsPressed;
}

bool Mouse::IsRawInputEnabled() const noexcept
{
	return rawInputEnabled;
}

void Mouse::EnableRawInput() noexcept
{
	rawInputEnabled = true;
}

void Mouse::DisableRawInput() noexcept
{
	rawInputEnabled = false;
}

std::optional<Mouse::Event> Mouse::PollEvent() noexcept
{
	if (eventBuffer.empty())
	{
		return std::nullopt;
	}
	Mouse::Event e = eventBuffer.front();
	eventBuffer.pop();
	return e;
}

std::optional<Mouse::RawMovement> Mouse::PollRawMovement() noexcept
{
	if (rawMovementBuffer.empty())
	{
		return std::nullopt;
	}
	const RawMovement delta = rawMovementBuffer.front();
	rawMovementBuffer.pop();
	return delta;
}

void Mouse::FlushEvents() noexcept
{
	eventBuffer = {};
	rawMovementBuffer = {};
}

void Mouse::OnMouseMove(int newX, int newY) noexcept
{
	x = newX;
	y = newY;
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimEventBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimEventBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimEventBuffer();
}

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
	rawMovementBuffer.push({ dx, dy });
	TrimRawInputBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	leftIsPressed = true;
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimEventBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	leftIsPressed = false;
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimEventBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	rightIsPressed = true;
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimEventBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	rightIsPressed = false;
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimEventBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimEventBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	eventBuffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimEventBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	wheelDeltaAccumulator += delta;
	while (wheelDeltaAccumulator >= WHEEL_DELTA)
	{
		wheelDeltaAccumulator -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (wheelDeltaAccumulator <= -WHEEL_DELTA)
	{
		wheelDeltaAccumulator += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

template<typename T>
void Mouse::TrimQueue(std::queue<T>& queue) noexcept
{
	while (queue.size() > bufferCapacity)
	{
		queue.pop();
	}
}

void Mouse::TrimEventBuffer() noexcept
{
	TrimQueue(eventBuffer);
}

void Mouse::TrimRawInputBuffer() noexcept
{
	TrimQueue(rawMovementBuffer);
}