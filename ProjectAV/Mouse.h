#pragma once
#include <queue>
#include <optional>

class Mouse
{
	friend class Window;
public:
	struct RawMovement
	{
		int dx, dy;
	};

	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x;
		int y;

	public:
		Event() noexcept
			:
			type(Type::Invalid),
			leftIsPressed(false),
			rightIsPressed(false),
			x(0),
			y(0)
		{
		}

		Event(Type type, const Mouse& parent) noexcept
			:
			type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			x(parent.x),
			y(parent.y)
		{
		}

		Type GetType() const noexcept { return type; }
		std::pair<int, int> GetPos() const noexcept { return{ x,y }; }
		int GetPosX() const noexcept { return x; }
		int GetPosY() const noexcept { return y; }
		bool IsLeftPressed() const noexcept { return leftIsPressed; }
		bool IsRightPressed() const noexcept { return rightIsPressed; }
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool IsLeftPressed() const noexcept;
	bool IsRightPressed() const noexcept;
	bool IsRawInputEnabled() const noexcept;
	void EnableRawInput() noexcept;
	void DisableRawInput() noexcept;

	std::optional<Mouse::Event> PollEvent() noexcept;
	std::optional<RawMovement> PollRawMovement() noexcept;
	bool IsEventBufferEmpty() const noexcept
	{
		return eventBuffer.empty();
	}
	void FlushEvents() noexcept;

private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnRawDelta(int dx, int dy) noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;

private:
	template<typename T>
	static void TrimQueue(std::queue<T>& queue) noexcept;
	void TrimEventBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;

private:
	static constexpr unsigned int bufferCapacity = 16u;
	int x = 0;
	int y = 0;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	int wheelDeltaAccumulator = 0;
	bool rawInputEnabled = false;
	std::queue<Event> eventBuffer;
	std::queue<RawMovement> rawMovementBuffer;
};