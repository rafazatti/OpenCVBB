
#include "opencv2/contrib/contrib.hpp"

#ifdef WIN32
	#include <windows.h>
	#include <tchar.h>
#else
	#include <dirent.h>
#endif

namespace cv
{
	std::vector<std::string> Directory::GetListFiles(  const std::string& path, const std::string & exten, bool addPath )
	{
		std::vector<std::string> list;
		list.clear();
		std::string path_f = path + "/" + exten;
		#ifdef WIN32
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;

			hFind = FindFirstFile((LPCSTR)path_f.c_str(), &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE) 
			{
				return list;
			}
			else
			{
				do
				{
					if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_NORMAL  || 
						FindFileData.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE || 
						FindFileData.dwFileAttributes == FILE_ATTRIBUTE_HIDDEN  || 
						FindFileData.dwFileAttributes == FILE_ATTRIBUTE_SYSTEM  || 
						FindFileData.dwFileAttributes == FILE_ATTRIBUTE_READONLY)
					{
						if (addPath)
							list.push_back(path + "/" + FindFileData.cFileName);
						else
							list.push_back(FindFileData.cFileName);
					}
				} 
				while(FindNextFile(hFind, &FindFileData));
				FindClose(hFind);  
			}
		#else
			DIR *dp;
			struct dirent *dirp;
			if((dp = opendir(path.c_str())) == NULL) 
			{
				return list;
			}

		#ifdef __QNX__
			// you have to ask QNX to please include more file information
			// and not to report duplicate names as a result of file system unions
			if ( -1 == dircntl(dp, D_SETFLAG, D_FLAG_STAT|D_FLAG_FILTER) ) {
				return list;
			}
		#endif

			while ((dirp = readdir(dp)) != NULL) 
			{
			#ifdef __QNX__
				// QNX looks at the world a little differently
				dirent_extra *extra;
				dirent_extra_stat *extra_stat;
				for (extra = _DEXTRA_FIRST(dirp),
				     extra_stat = reinterpret_cast<dirent_extra_stat *>(extra); 
				     _DEXTRA_VALID(extra, dirp); 
			             extra = _DEXTRA_NEXT(extra),
				     extra_stat = reinterpret_cast<dirent_extra_stat *>(extra))
				if ((extra->d_type != _DTYPE_NONE) && S_ISREG(extra_stat->d_stat.st_mode))
			#else
				if (dirp->d_type == DT_REG)
			#endif
				{
					if (exten.compare("*") == 0)
						list.push_back(static_cast<std::string>(dirp->d_name));
					else
						if (std::string(dirp->d_name).find(exten) != std::string::npos)
							list.push_back(static_cast<std::string>(dirp->d_name));
				}
			}
			closedir(dp);
		#endif

		return list;
	}

	std::vector<std::string> Directory::GetListFolders( const std::string& path, const std::string & exten, bool addPath )
	{
		std::vector<std::string> list;
		std::string path_f = path + "/" + exten;
		list.clear();
		#ifdef WIN32
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;

			hFind = FindFirstFile((LPCSTR)path_f.c_str(), &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE) 
			{
				return list;
			}
			else
			{
				do
				{
					if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && 
						strcmp(FindFileData.cFileName, ".") != 0 && 
						strcmp(FindFileData.cFileName, "..") != 0)
					{
						if (addPath)
							list.push_back(path + "/" + FindFileData.cFileName);
						else
							list.push_back(FindFileData.cFileName);
					}
				} 
				while(FindNextFile(hFind, &FindFileData));
				FindClose(hFind);  
			}

		#else
			DIR *dp;
			struct dirent *dirp;
			if((dp = opendir(path_f.c_str())) == NULL) 
			{
				return list;
			}

		#ifdef __QNX__
			// you have to ask QNX to please include more file information
			// and not to report duplicate names as a result of file system unions
			if ( -1 == dircntl(dp, D_SETFLAG, D_FLAG_STAT|D_FLAG_FILTER) ) {
				return list;
			}
		#endif

			while ((dirp = readdir(dp)) != NULL) 
			{
			#ifdef __QNX__
				// QNX looks at the world a little differently
				dirent_extra *extra;
				dirent_extra_stat *extra_stat;
				for (extra = _DEXTRA_FIRST(dirp),
				     extra_stat = reinterpret_cast<dirent_extra_stat *>(extra); 
				     _DEXTRA_VALID(extra, dirp); 
				     extra = _DEXTRA_NEXT(extra),
				     extra_stat = reinterpret_cast<dirent_extra_stat *>(extra))
				if ((extra->d_type != _DTYPE_NONE) && 
				    S_ISDIR(extra_stat->d_stat.st_mode) &&
			#else
				if (dirp->d_type == DT_DIR &&
			#endif
					strcmp(dirp->d_name, ".") != 0 && 
					strcmp(dirp->d_name, "..") != 0 )
				{
					if (exten.compare("*") == 0)
						list.push_back(static_cast<std::string>(dirp->d_name));
					else
						if (std::string(dirp->d_name).find(exten) != std::string::npos)
							list.push_back(static_cast<std::string>(dirp->d_name));
				}
			}
			closedir(dp);
		#endif

		return list;
	}

	std::vector<std::string> Directory::GetListFilesR ( const std::string& path, const std::string & exten, bool addPath )
	{
		std::vector<std::string> list = Directory::GetListFiles(path, exten, addPath);

		std::vector<std::string> dirs = Directory::GetListFolders(path, exten, addPath);

		std::vector<std::string>::const_iterator it;
		for (it = dirs.begin(); it != dirs.end(); ++it)
		{
			std::vector<std::string> cl = Directory::GetListFiles(*it, exten, addPath);
			list.insert(list.end(), cl.begin(), cl.end());
		}

		return list;
	}

}
