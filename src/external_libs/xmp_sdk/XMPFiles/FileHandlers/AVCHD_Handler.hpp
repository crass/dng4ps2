#ifndef __AVCHD_Handler_hpp__
#define __AVCHD_Handler_hpp__	1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "XMP_Environment.h"	// ! This must be the first include.

#include "XMPFiles_Impl.hpp"

#include "ExpatAdapter.hpp"

// =================================================================================================
/// \file AVCHD_Handler.hpp
/// \brief Folder format handler for AVCHD.
///
/// This header ...
///
// =================================================================================================

extern XMPFileHandler * AVCHD_MetaHandlerCTor ( XMPFiles * parent );

extern bool AVCHD_CheckFormat ( XMP_FileFormat format,
								const std::string & rootPath,
								const std::string & gpName,
								const std::string & parentName,
								const std::string & leafName,
								XMPFiles * parent );

static const XMP_OptionBits kAVCHD_HandlerFlags = (kXMPFiles_CanInjectXMP |
												   kXMPFiles_CanExpand |
												   kXMPFiles_CanRewrite |
												   kXMPFiles_PrefersInPlace |
												   kXMPFiles_CanReconcile |
												   kXMPFiles_AllowsOnlyXMP |
												   kXMPFiles_ReturnsRawPacket |
												   kXMPFiles_HandlerOwnsFile |
												   kXMPFiles_AllowsSafeUpdate |
												   kXMPFiles_FolderBasedFormat);

class AVCHD_MetaHandler : public XMPFileHandler
{
public:

	void CacheFileData();
	void ProcessXMP();

	XMP_OptionBits GetSerializeOptions()	// *** These should be standard for standalone XMP files.
		{ return (kXMP_UseCompactFormat | kXMP_OmitPacketWrapper); };
	
	void UpdateFile ( bool doSafeUpdate );
    void WriteFile  ( LFA_FileRef sourceRef, const std::string & sourcePath );

	AVCHD_MetaHandler ( XMPFiles * _parent );
	virtual ~AVCHD_MetaHandler();

private:

	AVCHD_MetaHandler() {};	// Hidden on purpose.

	bool MakeClipInfoPath ( std::string * path, XMP_StringPtr suffix, bool checkFile = false ) const;
	bool MakeClipStreamPath ( std::string * path, XMP_StringPtr suffix, bool checkFile = false ) const;
	bool MakePlaylistPath ( std::string * path, XMP_StringPtr suffix, bool checkFile = false ) const;

	void MakeLegacyDigest ( std::string * digestStr );
	
	std::string rootPath, clipName;

};	// AVCHD_MetaHandler

// =================================================================================================

#endif /* __AVCHD_Handler_hpp__ */
