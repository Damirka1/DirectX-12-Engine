#include "..\Headers\Input\Keyboard.h"

Keyboard::Keyboard(unsigned int QueueSize) noexcept
	:
	QueueSize(QueueSize)
{
}

bool Keyboard::KeyIsPressed(const char KeyCode) noexcept
{
	return keys[KeyCode];
}

std::optional<Keyboard::KeyEvent> Keyboard::GetEvent() noexcept
{
	if (Events.size() > 0)
	{
		auto ev = Events.front();
		Events.pop();
		return ev;
	}
	return std::nullopt;
}

std::optional<char> Keyboard::GetCharacters() noexcept
{
	if (Characters.size() > 0)
	{
		auto ev = Characters.front();
		Characters.pop();
		return ev;
	}
	return std::nullopt;
}

void Keyboard::HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam) noexcept
{
	switch (msg)
	{
	case WM_KILLFOCUS:
		ClearState();
		break;
	case WM_CHAR:
	{
		Characters.push(static_cast<unsigned char>(wParam));
		PopQueueCh();
		break;
	}
	case WM_KEYDOWN:
	{
		keys[wParam] = true;
		Events.emplace(static_cast<unsigned char>(wParam), Keyboard::KeyEvent::Type::Pressed);
		PopQueueEv();
		return;
	}
	case WM_KEYUP:
	{
		keys[wParam] = false;
		Events.emplace(static_cast<unsigned char>(wParam), Keyboard::KeyEvent::Type::Released);
		PopQueueEv();
		return;
	}
	default:
		return;
	}
}

void Keyboard::PopQueueEv() noexcept
{
	while (Events.size() > QueueSize)
		Events.pop();
}

void Keyboard::PopQueueCh() noexcept
{
	while (Characters.size() > QueueSize)
		Characters.pop();
}

void Keyboard::ClearState() noexcept
{
	Events = std::queue<KeyEvent>();
}

Keyboard::KeyEvent::KeyEvent() noexcept
	:
	Code(0),
	t(Type::Undefined)
{
}

Keyboard::KeyEvent::KeyEvent(unsigned char Code, Type type) noexcept
	:
	Code(Code),
	t(type)
{
}

bool Keyboard::KeyEvent::operator==(const char Key) noexcept
{
	if (t == Type::Pressed && Code == Key)
		return true;
	return false;
}
