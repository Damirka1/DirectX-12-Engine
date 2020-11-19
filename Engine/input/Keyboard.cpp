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

std::optional<char> Keyboard::GetCharacters()
{
	if (Characters.size() > 0)
	{
		auto ev = Characters.front();
		Characters.pop();
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

void Keyboard::PopQueueEv()
{
	while (Events.size() > QueueSize)
		Events.pop();
}

void Keyboard::PopQueueCh()
{
	while (Characters.size() > QueueSize)
		Characters.pop();
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
