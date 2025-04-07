// WindowMessagingSystem.h
#pragma once
#include <queue>
#include <string>
#include <unordered_map>
#include <any>

struct WindowMessage {
	std::string windowType;
	std::unordered_map<std::string, std::any> parameters;
};

class WindowMessagingSystem {
public:
	static WindowMessagingSystem& GetInstance() {
		static WindowMessagingSystem instance;
		return instance;
	}

	void Enqueue(const WindowMessage& message) {
		m_messages.push(message);
	}

	bool Dequeue(WindowMessage& message) {
		if (m_messages.empty()) return false;
		message = m_messages.front();
		m_messages.pop();
		return true;
	}

private:
	std::queue<WindowMessage> m_messages;
};