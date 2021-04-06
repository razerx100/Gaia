#ifndef __GDI_PLUS_MANAGER_HPP__
#define __GDI_PLUS_MANAGER_HPP__
#include <CleanWin.hpp>

class GDIPlusManager {
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};

#endif
