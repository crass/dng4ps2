#ifndef __P2_Handler_hpp__
#define __P2_Handler_hpp__	1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2007 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "XMP_Environment.h"	// ! This must be the first include.

#include "XMPFiles_Impl.hpp"

#include "ExpatAdapter.hpp"

#include "MD5.h"

// =================================================================================================
/// \file P2_Handler.hpp
/// \brief Folder format handler for P2.
///
/// This header ...
///
// =================================================================================================

// *** Could derive from Basic_Handler - buffer file tail in a temp file.

extern XMPFileHandler * P2_MetaHandlerCTor ( XMPFiles * parent );

extern bool P2_CheckFormat ( XMP_FileFormat format,
							 const std::string & rootPath,
							 const std::string & gpName,
							 const std::string & parentName,
							 const std::string & leafName,
							 XMPFiles * parent );

static const XMP_OptionBits kP2_HandlerFlags = (kXMPFiles_CanInjectXMP |
												kXMPFiles_CanExpand |
												kXMPFiles_CanRewrite |
												kXMPFiles_PrefersInPlace |
												kXMPFiles_CanReconcile |
												kXMPFiles_AllowsOnlyXMP |
												kXMPFiles_ReturnsRawPacket |
												kXMPFiles_HandlerOwnsFile |
												kXMPFiles_AllowsSafeUpdate |
												kXMPFiles_UsesSidecarXMP |
												kXMPFiles_FolderBasedFormat);

class P2_MetaHandler : public XMPFileHandler
{
public:

	void CacheFileData();
	void ProcessXMP();

	XMP_OptionBits GetSerializeOptions()	// *** These should be standard for standalone XMP files.
		{ return (kXMP_UseCompactFormat | kXMP_OmitPacketWrapper); };
	
	void UpdateFile ( bool doSafeUpdate );
    void WriteFile  ( LFA_FileRef sourceRef, const std::string & sourcePath );

	P2_MetaHandler ( XMPFiles * _parent );
	virtual ~P2_MetaHandler();

private:

	P2_MetaHandler() : expat(0), clipMetadata(0), clipContent(0) {};	// Hidden on purpose.
	
	void MakeClipFilePath ( std::string * path, XMP_StringPtr suffix );
	void MakeLegacyDigest ( std::string * digestStr );
	void CleanupLegacyXML();

	void DigestLegacyItem ( MD5_CTX & md5Context, XML_NodePtr legacyContext, XMP_StringPtr legacyPropName );
	void DigestLegacyRelations ( MD5_CTX & md5Context );

	void SetXMPPropertyFromLegacyXML ( bool digestFound,
									   XML_NodePtr legacyContext,
									   XMP_StringPtr schemaNS, 
									   XMP_StringPtr propName,
									   XMP_StringPtr legacyPropName,
									   bool isLocalized );

	void SetRelationsFromLegacyXML ( bool digestFound );
	void SetAudioInfoFromLegacyXML ( bool digestFound );
	void SetVideoInfoFromLegacyXML ( bool digestFound );
	void SetDurationFromLegacyXML  ( bool digestFound );

	void SetVideoFrameInfoFromLegacyXML ( XML_NodePtr legacyVideoContext, bool digestFound );
	void SetStartTimecodeFromLegacyXML  ( XML_NodePtr legacyVideoContext, bool digestFound );
	void SetGPSPropertyFromLegacyXML  ( XML_NodePtr legacyLocationContext, bool digestFound, XMP_StringPtr propName, XMP_StringPtr legacyPropName );
	void SetAltitudeFromLegacyXML  ( XML_NodePtr legacyLocationContext, bool digestFound );

	XML_Node * ForceChildElement ( XML_Node * parent, XMP_StringPtr localName, int indent = 0 );
	
	std::string rootPath, clipName, p2NS;
	
	ExpatAdapter * expat;
	XML_Node * clipMetadata;	// ! Don't delete, points into the Expat tree.
	XML_Node * clipContent;		// ! Don't delete, points into the Expat tree.
	
};	// P2_MetaHandler

// =================================================================================================

#endif /* __P2_Handler_hpp__ */
