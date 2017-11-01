#pragma once

#include <string>

struct VulkanException : public std::exception {
public:
	explicit VulkanException(char const* const _Message);

	char const* what() const;

private:
	std::string msg;
};
