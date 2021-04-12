#include <ImGuiMan.hpp>
#include <ImGuiImpl.hpp>

ImGuiMan::ImGuiMan() {
	ImGuiImpl::ImGuiInitContext();
}

ImGuiMan::~ImGuiMan() {
	ImGuiImpl::ImGuiDestroyContext();
}
