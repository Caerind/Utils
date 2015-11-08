#include "Path.hpp"

#include <regex>

// implementation file for non-OS specific functions

namespace gfs
{
	Path userHome();	// forward declaration
}

std::string normalizePath(const std::string& path)
{
	const std::regex slashes("([\\/\\\\]{2,})");	// shortens to: ([\/\\]{2,})
	
	return std::regex_replace(path, slashes, "/");
}

void fixUserHome(std::string& path)
{
	if(path.front() == '~')
		path.replace(0, 1, static_cast<const char*>(gfs::userHome()));
}

namespace gfs
{
	Path::Path()
	:	pathStr(""),
		typeVal(Type::Unknown),
		existsVal(false),
		permissionsVal(0)
	{}

	Path::Path(const std::string& path, bool resolveSymLink)
	:	pathStr(normalizePath(path)),
		typeVal(Type::Unknown),
		existsVal(false),
		permissionsVal(0)
	{
		fixUserHome(pathStr);
		checkPath(*this, resolveSymLink);
	}

	Path::Path(const char* path, bool resolveSymLink)
	:	pathStr(normalizePath(path)),
		typeVal(Type::Unknown),
		existsVal(false),
		permissionsVal(0)
	{
		fixUserHome(pathStr);
		checkPath(*this, resolveSymLink);
	}

	bool Path::exists()
	{
		checkPath(*this, true);
		return existsVal;
	}

	bool Path::exists() const
	{
		return existsVal;
	}

	Path::Type Path::type() const
	{
		return typeVal;
	}
	
	unsigned int Path::permissions() const
	{
		return permissionsVal;
	}
	
	Path Path::parent() const
	{
		if(pathStr.empty())
			return {};
			
		char last = pathStr.back();
		
		std::size_t lastDirDiv;
		
		if(last != '/' && last != '\\')
			lastDirDiv = pathStr.find_last_of("/\\");
		else
			lastDirDiv = pathStr.find_last_of("/\\", pathStr.size() - 2);
		
		if(pathStr.front() != '.')
		{
			if(lastDirDiv != std::string::npos)
				return {pathStr.substr(0, lastDirDiv + 1)};
		}
		else
		{
			if(lastDirDiv != std::string::npos)
				return {pathStr.substr(0, lastDirDiv + 1) + "../"};
		}
		
		return *this;
	}
	
	std::string Path::filename() const
	{
		if(pathStr.empty())
			return "";
		
		char last = pathStr.back();
		
		std::size_t lastDirDiv;
		
		if(last != '/' && last != '\\')
			lastDirDiv = pathStr.find_last_of("/\\");
		else
			lastDirDiv = pathStr.find_last_of("/\\", pathStr.size() - 2);
			
		if(lastDirDiv != std::string::npos)
			return pathStr.substr(lastDirDiv + 1);
		
		return pathStr;
	}
	
	std::string Path::name() const
	{
		if(pathStr.empty())
			return "";
		
		char last = pathStr.back();
		
		std::size_t lastDirDiv;
		
		if(last != '/' && last != '\\')
			lastDirDiv = pathStr.find_last_of("/\\");
		else
			lastDirDiv = pathStr.find_last_of("/\\", pathStr.size() - 2);
		
		std::size_t extDot = pathStr.find_last_of('.');
		
		if(lastDirDiv != std::string::npos && extDot != std::string::npos)
			return pathStr.substr(lastDirDiv + 1, extDot - lastDirDiv - 1);
		else if(lastDirDiv != std::string::npos)
			return pathStr.substr(lastDirDiv + 1);
		
		return pathStr;
	}
	
	std::string Path::extension() const
	{
		if(pathStr.empty())
			return "";
		
		std::size_t lastDirDiv = pathStr.find_last_of("/\\");
		std::size_t extDot = pathStr.find_last_of('.');
		
		if(lastDirDiv > extDot)
			return "";
		
		if(extDot != std::string::npos)
			return pathStr.substr(extDot + 1);
		
		return "";
	}

	Path::operator const char*() const
	{
		return pathStr.c_str();
	}
	
	Path::operator bool() const
	{
		return existsVal;
	}
	
	Path& Path::operator=(const Path& other)
	{
		this->pathStr = other.pathStr;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator=(const std::string& other)
	{
		this->pathStr = other;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator=(const char* other)
	{
		this->pathStr = other;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator+=(const Path& other)
	{
		this->pathStr += other.pathStr;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator+=(const std::string& other)
	{
		this->pathStr += other;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator+=(const char* other)
	{
		this->pathStr += other;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator/=(const Path& other)
	{
		this->pathStr += '/' + other.pathStr;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator/=(const std::string& other)
	{
		this->pathStr += '/' + other;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path& Path::operator/=(const char* other)
	{
		this->pathStr += '/' + other;
		checkPath(*this, true);
		
		return *this;
	}
	
	Path operator+(const Path& lhs, const Path& rhs)
	{
		return {lhs.pathStr + rhs.pathStr};
	}
	
	Path operator+(const std::string& lhs, const Path& rhs)
	{
		return {lhs + rhs.pathStr};
	}
	
	Path operator+(const Path& lhs, const std::string& rhs)
	{
		return {lhs.pathStr + rhs};
	}
	
	Path operator+(const char* lhs, const Path& rhs)
	{
		return {lhs + rhs.pathStr};
	}
	
	Path operator+(const Path& lhs, const char* rhs)
	{
		return {lhs.pathStr + rhs};
	}
	
	Path operator/(const Path& lhs, const Path& rhs)
	{
		return {lhs.pathStr + '/' + rhs.pathStr};
	}
	
	Path operator/(const std::string& lhs, const Path& rhs)
	{
		return {lhs + '/' + rhs.pathStr};
	}
	
	Path operator/(const Path& lhs, const std::string& rhs)
	{
		return {lhs.pathStr + '/' + rhs};
	}
	
	Path operator/(const char* lhs, const Path& rhs)
	{
		return {lhs + '/' + rhs.pathStr};
	}
	
	Path operator/(const Path& lhs, const char* rhs)
	{
		return {lhs.pathStr + '/' + rhs};
	}
	
	bool operator==(const Path& lhs, const Path& rhs)
	{
		return lhs.pathStr == rhs.pathStr;
	}
	
	bool operator!=(const Path& lhs, const Path& rhs)
	{
		return !(lhs.pathStr == rhs.pathStr);
	}
	
	std::ostream& operator<<(std::ostream& os, const Path& path)
	{
		os << path.pathStr;
		
		return os;
	}
}



#ifdef _WIN32

#include <ctime>
#include <windows.h>

std::time_t winFileTimeToTimeT(const FILETIME& ft)
{
	static const unsigned long long WINDOWS_TICK = 10000000LL;
	static const unsigned long long SEC_TO_UNIX_EPOCH = 11644473600LL;

	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	return ull.QuadPart / WINDOWS_TICK - SEC_TO_UNIX_EPOCH;
}

namespace gfs
{
	std::chrono::system_clock::time_point Path::lastAccess() const
	{
		WIN32_FILE_ATTRIBUTE_DATA data;

		if(existsVal && GetFileAttributesEx(pathStr.c_str(), GetFileExInfoStandard, &data))
		{
			std::time_t time = winFileTimeToTimeT(data.ftLastAccessTime);

			return std::chrono::system_clock::from_time_t(time);
		}

		return {};
	}
	
	std::chrono::system_clock::time_point Path::lastModify() const
	{
		WIN32_FILE_ATTRIBUTE_DATA data;

		if(existsVal && GetFileAttributesEx(pathStr.c_str(), GetFileExInfoStandard, &data))
		{
			std::time_t time = winFileTimeToTimeT(data.ftLastWriteTime);

			return std::chrono::system_clock::from_time_t(time);
		}

		return {};
	}
	
	unsigned long long int Path::fileSize() const
	{
		if(!existsVal)
			return 0;

		HANDLE file = CreateFile(*this, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		LARGE_INTEGER size;
		size.QuadPart = 0;
		GetFileSizeEx(file, &size);
		CloseHandle(file);

		return size.QuadPart;
	}
	
	void Path::checkPath(Path& path, bool resolveSymLink)
	{
		HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		DWORD attr = GetFileAttributes(path);
		DWORD type = GetFileType(file);

		if(file != INVALID_HANDLE_VALUE && attr != INVALID_FILE_ATTRIBUTES)
		{
			switch(type)
			{
				case FILE_TYPE_CHAR:
					path.typeVal = Type::Character;
					break;
				case FILE_TYPE_DISK:
					path.typeVal = Type::Block;
					break;
				case FILE_TYPE_PIPE:
					path.typeVal = Type::Pipe;
					break;
				default:
					path.typeVal = Type::Unknown;
					break;
			}

			if((attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
				path.typeVal = Type::Directory;
			else if((attr & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
				path.typeVal = Type::SymLink;
			else
				path.typeVal = Type::File;

			// ignore permissions on Windows... it's... convoluted. for now at least

			if(path.typeVal != Type::SymLink || resolveSymLink)
			{
				// get absolute path
				char buf[MAX_PATH];

				std::size_t rSize = GetFinalPathNameByHandle(file, buf, MAX_PATH, FILE_NAME_OPENED);

				path.pathStr = {buf, rSize};
			}

			// get rid of the unicode stuff on the front of string
			std::size_t driveColon = path.pathStr.find(':');
			if(driveColon != std::string::npos)
				path.pathStr.erase(0, driveColon - 1);

			// add directory divider to end of directory Paths
			if(path.typeVal == Type::Directory)
				path.pathStr += '\\';

			path.existsVal = true;
		}
		else
		{
			path.typeVal = Type::Unknown;
			path.existsVal = false;
		}
	}
}

#endif

#ifdef __linux

// implementation for Linux

#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace gfs
{
	const unsigned int Path::OwnerAll = S_IRWXU;
	const unsigned int Path::OwnerRead = S_IRUSR;
	const unsigned int Path::OwnerWrite = S_IWUSR;
	const unsigned int Path::OwnerExec = S_IXUSR;
	
	const unsigned int Path::GroupAll = S_IRWXG;
	const unsigned int Path::GroupRead = S_IRGRP;
	const unsigned int Path::GroupWrite = S_IWGRP;
	const unsigned int Path::GroupExec = S_IXGRP;
	
	const unsigned int Path::OthersAll = S_IRWXO;
	const unsigned int Path::OthersRead = S_IROTH;
	const unsigned int Path::OthersWrite = S_IWOTH;
	const unsigned int Path::OthersExec = S_IXOTH;
	
	std::chrono::system_clock::time_point Path::lastAccess() const
	{
		struct stat st;
		
		if(existsVal && !lstat(pathStr.c_str(), &st))
			return std::chrono::system_clock::from_time_t(st.st_atime);
		else
			return {};
	}
	
	std::chrono::system_clock::time_point Path::lastModify() const
	{
		struct stat st;
		
		if(existsVal && !lstat(pathStr.c_str(), &st))
			return std::chrono::system_clock::from_time_t(st.st_mtime);
		else
			return {};
	}
	
	unsigned long long int Path::fileSize() const
	{
		struct stat st;
		
		if(existsVal && !lstat(pathStr.c_str(), &st))
			return st.st_size;
		else
			return 0;
	}
	
	void Path::checkPath(Path& path, bool resolveSymLink)
	{
		struct stat st;
		
		if(!lstat(path, &st))
		{
			switch(st.st_mode & S_IFMT)
			{
				case S_IFSOCK:
					path.typeVal = Type::Socket;
					break;
				case S_IFLNK:
					path.typeVal = Type::SymLink;
					break;
				case S_IFREG:
					path.typeVal = Type::File;
					break;
				case S_IFBLK:
					path.typeVal = Type::Block;
					break;
				case S_IFDIR:
					path.typeVal = Type::Directory;
					break;
				case S_IFCHR:
					path.typeVal = Type::Character;
					break;
				case S_IFIFO:
					path.typeVal = Type::Pipe;
					break;
				default:
					path.typeVal = Type::Unknown;
					break;
			}

			if(!path.permissionsVal)
				path.permissionsVal += st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
			
			if(path.typeVal != Type::SymLink || resolveSymLink)
			{
				// get absolute path
				char buf[PATH_MAX];
				
				if(realpath(path, buf) != nullptr)
					path.pathStr = buf;
			}
			
			if(path.typeVal == Type::Directory)
				path.pathStr += "/";
	
			path.existsVal = true;
		}
		else
		{
			path.typeVal = Type::Unknown;
			path.existsVal = false;
		}
	}
}

#endif

