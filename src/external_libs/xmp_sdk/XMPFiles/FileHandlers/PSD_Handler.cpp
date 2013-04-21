// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2006 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "PSD_Handler.hpp"

#include "TIFF_Support.hpp"
#include "PSIR_Support.hpp"
#include "IPTC_Support.hpp"
#include "ReconcileLegacy.hpp"
#include "Reconcile_Impl.hpp"

#include "MD5.h"

using namespace std;

// =================================================================================================
/// \file PSD_Handler.cpp
/// \brief File format handler for PSD (Photoshop).
///
/// This handler ...
///
// =================================================================================================

// =================================================================================================
// PSD_CheckFormat
// ===============

// For PSD we just check the "8BPS" signature, the following version, and that the file is at least
// 34 bytes long. This covers the 26 byte header, the 4 byte color mode section length (which might
// be 0), and the 4 byte image resource section length (which might be 0). The parsing logic in
// CacheFileData will do further checks that the image resources actually exist. Those checks are
// not needed to decide if this is a PSD file though, instead they decide if this is valid PSD.

// ! The CheckXyzFormat routines don't track the filePos, that is left to ScanXyzFile.

bool PSD_CheckFormat ( XMP_FileFormat format,
	                   XMP_StringPtr  filePath,
                       LFA_FileRef    fileRef,
                       XMPFiles *     parent )
{
	IgnoreParam(format); IgnoreParam(filePath); IgnoreParam(parent);
	XMP_Assert ( format == kXMP_PhotoshopFile );

	IOBuffer ioBuf;

	LFA_Seek ( fileRef, 0, SEEK_SET );
	if ( ! CheckFileSpace ( fileRef, &ioBuf, 34 ) ) return false;	// 34 = header plus 2 lengths

	if ( ! CheckBytes ( ioBuf.ptr, "8BPS", 4 ) ) return false;
	ioBuf.ptr += 4;	// Move to the version.
	XMP_Uns16 version = GetUns16BE ( ioBuf.ptr );
	if ( (version != 1) && (version != 2) ) return false;

	return true;

}	// PSD_CheckFormat

// =================================================================================================
// PSD_MetaHandlerCTor
// ===================

XMPFileHandler * PSD_MetaHandlerCTor ( XMPFiles * parent )
{
	return new PSD_MetaHandler ( parent );

}	// PSD_MetaHandlerCTor

// =================================================================================================
// PSD_MetaHandler::PSD_MetaHandler
// ================================

PSD_MetaHandler::PSD_MetaHandler ( XMPFiles * _parent ) : iptcMgr(0), exifMgr(0), skipReconcile(false)
{
	this->parent = _parent;
	this->handlerFlags = kPSD_HandlerFlags;
	this->stdCharForm  = kXMP_Char8Bit;

}	// PSD_MetaHandler::PSD_MetaHandler

// =================================================================================================
// PSD_MetaHandler::~PSD_MetaHandler
// =================================

PSD_MetaHandler::~PSD_MetaHandler()
{

	if ( this->iptcMgr != 0 ) delete ( this->iptcMgr );
	if ( this->exifMgr != 0 ) delete ( this->exifMgr );

}	// PSD_MetaHandler::~PSD_MetaHandler

// =================================================================================================
// PSD_MetaHandler::CacheFileData
// ==============================
//
// Find and parse the image resource section, everything we want is in there. Don't simply capture
// the whole section, there could be lots of stuff we don't care about.

// *** This implementation simply returns when an invalid file is encountered. Should we throw instead?

void PSD_MetaHandler::CacheFileData()
{
	LFA_FileRef      fileRef    = this->parent->fileRef;
	XMP_PacketInfo & packetInfo = this->packetInfo;

	XMP_AbortProc abortProc  = this->parent->abortProc;
	void *        abortArg   = this->parent->abortArg;
	const bool    checkAbort = (abortProc != 0);

	XMP_Assert ( ! this->containsXMP );
	// Set containsXMP to true here only if the XMP image resource is found.

	if ( checkAbort && abortProc(abortArg) ) {
		XMP_Throw ( "PSD_MetaHandler::CacheFileData - User abort", kXMPErr_UserAbort );
	}

	XMP_Uns8  psdHeader[30];
	XMP_Int64 filePos;
	XMP_Uns32 ioLen, cmLen, psirLen;

	filePos = LFA_Seek ( fileRef, 0, SEEK_SET );

	ioLen = LFA_Read ( fileRef, psdHeader, 30 );
	if ( ioLen != 30 ) return;	// Throw?

	this->imageHeight = GetUns32BE ( &psdHeader[14] );
	this->imageWidth  = GetUns32BE ( &psdHeader[18] );

	cmLen = GetUns32BE ( &psdHeader[26] );

	XMP_Int64 psirOrigin = 26 + 4 + cmLen;

	filePos = LFA_Seek ( fileRef, psirOrigin, SEEK_SET );
	if ( filePos !=  psirOrigin ) return;	// Throw?

	ioLen = LFA_Read ( fileRef, psdHeader, 4 );
	if ( ioLen != 4 ) return;	// Throw?

	psirLen = GetUns32BE ( &psdHeader[0] );

	this->psirMgr.ParseFileResources ( fileRef, psirLen );

	PSIR_Manager::ImgRsrcInfo xmpInfo;
	bool found = this->psirMgr.GetImgRsrc ( kPSIR_XMP, &xmpInfo );

	if ( found ) {

		// printf ( "PSD_MetaHandler::CacheFileData - XMP packet offset %d (0x%X), size %d\n",
		//		  xmpInfo.origOffset, xmpInfo.origOffset, xmpInfo.dataLen );
		this->packetInfo.offset = xmpInfo.origOffset;
		this->packetInfo.length = xmpInfo.dataLen;
		this->packetInfo.padSize   = 0;				// Assume for now, set these properly in ProcessXMP.
		this->packetInfo.charForm  = kXMP_CharUnknown;
		this->packetInfo.writeable = true;

		this->xmpPacket.assign ( (XMP_StringPtr)xmpInfo.dataPtr, xmpInfo.dataLen );

		this->containsXMP = true;

	}

}	// PSD_MetaHandler::CacheFileData

// =================================================================================================
// PSD_MetaHandler::ProcessXMP
// ===========================
//
// Process the raw XMP and legacy metadata that was previously cached.

void PSD_MetaHandler::ProcessXMP()
{

	this->processedXMP = true;	// Make sure we only come through here once.

	// Set up everything for the legacy import, but don't do it yet. This lets us do a forced legacy
	// import if the XMP packet gets parsing errors.

	bool readOnly = ((this->parent->openFlags & kXMPFiles_OpenForUpdate) == 0);

	if ( readOnly ) {
		this->iptcMgr = new IPTC_Reader();
		this->exifMgr = new TIFF_MemoryReader();
	} else {
		this->iptcMgr = new IPTC_Writer();	// ! Parse it later.
		this->exifMgr = new TIFF_FileWriter();
	}

	PSIR_Manager & psir = this->psirMgr;	// Give the compiler help in recognizing non-aliases.
	IPTC_Manager & iptc = *this->iptcMgr;
	TIFF_Manager & exif = *this->exifMgr;

	PSIR_Manager::ImgRsrcInfo iptcInfo, exifInfo;
	bool haveIPTC = psir.GetImgRsrc ( kPSIR_IPTC, &iptcInfo );
	bool haveExif = psir.GetImgRsrc ( kPSIR_Exif, &exifInfo );
	int iptcDigestState = kDigestMatches;

	if ( haveExif ) exif.ParseMemoryStream ( exifInfo.dataPtr, exifInfo.dataLen );
	
	if ( haveIPTC ) {
		
		bool haveDigest = false;
		PSIR_Manager::ImgRsrcInfo digestInfo;
		haveDigest = psir.GetImgRsrc ( kPSIR_IPTCDigest, &digestInfo );
		if ( digestInfo.dataLen != 16 ) haveDigest = false;
		
		if ( ! haveDigest ) {
			iptcDigestState = kDigestMissing;
		} else {
			iptcDigestState = PhotoDataUtils::CheckIPTCDigest ( iptcInfo.dataPtr, iptcInfo.dataLen, digestInfo.dataPtr );
		}

	}

	XMP_OptionBits options = 0;
	if ( this->containsXMP ) options |= k2XMP_FileHadXMP;
	if ( haveIPTC ) options |= k2XMP_FileHadIPTC;
	if ( haveExif ) options |= k2XMP_FileHadExif;

	// Process the XMP packet. If it fails to parse, do a forced legacy import but still throw an
	// exception. This tells the caller that an error happened, but gives them recovered legacy
	// should they want to proceed with that.

	bool haveXMP = false;
	
	if ( ! this->xmpPacket.empty() ) {
		XMP_Assert ( this->containsXMP );
		// Common code takes care of packetInfo.charForm, .padSize, and .writeable.
		XMP_StringPtr packetStr = this->xmpPacket.c_str();
		XMP_StringLen packetLen = (XMP_StringLen)this->xmpPacket.size();
		try {
			this->xmpObj.ParseFromBuffer ( packetStr, packetLen );
			haveXMP = true;
		} catch ( ... ) {
			XMP_ClearOption ( options, k2XMP_FileHadXMP );
			if ( haveIPTC ) iptc.ParseMemoryDataSets ( iptcInfo.dataPtr, iptcInfo.dataLen );
			if ( iptcDigestState == kDigestMatches ) iptcDigestState = kDigestMissing;
			ImportPhotoData ( exif, iptc, psir, iptcDigestState, &this->xmpObj, options );
			throw;	// ! Rethrow the exception, don't absorb it.
		}
	}

	// Process the legacy metadata.

	if ( haveIPTC && (! haveXMP) && (iptcDigestState == kDigestMatches) ) iptcDigestState = kDigestMissing;
	bool parseIPTC = (iptcDigestState != kDigestMatches) || (! readOnly);
	if ( parseIPTC ) iptc.ParseMemoryDataSets ( iptcInfo.dataPtr, iptcInfo.dataLen );
	ImportPhotoData ( exif, iptc, psir, iptcDigestState, &this->xmpObj, options );
	this->containsXMP = true;	// Assume we now have something in the XMP.

}	// PSD_MetaHandler::ProcessXMP

// =================================================================================================
// PSD_MetaHandler::UpdateFile
// ===========================

void PSD_MetaHandler::UpdateFile ( bool doSafeUpdate )
{
	XMP_Assert ( ! doSafeUpdate );	// This should only be called for "unsafe" updates.

	XMP_Int64 oldPacketOffset = this->packetInfo.offset;
	XMP_Int32 oldPacketLength = this->packetInfo.length;

	if ( oldPacketOffset == kXMPFiles_UnknownOffset ) oldPacketOffset = 0;	// ! Simplify checks.
	if ( oldPacketLength == kXMPFiles_UnknownLength ) oldPacketLength = 0;

	bool fileHadXMP = ((oldPacketOffset != 0) && (oldPacketLength != 0));

	// Update the IPTC-IIM and native TIFF/Exif metadata. ExportPhotoData also trips the tiff: and
	// exif: copies from the XMP, so reserialize the now final XMP packet.
	
	ExportPhotoData ( kXMP_PhotoshopFile, &this->xmpObj, this->exifMgr, this->iptcMgr, &this->psirMgr );
	
	try {
		XMP_OptionBits options = kXMP_UseCompactFormat;
		if ( fileHadXMP ) options |= kXMP_ExactPacketLength;
		this->xmpObj.SerializeToBuffer ( &this->xmpPacket, options, oldPacketLength );
	} catch ( ... ) {
		this->xmpObj.SerializeToBuffer ( &this->xmpPacket, kXMP_UseCompactFormat );
	}

	// Decide whether to do an in-place update. This can only happen if all of the following are true:
	//	- There is an XMP packet in the file.
	//	- The are no changes to the legacy image resources. (The IPTC and EXIF are in the PSIR.)
	//	- The new XMP can fit in the old space.

	bool doInPlace = (fileHadXMP && (this->xmpPacket.size() <= (size_t)oldPacketLength));
	if ( this->psirMgr.IsLegacyChanged() ) doInPlace = false;

	if ( doInPlace ) {

		#if GatherPerformanceData
			sAPIPerf->back().extraInfo += ", PSD in-place update";
		#endif

		if ( this->xmpPacket.size() < (size_t)this->packetInfo.length ) {
			// They ought to match, cheap to be sure.
			size_t extraSpace = (size_t)this->packetInfo.length - this->xmpPacket.size();
			this->xmpPacket.append ( extraSpace, ' ' );
		}

		LFA_FileRef liveFile = this->parent->fileRef;

		XMP_Assert ( this->xmpPacket.size() == (size_t)oldPacketLength );	// ! Done by common PutXMP logic.

		// printf ( "PSD_MetaHandler::UpdateFile - XMP in-place packet offset %lld (0x%llX), size %d\n",
		//		  oldPacketOffset, oldPacketOffset, this->xmpPacket.size() );
		LFA_Seek ( liveFile, oldPacketOffset, SEEK_SET );
		LFA_Write ( liveFile, this->xmpPacket.c_str(), (XMP_StringLen)this->xmpPacket.size() );

	} else {

		#if GatherPerformanceData
			sAPIPerf->back().extraInfo += ", PSD copy update";
		#endif

		std::string origPath = this->parent->filePath;
		LFA_FileRef origRef  = this->parent->fileRef;

		std::string updatePath;
		LFA_FileRef updateRef = 0;

		CreateTempFile ( origPath, &updatePath, kCopyMacRsrc );
		updateRef = LFA_Open ( updatePath.c_str(), 'w' );

		this->parent->filePath = updatePath;
		this->parent->fileRef  = updateRef;

		try {
			XMP_Assert ( ! this->skipReconcile );
			this->skipReconcile = true;
			this->WriteFile ( origRef, origPath );
			this->skipReconcile = false;
		} catch ( ... ) {
			this->skipReconcile = false;
			LFA_Close ( updateRef );
			LFA_Delete ( updatePath.c_str() );
			this->parent->filePath = origPath;
			this->parent->fileRef  = origRef;
			throw;
		}

		LFA_Close ( origRef );
		LFA_Delete ( origPath.c_str() );

		LFA_Close ( updateRef );
		LFA_Rename ( updatePath.c_str(), origPath.c_str() );
		this->parent->filePath = origPath;
		this->parent->fileRef = 0;

	}

	this->needsUpdate = false;

}	// PSD_MetaHandler::UpdateFile

// =================================================================================================
// PSD_MetaHandler::WriteFile
// ==========================

// The metadata parts of a Photoshop file are all in the image resources. The PSIR_Manager's
// UpdateFileResources method will take care of the image resource portion of the file, updating
// those resources that have changed and preserving those that have not.

void PSD_MetaHandler::WriteFile ( LFA_FileRef sourceRef, const std::string & sourcePath )
{
	LFA_FileRef destRef = this->parent->fileRef;

	XMP_AbortProc abortProc  = this->parent->abortProc;
	void *        abortArg   = this->parent->abortArg;
	const bool    checkAbort = (abortProc != 0);

	XMP_Uns64 sourceLen = LFA_Measure ( sourceRef );
	if ( sourceLen == 0 ) return;	// Tolerate empty files.

	// Reconcile the legacy metadata, unless this is called from UpdateFile. Reserialize the XMP to
	// get standard padding, PutXMP has probably done an in-place serialize. Set the XMP image resource.

	if ( ! skipReconcile ) {
		// Update the IPTC-IIM and native TIFF/Exif metadata, and reserialize the now final XMP packet.
		ExportPhotoData ( kXMP_JPEGFile, &this->xmpObj, this->exifMgr, this->iptcMgr, &this->psirMgr );
		this->xmpObj.SerializeToBuffer ( &this->xmpPacket, kXMP_UseCompactFormat );
	}

	this->xmpObj.SerializeToBuffer ( &this->xmpPacket, kXMP_UseCompactFormat );
	this->packetInfo.offset = kXMPFiles_UnknownOffset;
	this->packetInfo.length = (XMP_StringLen)this->xmpPacket.size();
	FillPacketInfo ( this->xmpPacket, &this->packetInfo );

	this->psirMgr.SetImgRsrc ( kPSIR_XMP, this->xmpPacket.c_str(), (XMP_StringLen)this->xmpPacket.size() );

	// Copy the file header and color mode section, then write the updated image resource section,
	// and copy the tail of the source file (layer and mask section to EOF).

	LFA_Seek ( sourceRef, 0, SEEK_SET );
	LFA_Truncate (destRef, 0 );

	LFA_Copy ( sourceRef, destRef, 26 );	// Copy the file header.

	XMP_Uns32 cmLen;
	LFA_Read ( sourceRef, &cmLen, 4 );
	LFA_Write ( destRef, &cmLen, 4 );	// Copy the color mode section length.
	cmLen = GetUns32BE ( &cmLen );
	LFA_Copy ( sourceRef, destRef, cmLen );	// Copy the color mode section contents.

	XMP_Uns32 irLen;
	LFA_Read ( sourceRef, &irLen, 4 );	// Get the source image resource section length.
	irLen = GetUns32BE ( &irLen );

	this->psirMgr.UpdateFileResources ( sourceRef, destRef, 0, abortProc, abortArg );

	XMP_Uns64 tailOffset = 26 + 4 + cmLen + 4 + irLen;
	XMP_Uns64 tailLength = sourceLen - tailOffset;

	LFA_Seek ( sourceRef, tailOffset, SEEK_SET );
	LFA_Seek ( destRef, 0, SEEK_END );
	LFA_Copy ( sourceRef, destRef, tailLength );	// Copy the tail of the file.

	this->needsUpdate = false;

}	// PSD_MetaHandler::WriteFile

// =================================================================================================
