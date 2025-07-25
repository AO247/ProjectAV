#pragma once
#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() noexcept : type(Type::Invalid), code(0) {}
		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code)
		{
		}
		bool IsPress() const noexcept { return type == Type::Press; }
		bool IsRelease() const noexcept { return type == Type::Release; }
		unsigned char GetCode() const noexcept { return code; }
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	bool IsKeyPressed(unsigned char keycode) const noexcept;

	std::optional<Event> PollKeyEvent() noexcept;
	bool IsKeyBufferEmpty() const noexcept;
	void FlushKeyEvents() noexcept;

	std::optional<char> PollChar() noexcept;
	bool IsCharBufferEmpty() const noexcept;
	void FlushChars() noexcept;

	void FlushAll() noexcept;

	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool IsAutorepeatEnabled() const noexcept;

	bool IsJustPressed(unsigned char keycode) const noexcept;
	bool IsJustReleased(unsigned char keycode) const noexcept;

	// === NOWA METODA PUBLICZNA (do wywo�ania raz na klatk�) ===
	void UpdateFrameState() noexcept;

private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;

private:
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	static constexpr unsigned int keyCount = 256u;
	static constexpr unsigned int bufferCapacity = 16u;
	bool autorepeatEnabled = false;
	std::bitset<keyCount> keyStates;
	std::bitset<keyCount> lastKeyStates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
};