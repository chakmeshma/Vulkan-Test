#include "Vulkan Engine Exception.h"


VulkanException::VulkanException(char const* const _Message)
{
	msg.resize(strlen(_Message));
	msg.assign(_Message);
}

char const* VulkanException::what() const
{
	return msg.data();
}