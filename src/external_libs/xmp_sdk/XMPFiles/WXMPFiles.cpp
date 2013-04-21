// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2004 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "XMP_Environment.h"
#include "XMP_Const.h"

#include "client-glue/WXMPFiles.hpp"

#include "XMPFiles_Impl.hpp"
#include "XMPFiles.hpp"

#if XMP_WinBuild
	#if XMP_DebugBuild
		#pragma warning ( disable : 4297 ) // function assumed not to throw an exception but does
	#endif
#endif

#if __cplusplus
extern "C" {
#endif

// =================================================================================================

static WXMP_Result voidResult;	// Used for functions that  don't use the normal result mechanism.

// =================================================================================================

void WXMPFiles_GetVersionInfo_1 ( XMP_VersionInfo * versionInfo )
{
	WXMP_Result * wResult = &voidResult;	// ! Needed to "fool" the EnterWrapper macro.
	XMP_ENTER_NoLock ( "WXMPFiles_GetVersionInfo_1" )
	
		XMPFiles::GetVersionInfo ( versionInfo );
	
	XMP_EXIT_NoThrow
}
    
// -------------------------------------------------------------------------------------------------
    
void WXMPFiles_Initialize_1 ( XMP_OptionBits options,
							  WXMP_Result *  wResult )
{
	XMP_ENTER_NoLock ( "WXMPFiles_Initialize_1" )
	
		wResult->int32Result = XMPFiles::Initialize ( options );
	
	XMP_EXIT
}

// -------------------------------------------------------------------------------------------------
    
void WXMPFiles_Terminate_1()
{
	WXMP_Result * wResult = &voidResult;	// ! Needed to "fool" the EnterWrapper macro.
	XMP_ENTER_NoLock ( "WXMPFiles_Terminate_1" )
	
		XMPFiles::Terminate();
	
	XMP_EXIT_NoThrow
}

// =================================================================================================
    
void WXMPFiles_CTor_1 ( WXMP_Result * wResult )
{
	XMP_ENTER_Static ( "WXMPFiles_CTor_1" )	// No lib object yet, use the static entry.
	
		XMPFiles * newObj = new XMPFiles();
		++newObj->clientRefs;
		XMP_Assert ( newObj->clientRefs == 1 );
		wResult->ptrResult = newObj;
	
	XMP_EXIT
}

// -------------------------------------------------------------------------------------------------

void WXMPFiles_IncrementRefCount_1 ( XMPFilesRef xmpObjRef )
{
	WXMP_Result * wResult = &voidResult;	// ! Needed to "fool" the EnterWrapper macro.
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_IncrementRefCount_1" )
	
		++thiz->clientRefs;
		XMP_Assert ( thiz->clientRefs > 0 );
	
	XMP_EXIT_NoThrow
}

// -------------------------------------------------------------------------------------------------

void WXMPFiles_DecrementRefCount_1 ( XMPFilesRef xmpObjRef )
{
	WXMP_Result * wResult = &voidResult;	// ! Needed to "fool" the EnterWrapper macro.
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_DecrementRefCount_1" )
	
		XMP_Assert ( thiz->clientRefs > 0 );
		--thiz->clientRefs;
		if ( thiz->clientRefs <= 0 ) {
			objLock.Release();
			delete ( thiz );
		}
	
	XMP_EXIT_NoThrow
}

// =================================================================================================

void WXMPFiles_GetFormatInfo_1 ( XMP_FileFormat   format,
                                 XMP_OptionBits * flags,
                                 WXMP_Result *    wResult )
{
	XMP_ENTER_Static ( "WXMPFiles_GetFormatInfo_1" )
	
		wResult->int32Result = XMPFiles::GetFormatInfo ( format, flags );
	
	XMP_EXIT
}

// =================================================================================================

void WXMPFiles_CheckFileFormat_1 ( XMP_StringPtr filePath,
								   WXMP_Result * wResult )
{
	XMP_ENTER_Static ( "WXMPFiles_CheckFileFormat_1" )
	
		wResult->int32Result = XMPFiles::CheckFileFormat ( filePath );
	
	XMP_EXIT
}

// =================================================================================================

void WXMPFiles_CheckPackageFormat_1 ( XMP_StringPtr folderPath,
                       				  WXMP_Result * wResult )
{
	XMP_ENTER_Static ( "WXMPFiles_CheckPackageFormat_1" )
	
		wResult->int32Result = XMPFiles::CheckPackageFormat ( folderPath );
	
	XMP_EXIT
}

// =================================================================================================

void WXMPFiles_OpenFile_1 ( XMPFilesRef    xmpObjRef,
                            XMP_StringPtr  filePath,
			                XMP_FileFormat format,
			                XMP_OptionBits openFlags,
                            WXMP_Result *  wResult )
{
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_OpenFile_1" )
		StartPerfCheck ( kAPIPerf_OpenFile, filePath );
	
		bool ok = thiz->OpenFile ( filePath, format, openFlags );
		wResult->int32Result = ok;
	
		EndPerfCheck ( kAPIPerf_OpenFile );
	XMP_EXIT
}
    
// -------------------------------------------------------------------------------------------------

void WXMPFiles_CloseFile_1 ( XMPFilesRef    xmpObjRef,
                             XMP_OptionBits closeFlags,
                             WXMP_Result *  wResult )
{
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_CloseFile_1" )
		StartPerfCheck ( kAPIPerf_CloseFile, "" );
	
		thiz->CloseFile ( closeFlags );
	
		EndPerfCheck ( kAPIPerf_CloseFile );
	XMP_EXIT
}
	
// -------------------------------------------------------------------------------------------------

void WXMPFiles_GetFileInfo_1 ( XMPFilesRef      xmpObjRef,
                               void *           clientPath,
			                   XMP_OptionBits * openFlags,
			                   XMP_FileFormat * format,
			                   XMP_OptionBits * handlerFlags,
			                   SetClientStringProc SetClientString,
                               WXMP_Result *    wResult )
{
	XMP_ENTER_ObjRead ( XMPFiles, "WXMPFiles_GetFileInfo_1" )
	
		XMP_StringPtr pathStr;
		XMP_StringLen pathLen;

		bool isOpen = thiz.GetFileInfo ( &pathStr, &pathLen, openFlags, format, handlerFlags );
		if ( isOpen && (clientPath != 0) ) (*SetClientString) ( clientPath, pathStr, pathLen );
		wResult->int32Result = isOpen;
	
	XMP_EXIT
}
    
// -------------------------------------------------------------------------------------------------

void WXMPFiles_SetAbortProc_1 ( XMPFilesRef   xmpObjRef,
                         	    XMP_AbortProc abortProc,
							    void *        abortArg,
							    WXMP_Result * wResult )
{
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_SetAbortProc_1" )
	
		thiz->SetAbortProc ( abortProc, abortArg );
	
	XMP_EXIT
}
    
// -------------------------------------------------------------------------------------------------

void WXMPFiles_GetXMP_1 ( XMPFilesRef      xmpObjRef,
                          XMPMetaRef       xmpRef,
		                  void *           clientPacket,
		                  XMP_PacketInfo * packetInfo,
		                  SetClientStringProc SetClientString,
                          WXMP_Result *    wResult )
{
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_GetXMP_1" )
		StartPerfCheck ( kAPIPerf_GetXMP, "" );

		bool hasXMP = false;
		XMP_StringPtr packetStr;
		XMP_StringLen packetLen;

		if ( xmpRef == 0 ) {
			hasXMP = thiz->GetXMP ( 0, &packetStr, &packetLen, packetInfo );
		} else {
			SXMPMeta xmpObj ( xmpRef );
			hasXMP = thiz->GetXMP ( &xmpObj, &packetStr, &packetLen, packetInfo );
		}
		
		if ( hasXMP && (clientPacket != 0) ) (*SetClientString) ( clientPacket, packetStr, packetLen );
		wResult->int32Result = hasXMP;
	
		EndPerfCheck ( kAPIPerf_GetXMP );
	XMP_EXIT
}
    
// -------------------------------------------------------------------------------------------------

void WXMPFiles_PutXMP_1 ( XMPFilesRef   xmpObjRef,
                          XMPMetaRef    xmpRef,	// ! Only one of the XMP object or packet are passed.
                          XMP_StringPtr xmpPacket,
                          XMP_StringLen xmpPacketLen,
                          WXMP_Result * wResult )
{
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_PutXMP_1" )
		StartPerfCheck ( kAPIPerf_PutXMP, "" );
	
		if ( xmpRef != 0 ) {
			thiz->PutXMP ( xmpRef );
		} else {
			thiz->PutXMP ( xmpPacket, xmpPacketLen );
		}
	
		EndPerfCheck ( kAPIPerf_PutXMP );
	XMP_EXIT
}
    
// -------------------------------------------------------------------------------------------------

void WXMPFiles_CanPutXMP_1 ( XMPFilesRef   xmpObjRef,
                             XMPMetaRef    xmpRef,	// ! Only one of the XMP object or packet are passed.
                             XMP_StringPtr xmpPacket,
                             XMP_StringLen xmpPacketLen,
                             WXMP_Result * wResult )
{
	XMP_ENTER_ObjWrite ( XMPFiles, "WXMPFiles_CanPutXMP_1" )
		StartPerfCheck ( kAPIPerf_CanPutXMP, "" );
	
		if ( xmpRef != 0 ) {
			wResult->int32Result = thiz->CanPutXMP ( xmpRef );
		} else {
			wResult->int32Result = thiz->CanPutXMP ( xmpPacket, xmpPacketLen );
		}
	
		EndPerfCheck ( kAPIPerf_CanPutXMP );
	XMP_EXIT
}

// =================================================================================================

#if __cplusplus
}
#endif
