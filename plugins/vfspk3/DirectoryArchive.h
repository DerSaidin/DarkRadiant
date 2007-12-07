#ifndef DIRECTORYARCHIVE_H_
#define DIRECTORYARCHIVE_H_

#include "iarchive.h"

/**
 * greebo: This wraps around a certain path in the "real"
 *         filesystem on the user's hard drive. 
 * 
 * A real folder is treated like any other "archive" and gets
 * added to the list of PK4 archives, using this class.
 */
class DirectoryArchive : 
	public Archive
{
	std::string _root;
public:
	// Pass the root path to the constructor
	DirectoryArchive(const std::string& root);

	void release();
	
	virtual ArchiveFile* openFile(const char* name);
	
	virtual ArchiveTextFile* openTextFile(const char* name);
	
	virtual bool containsFile(const char* name);
	
	virtual void forEachFile(VisitorFunc visitor, const char* root);
};

#endif /*DIRECTORYARCHIVE_H_*/