// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "XMP_Environment.h"	// ! This must be the first include.
#include "MP3_Handler.hpp"

// =================================================================================================
/// \file MP3_Handler.cpp
/// \brief MP3 handler class.
// =================================================================================================

// =================================================================================================
// Helper structs and private routines
// ====================
struct ReconProps {
	const char* frameID;
	const char* ns;
	const char* prop;
};

const static XMP_Uns32 XMP_FRAME_ID = 0x50524956;

const static ReconProps reconProps[] = {
	{ "TPE1", kXMP_NS_DM,	"artist" },
	{ "TALB", kXMP_NS_DM,	"album"  },
	{ "TRCK", kXMP_NS_DM,	"trackNumber"  },
	// exceptions that need attention:											
	{ "TCON", kXMP_NS_DM,	"genre"  }, // genres might be numeric
	{ "TIT2", kXMP_NS_DC,	"title"  }, // ["x-default"] language alternatives
	{ "COMM", kXMP_NS_DM,	"logComment"  }, // two distinct strings, language alternative
	
	{ "TYER", kXMP_NS_XMP,	"CreateDate"  }, // Year (YYYY) Deprecated since 2.4
	{ "TDAT", kXMP_NS_XMP,	"CreateDate"  }, // Date (DDMM) Deprecated since 2.4
	{ "TIME", kXMP_NS_XMP,	"CreateDate"  }, // Time (HHMM) Deprecated since 2.4
	{ "TDRC", kXMP_NS_XMP,	"CreateDate"  }, // assemble date/time v2.4

	// new reconciliations introduced in Version 5
	{ "TCMP", kXMP_NS_DM,	"partOfCompilation"  },	// presence/absence of TCMP frame dedides
	{ "USLT", kXMP_NS_DM,	"lyrics"  },
	{ "TCOM", kXMP_NS_DM,	"composer"  },
	{ "TPOS", kXMP_NS_DM,	"discNumber"  },		// * a text field! might contain "/<total>"
	{ "TCOP", kXMP_NS_DC,	"rights"  },  // ["x-default"] language alternatives
	{ "TPE4", kXMP_NS_DM,	"engineer"  },
	{ "WCOP", kXMP_NS_XMP_Rights ,	"WebStatement"  },

	{ 0, 0, 0 }	// must be last as a sentinel
};

// =================================================================================================
// MP3_MetaHandlerCTor
// ====================
XMPFileHandler * MP3_MetaHandlerCTor ( XMPFiles * parent )
{
	return new MP3_MetaHandler ( parent );
}

// =================================================================================================
// MP3_CheckFormat
// ===============
// For MP3 we check parts .... See the MP3 spec for offset info.
bool MP3_CheckFormat ( XMP_FileFormat format,
					  XMP_StringPtr  filePath,
					  LFA_FileRef    file,
					  XMPFiles *     parent )
{
	IgnoreParam(filePath); IgnoreParam(parent);	//supress warnings
	XMP_Assert ( format == kXMP_MP3File );		//standard assert
	LFA_Rewind( file );

	XMP_Uns8 header[3];
	LFA_Read( file, header, 3, true );
	if ( !CheckBytes( &header[0], "ID3", 3 ))
		return (parent->format == kXMP_MP3File);	// No ID3 signature -> depend on first call hint.	

	XMP_Uns8 major = LFA_ReadUns8( file );
	XMP_Uns8 minor = LFA_ReadUns8( file );

	if ( (major<3 || major>4) || (minor == 0xFF) )
		return false;	// only support IDv2.3 and ID3v2.4, minor must not be 0xFF

	XMP_Uns8 flags = LFA_ReadUns8( file );

	//TODO
	if ( flags & 0x10 ) 
		XMP_Throw("no support for MP3 with footer",kXMPErr_Unimplemented); //no support for MP3 with footer
	if ( flags & 0x80 )
		return false; //no support for unsynchronized MP3 (as before, also see [1219125])
	if ( flags & 0x0F ) 
		XMP_Throw("illegal header lower bits",kXMPErr_Unimplemented);

	XMP_Uns32 size = LFA_ReadUns32_BE( file );
	if ( size & 0x80808080 ) return false; //if any bit survives -> not a valid synchsafe 32 bit integer

	return true;
}	// MP3_CheckFormat


// =================================================================================================
// MP3_MetaHandler::MP3_MetaHandler
// ================================

MP3_MetaHandler::MP3_MetaHandler ( XMPFiles * _parent )
{
	this->parent = _parent;
	this->handlerFlags = kMP3_HandlerFlags;
	this->stdCharForm  = kXMP_Char8Bit;
}

// =================================================================================================
// MP3_MetaHandler::~MP3_MetaHandler
// =================================

MP3_MetaHandler::~MP3_MetaHandler()
{
	// free frames
	ID3v2Frame* curFrame;
	while( !this->framesVector.empty() )
	{
		curFrame = this->framesVector.back();
		delete curFrame;
		framesVector.pop_back();
	}
}

// =================================================================================================
// MP3_MetaHandler::CacheFileData
// ==============================
void MP3_MetaHandler::CacheFileData()
{
	//*** abort procedures
	this->containsXMP = false;		//assume no XMP for now

	LFA_FileRef file = this->parent->fileRef;
	XMP_PacketInfo &packetInfo = this->packetInfo;

	LFA_Rewind(file);

	hasID3Tag = id3Header.read( file );
	majorVersion = id3Header.fields[ID3Header::o_version_major];
	minorVersion = id3Header.fields[ID3Header::o_version_minor];
	hasExtHeader = (0 != ( 0x40 & id3Header.fields[ID3Header::o_flags])); //'naturally' false if no ID3Tag
	hasFooter = ( 0 != ( 0x10 & id3Header.fields[ID3Header::o_flags])); //'naturally' false if no ID3Tag

	// stored size is w/o initial header (thus adding 10)
	// + but extended header (if existing)
	// + padding + frames after unsynchronisation (?)
	// (if no ID3 tag existing, constructed default correctly sets size to 10.)
	oldTagSize = 10 + synchToInt32(GetUns32BE( &id3Header.fields[ID3Header::o_size] ));

	if (hasExtHeader)
	{
		extHeaderSize = synchToInt32( LFA_ReadInt32_BE( file));
		XMP_Uns8 extHeaderNumFlagBytes = LFA_ReadUns8( file );

		// v2.3 doesn't include the size, while v2.4 does
		if ( majorVersion < 4 ) extHeaderSize += 4; 
		XMP_Validate( extHeaderSize >= 6, "extHeader size too small", kXMPErr_BadFileFormat );

		bool ok;
		LFA_Seek(file, extHeaderSize - 6, SEEK_CUR , &ok);
		XMP_Assert(ok);
	}
	else
	{
		extHeaderSize = 0; // := there is no such header.
	}

	this->framesVector.clear(); //mac precaution
	ID3v2Frame* curFrame = 0; // reusable

	////////////////////////////////////////////////////
	// read frames
	while ( LFA_Tell(file) < oldTagSize )
	{
		curFrame = new ID3v2Frame();

		try {
			XMP_Int64 frameSize = curFrame->read( file, majorVersion );
			if (frameSize == 0) // no more frames coming => proceed to padding
			{
				delete curFrame; // ..since not becoming part of vector for latter delete.
				break;			 // not a throw. There's nothing wrong with padding.
			}
			this->containsXMP = true;
		} catch( XMP_Error e)
		{
			delete curFrame;
			XMP_Throw( e.GetErrMsg(), e.GetID()); // rethrow
		}

		// these are both pointer assignments, no (copy) construction
		// (MemLeak Note: for all things pushed, memory cleanup is taken care of in destructor.)
		this->framesVector.push_back( curFrame );

		//remember XMP-Frame, if it occurs:
		if ( CheckBytes( &curFrame->fields[ID3v2Frame::o_id], "PRIV", 4 ))
			if( curFrame->contentSize > 8 ) // to avoid adress violation on very small non-XMP PRIV frames
				if( CheckBytes( &curFrame->content[0], "XMP\0", 4 ))
				{
					// be sure that this is the first packet (all else would be illegal format)
					XMP_Validate( framesMap[ XMP_FRAME_ID] == 0, "two XMP packets in one file", kXMPErr_BadFileFormat );
					//add this to map, needed on reconciliation
					framesMap[ XMP_FRAME_ID ] = curFrame;

					this->packetInfo.length = curFrame->contentSize - 4; // content minus "XMP\0"
					this->packetInfo.offset = ( LFA_Tell(file) - this->packetInfo.length );

					this->xmpPacket.erase(); //safety
					this->xmpPacket.assign( &curFrame->content[4], curFrame->contentSize - 4 );
					this->containsXMP = true; // do this last, after all possible failure
				}

				// No space for another frame? => assume into ID3v2.4 padding.
				if ( LFA_Tell(file) + 10 >= oldTagSize )
					break;
	}

	////////////////////////////////////////////////////
	// padding
	oldPadding = oldTagSize - LFA_Tell( file );
	oldFramesSize = oldTagSize - 10 - oldPadding;

	XMP_Validate( oldPadding >= 0, "illegal oldTagSize or padding value", kXMPErr_BadFileFormat );

	for ( XMP_Int64 i = oldPadding; i > 0;)
	{
		if ( i >= 8 ) // worthwhile optimization
		{
			if ( LFA_ReadInt64_BE(file) != 0 )
				XMP_Throw ( "padding not nulled out.", kXMPErr_BadFileFormat );
			i -= 8;
			continue;
		}
		if ( LFA_ReadUns8(file) != 0)
			XMP_Throw ( "padding(2) not nulled out.", kXMPErr_BadFileFormat );
		i--;
	}

	//// read ID3v1 tag
	if ( ! this->containsXMP ) // all else has priority
	{
		this->containsXMP = id3v1Tag.read( file, &this->xmpObj );
	}

}	// MP3_MetaHandler::CacheFileData


// =================================================================================================
// MP3_MetaHandler::ProcessXMP
// ===========================
//
// Process the raw XMP and legacy metadata that was previously cached.
void MP3_MetaHandler::ProcessXMP()
{
	// Process the XMP packet.
	if ( ! this->xmpPacket.empty() ) {	
		XMP_Assert ( this->containsXMP );
		XMP_StringPtr packetStr = this->xmpPacket.c_str();
		XMP_StringLen packetLen = (XMP_StringLen) this->xmpPacket.size();
		this->xmpObj.ParseFromBuffer ( packetStr, packetLen );
		this->processedXMP = true;
	}

	///////////////////////////////////////////////////////////////////
	// assumptions on presence-absence "flag tags"
	//   ( unless no xmp whatsoever present )
	if ( ! this->xmpPacket.empty() ) {	
		this->xmpObj.SetProperty( kXMP_NS_DM, "partOfCompilation", "false" );
	}

	////////////////////////////////////////////////////////////////////
	// import of legacy properties
	ID3v2Frame* curFrame;
	XMP_Bool hasTDRC = false;
	XMP_DateTime newDateTime;

	if (this->hasID3Tag) // otherwise pretty pointless...		
		for (int r=0; reconProps[r].frameID != 0; r++)
		{
			//get the frame ID to look for
			XMP_Uns32 frameID = GetUns32BE( reconProps[r].frameID );

			// deal with each such frame in the frameVector
			// (since there might be several, some of them not applicable, i.e. COMM)
			for ( vector<ID3_Support::ID3v2Frame*>::iterator it = framesVector.begin(); it!=framesVector.end(); ++it)
			{
				curFrame = *it;
				if (frameID != curFrame->id) // nothing applicable. Next!
					continue;

				// go deal with it!
				// get the property
				std::string utf8string;
				bool result = curFrame->getFrameValue(majorVersion, frameID, &utf8string);

				if (! result)
					continue; //ignore but preserve this frame (i.e. not applicable COMM frame)

				//////////////////////////////////////////////////////////////////////////////////
				// if we come as far as here, it's proven that there's a relevant XMP property
				this->containsXMP = true;

				ID3_Support::ID3v2Frame* t = framesMap[ frameID ];
				if ( t != 0 ) // an (earlier, relevant) frame?
					t->active = false;

				// add this to map (needed on reconciliation)
				// note: above code reaches, that COMM/USLT frames
				// only then reach this map, if they are 'eng'(lish)
				// multiple occurences indeed leads to last one survives
				// ( in this map, all survive in the file )
				framesMap[ frameID ] = curFrame;

				// now write away as needed;
				// merely based on existence, relevant even if empty:
				if ( frameID == 0x54434D50) // TCMP if exists: part of compilation
				{
					this->xmpObj.SetProperty( kXMP_NS_DM, "partOfCompilation", "true" );
				} else if ( ! utf8string.empty() ) 
					switch( frameID )
					{
					case 0x54495432: // TIT2 -> title["x-default"]
					case 0x54434F50: // TCOP -> rights["x-default"]
						this->xmpObj.SetLocalizedText( reconProps[r].ns , reconProps[r].prop,"" , "x-default" , utf8string );				
						break;
					case 0x54434F4E: // TCON -> genre ( might be numeric string. prior to 2.3 a one-byte numeric value? )
						{					
							XMP_Int32 pos = 0; // going through input string
							if ( utf8string[pos] == '(' ) { // number in brackets?
								pos++;
								XMP_Uns8 iGenre = (XMP_Uns8) atoi( &utf8string.c_str()[1] );
								if ( (iGenre > 0) && (iGenre < 127) ) {
									utf8string.assign( Genres[iGenre] );
								} else {
									utf8string.assign( Genres[12] ); // "Other"
								}
							} else {
								// Text, let's "try" to find it anyway (for best upper/lower casing)
								int i;
								const char* genreCString = utf8string.c_str();
								for ( i=0; i < 127; ++i ) {
									if ( 
										   (strlen( genreCString ) == strlen(Genres[i])) &&  //fixing buggy stricmp behaviour on PPC
											(stricmp( genreCString, Genres[i] ) == 0 )) {
										utf8string.assign( Genres[i] ); // found, let's use the one in the list
										break;
									}
								}
								// otherwise (if for-loop runs through): leave as is
							}
							// write out property (derived or direct, but certainly non-numeric)
							this->xmpObj.SetProperty( reconProps[r].ns, reconProps[r].prop, utf8string );
						}
						break;
					case 0x54594552: // TYER -> xmp:CreateDate.year
						{
							try {	// Don't let wrong dates in id3 stop import.
								if ( !hasTDRC )
								{
									newDateTime.year = SXMPUtils::ConvertToInt( utf8string );
									newDateTime.hasDate = true;
								}
							} catch ( ... ) {
									// Do nothing, let other imports proceed.								
							}
							break;
						}
					case 0x54444154: //TDAT	-> xmp:CreateDate.month and day
						{
							try {	// Don't let wrong dates in id3 stop import.								
								// only if no TDRC has been found before
								//&& must have the format DDMM								
								if ( !hasTDRC && utf8string.length() == 4 )
								{
									newDateTime.day = SXMPUtils::ConvertToInt(utf8string.substr(0,2));
									newDateTime.month = SXMPUtils::ConvertToInt( utf8string.substr(2,2));
									newDateTime.hasDate = true;
								}
							} catch ( ... ) {								
								// Do nothing, let other imports proceed.								
							}
							break;
						}
					case 0x54494D45: //TIME	-> xmp:CreateDate.hours and minutes
						{
							try {	// Don't let wrong dates in id3 stop import.
								// only if no TDRC has been found before
								// && must have the format HHMM
								if ( !hasTDRC && utf8string.length() == 4 )
								{
									newDateTime.hour = SXMPUtils::ConvertToInt(utf8string.substr(0,2));
									newDateTime.minute = SXMPUtils::ConvertToInt( utf8string.substr(2,2));
									newDateTime.hasTime = true;
								}
							} catch ( ... ) {
								// Do nothing, let other imports proceed.								
							}
							break;
						}					
					case 0x54445243: // TDRC -> xmp:CreateDate //id3 v2.4
						{
							try {	// Don't let wrong dates in id3 stop import.								
								hasTDRC = true;
								// This always wins over TYER, TDAT and TIME
								SXMPUtils::ConvertToDate( utf8string, &newDateTime );					
							} catch ( ... ) {
								// Do nothing, let other imports proceed.								
							}
							break;
						}						
					default:
						// NB: COMM/USLT need no special fork regarding language alternatives/multiple occurence.
						//		relevant code forks are in ID3_Support::getFrameValue()
						this->xmpObj.SetProperty( reconProps[r].ns, reconProps[r].prop, utf8string );
						break;
					}//switch
			} //for iterator
		}//for reconProps

	// import DateTime
	XMP_DateTime oldDateTime;		
	xmpObj.GetProperty_Date( kXMP_NS_XMP, "CreateDate", &oldDateTime, 0 );


	
	// NOTE: no further validation nessesary the function "SetProperty_Date" will care about validating date and time
	// any exception will be caught and block import
	try {
		// invalid year will be catched and blocks import
		XMP_Validate( (newDateTime.year > 0 && newDateTime.year < 9999), "", kXMPErr_BadParam );
	
		// 2. if year has changed --> everything (date/time) has changed --> overwrite old DateTime with new DateTime
		if ( ( newDateTime.year != oldDateTime.year ) || // year has changed?
			// or has same year but new day/month (checking existance month indicates both (day and month) in our case)
			(( newDateTime.month != 0 ) && ( newDateTime.day != oldDateTime.day ||  newDateTime.month != oldDateTime.month )) ||
			// or has same year and same date but different time
			( newDateTime.hasTime && ( newDateTime.hour != oldDateTime.minute ||  newDateTime.hour != oldDateTime.minute )) )
		{
			this->xmpObj.SetProperty_Date( kXMP_NS_XMP, "CreateDate", newDateTime );
		} // ..else: keep old dateTime to don't loose data

	} catch ( ... ) {
		// Dont import invalid dates from ID3								
	}
	

	// very important to avoid multiple runs! (in which case I'd need to clean certain
	// fields (i.e. regarding ->active setting)
	this->processedXMP = true;

}	// MP3_MetaHandler::ProcessXMP


// =================================================================================================
// MP3_MetaHandler::UpdateFile
// ===========================
void MP3_MetaHandler::UpdateFile ( bool doSafeUpdate )
{
	if ( doSafeUpdate )
		XMP_Throw ( "UCF_MetaHandler::UpdateFile: Safe update not supported", kXMPErr_Unavailable );

	LFA_FileRef file ( this->parent->fileRef );

	// leave 2.3 resp. 2.4 header, since we want to let alone 
	// and don't know enough about the encoding of unrelated frames...
	XMP_Assert( this->containsXMP );

	tagIsDirty = false;
	mustShift = false;

	// write out native properties:
	// * update existing ones
	// * create new frames as needed
	// * delete frames if property is gone!
	// see what there is to do for us:

	// RECON LOOP START
	for (int r=0; reconProps[r].frameID != 0; r++)
	{
		std::string value;
		bool needDescriptor = false;
		bool need16LE = true;
		bool needEncodingByte = true;

		XMP_Uns32 frameID = GetUns32BE( reconProps[r].frameID ); // the would-be frame
		ID3v2Frame* frame = framesMap[ frameID ];	// the actual frame (if already existing)

		// get XMP property
		//	* honour specific exceptions
		//  * leave value empty() if it doesn't exist ==> frame must be delete/not created
		switch( frameID )
		{
		case 0x54434D50: // TCMP if exists: part of compilation
			need16LE = false;
			if ( xmpObj.GetProperty( kXMP_NS_DM, "partOfCompilation", &value, 0 )
				&& ( 0 == stricmp( value.c_str(), "true" ) ))
				value = "1"; // set a TCMP frame of value 1
			else
				value.erase(); // delete/prevent creation of frame
			break;

		case 0x54495432: // TIT2 -> title["x-default"]
		case 0x54434F50: // TCOP -> rights["x-default"]
			if (! xmpObj.GetLocalizedText( reconProps[r].ns, reconProps[r].prop, "", "x-default", 0, &value, 0 )) //jaja, side effect.
				value.erase(); // if not, erase string.
			break;
		case 0x54434F4E: // TCON -> genre
			{
				if (! xmpObj.GetProperty( reconProps[r].ns, reconProps[r].prop, &value, 0 ))
				{	// nothing found? -> Erase string. (Leads to Unset below)
					value.erase();
					break;
				}
				// genre: we need to get the number back, if possible
				XMP_Int16 iFound = -1; // flag as "not found"
				for ( int i=0; i < 127; ++i ) {
					if ( (value.size() == strlen(Genres[i])) 
					  && (stricmp( value.c_str(), Genres[i] ) == 0) ) //fixing stricmp buggy on PPC
						{
							iFound = i; // Found
							break;
						}
				}
				if ( iFound == -1 ) // not found known numeric genre?
					break; // stick with the literal value (also for v2.3, since this is common practice!)

				need16LE = false; // no unicode need for (##)
				char strGenre[64];
				snprintf ( strGenre, sizeof(strGenre), "(%d)", iFound );	// AUDIT: Using sizeof(strGenre) is safe.
				value.assign(strGenre);
			}
			break;
		case 0x434F4D4D: // COMM
		case 0x55534C54: // USLT, both need descriptor.
			needDescriptor = true;
			if (! xmpObj.GetProperty( reconProps[r].ns, reconProps[r].prop, &value, 0 ))
				value.erase();
			break;
		case 0x54594552: //TYER
		case 0x54444154: //TDAT	
		case 0x54494D45: //TIME			
			{
				if ( majorVersion <= 3 ) // TYER, TIME and TDAT depricated since v. 2.4 -> else use TDRC
				{
					XMP_DateTime dateTime;	
					if (! xmpObj.GetProperty_Date( reconProps[r].ns, reconProps[r].prop, &dateTime, 0 ))
					{	// nothing found? -> Erase string. (Leads to Unset below)
						value.erase();
						break;
					}

					// TYER
					if ( frameID == 0x54594552 )
					{
						XMP_Validate( dateTime.year <= 9999 && dateTime.year > 0 , "Year is out of range", kXMPErr_BadParam);
						// get only Year!
						SXMPUtils::ConvertFromInt( dateTime.year, "", &value );
						break;
					} 
					// TDAT
					else if ( frameID == 0x54444154 && dateTime.hasDate ) // date validation made by "GetProperty_Date"
					{						
						std::string day, month;
						SXMPUtils::ConvertFromInt( dateTime.day, "", &day );
						SXMPUtils::ConvertFromInt( dateTime.month, "", &month );
						if ( dateTime.day < 10 )
							value = "0";
						value += day;
						if ( dateTime.month < 10 )
							value += "0";
						value += month;
						break;
					} 
					// TIME
					else if ( frameID == 0x54494D45 && dateTime.hasTime ) // time validation made by "GetProperty_Date" )
					{
						std::string hour, minute;
						SXMPUtils::ConvertFromInt( dateTime.hour, "", &hour );
						SXMPUtils::ConvertFromInt( dateTime.minute, "", &minute );
						if ( dateTime.hour < 10 )
							value = "0";
						value += hour;
						if ( dateTime.minute < 10 )
							value += "0";
						value += minute;
						break;
					} 
					else
					{
						value.erase();
						break;
					}
				}
				else // v.2.4 --> delete TYER,TIME or TDAT & write into TDRC
				{
					value.erase();
					break;
				}
			}
		case 0x54445243: //TDRC (only v2.4)
			{
				// only export for id3 > v2.4
				if ( majorVersion > 3 ) // (only v2.4)
				{
					if (! xmpObj.GetProperty( reconProps[r].ns, reconProps[r].prop, &value, 0 ))
						value.erase();				
				}
				break;
			}
		break;
		case 0x57434F50: //WCOP
			needEncodingByte = false;
			need16LE = false;
			if (! xmpObj.GetProperty( reconProps[r].ns, reconProps[r].prop, &value, 0 ))
				value.erase(); // if not, erase string
		break;
		case 0x5452434B: // TRCK
		case 0x54504F53: // TPOS
			need16LE = false;
			// no break, go on:
		default:
			if (! xmpObj.GetProperty( reconProps[r].ns, reconProps[r].prop, &value, 0 ))
				value.erase(); // if not, erase string
			break;
		}

		// [XMP exist] x [frame exist] => four cases:
		// 1/4) nothing before, nothing now
		if ( value.empty() && (frame==0))  
			continue; // nothing to do

		// all else means there will be rewrite work to do:
		tagIsDirty = true;

		// 2/4) value before, now gone:
		if ( value.empty() && (frame!=0))
		{
			frame->active = false; //mark for non-use
			continue;
		}
		// 3/4) no old value, create new value
		if ( frame==0)
		{	
			ID3v2Frame* newFrame=new ID3v2Frame( frameID );
			newFrame->setFrameValue( value, needDescriptor,  need16LE, false, needEncodingByte ); //always write as utf16-le incl. BOM
			framesVector.push_back( newFrame );
			framesMap[ frameID ] = newFrame;
			continue;
		}
		// 4/4) change existing value
		else // resp. change frame
		{
			frame->setFrameValue( value, needDescriptor, need16LE, false, needEncodingByte );
		}
	} 	// RECON LOOP END

	/////////////////////////////////////////////////////////////////////////////////
	// (Re)Build XMP frame:
	ID3v2Frame* frame = framesMap[ XMP_FRAME_ID ];
	if ( frame == 0)
	{	
		ID3v2Frame* newFrame=new ID3v2Frame( XMP_FRAME_ID );
		newFrame->setFrameValue( this->xmpPacket, false, false, true );
		framesVector.push_back( newFrame );
		framesMap[ XMP_FRAME_ID ] = newFrame;
	} else
		frame->setFrameValue( this->xmpPacket, false, false, true );

	////////////////////////////////////////////////////////////////////////////////
	// Decision making
	newFramesSize = 0;
	for ( XMP_Uns32 i=0; i < framesVector.size(); i++)
	{
		if (framesVector[i]->active)
			newFramesSize += (10 + framesVector[i]->contentSize );
	}

	mustShift = ( newFramesSize > (oldTagSize - 10)) ||
	//optimization: If more than 8K can be saved by rewriting the MP3, go do it:
				((newFramesSize + 8*1024) < oldTagSize );

	if (!mustShift)	// fill what we got
		newTagSize = oldTagSize;
	else // if need to shift anyway, get some nice 2K padding
		newTagSize = newFramesSize + 2048 + 10;
	newPadding = newTagSize -10 - newFramesSize;

	// shifting needed? -> shift
	if ( mustShift )
	{
		XMP_Int64 filesize = LFA_Measure( file );
		if ( this->hasID3Tag )
			LFA_Move( file, oldTagSize, file, newTagSize , filesize - oldTagSize ); //fix [2338569]
		else
			LFA_Move( file, 0, file, newTagSize, filesize ); // move entire file up.
	}

	// correct size stuff, write out header
	LFA_Rewind( file );
	id3Header.write( file, newTagSize);	

	// write out tags
	for ( XMP_Uns32 i=0; i < framesVector.size(); i++)
	{
		if ( framesVector[i]->active)
			framesVector[i]->write(file, majorVersion);
	}

	// write out padding:
	for ( XMP_Int64 i = newPadding; i > 0;)
	{
		const XMP_Uns64 zero = 0;
		if ( i >= 8 ) // worthwhile optimization
		{
			LFA_Write( file, &zero, 8 );
			i -= 8;
			continue;
		}
		LFA_Write( file, &zero, 1 );
		i--;
	}

	// check end of file for ID3v1 tag
	XMP_Int64 possibleTruncationPoint = LFA_Seek( file, -128, SEEK_END);
	bool alreadyHasID3v1 = (LFA_ReadInt32_BE( file ) & 0xFFFFFF00) == 0x54414700; // "TAG"
	if ( ! alreadyHasID3v1 ) // extend file
		LFA_Extend( file, LFA_Measure( file ) + 128 );
	id3v1Tag.write( file, &this->xmpObj );

	this->needsUpdate = false; //do last for safety reasons
}	// MP3_MetaHandler::UpdateFile

// =================================================================================================
// MP3_MetaHandler::WriteFile
// ==========================

void MP3_MetaHandler::WriteFile ( LFA_FileRef         sourceRef,
								 const std::string & sourcePath )
{
	IgnoreParam(sourceRef); IgnoreParam(sourcePath);
	XMP_Throw ( "MP3_MetaHandler::WriteFile: Not supported", kXMPErr_Unimplemented );
}	// MP3_MetaHandler::WriteFile
