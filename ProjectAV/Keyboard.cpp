#include "Keyboard.h"

bool Keyboard::IsKeyPressed(unsigned char keycode) const noexcept
{
	return keyStates[keycode];
}

std::optional<Keyboard::Event> Keyboard::PollKeyEvent() noexcept
{
	if (keyBuffer.empty())
	{
		return std::nullopt;
	}
	Keyboard::Event e = keyBuffer.front();
	keyBuffer.pop();
	return e;
}

bool Keyboard::IsKeyBufferEmpty() const noexcept
{
	return keyBuffer.empty();
}

std::optional<char> Keyboard::PollChar() noexcept
{
	if (charBuffer.empty())
	{
		return std::nullopt;
	}
	unsigned char charcode = charBuffer.front();
	charBuffer.pop();
	return charcode;
}

bool Keyboard::IsCharBufferEmpty() const noexcept
{
	return charBuffer.empty();
}

void Keyboard::FlushKeyEvents() noexcept
{
	keyBuffer = {};
}

void Keyboard::FlushChars() noexcept
{
	charBuffer = {};
}

void Keyboard::FlushAll() noexcept
{
	FlushKeyEvents();
	FlushChars();
}

void Keyboard::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::IsAutorepeatEnabled() const noexcept
{
	return autorepeatEnabled;
}

bool Keyboard::IsJustPressed(unsigned char keycode) const noexcept
{
	// Jest wciœniêty teraz, ale nie by³ w poprzedniej klatce
	return keyStates[keycode] && !lastKeyStates[keycode];
}

bool Keyboard::IsJustReleased(unsigned char keycode) const noexcept
{
	// Nie jest wciœniêty teraz, ale by³ w poprzedniej klatce
	return !keyStates[keycode] && lastKeyStates[keycode];
}

void Keyboard::UpdateFrameState() noexcept
{
	// Na koniec klatki, obecny stan staje siê starym stanem
	lastKeyStates = keyStates;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keyStates[keycode] = true;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keyStates[keycode] = false;
	keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	charBuffer.push(character);
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferCapacity)
	{
		buffer.pop();
	}
}