#include "..\Headers\Input\Keyboard.h"

Keyboard::Keyboard(unsigned int QueueSize)
	:
	QueueSize(QueueSize)
{
}

bool Keyboard::KeyIsPressed(const char* KeyCode)
{
	return keys[*KeyCode];
}

std::optional<Keyboard::KeyEvent> Keyboard::GetEvent()
{
	if (Events.size() > 0)
	{
		auto ev = Events.front();
		Events.pop();
		return ev;
	}
	return std::nullopt;
}

void Keyboard::HandleMsg(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam)
{
	switch (msg)
	{
	case WM_KILLFOCUS:
		ClearState();
		break;
	case WM_CHAR:
	case WM_KEYDOWN:
	{
		keys[wParam] = true;
		Events.emplace(static_cast<unsigned char>(wParam), Keyboard::KeyEvent::Type::Pressed);
		PopQueue();
		return;
	}
	case WM_KEYUP:
	{
		keys[wParam] = false;
		Events.emplace(static_cast<unsigned char>(wParam), Keyboard::KeyEvent::Type::Released);
		PopQueue();
		return;
	}
	default:
		return;
	}
}

void Keyboard::PopQueue()
{
	while (Events.size() > QueueSize)
		Events.pop();
}

void Keyboard::ClearState()
{
	Events = std::queue<KeyEvent>();
}

Keyboard::KeyEvent::KeyEvent()
	:
	Code(0),
	t(Type::Undefined)
{
}

Keyboard::KeyEvent::KeyEvent(unsigned char Code, Type type)
	:
	Code(Code),
	t(type)
{
}

bool Keyboard::KeyEvent::operator==(const char* Key)
{
	if (t == Type::Pressed && Code == *Key)
		return true;
	return false;
}
