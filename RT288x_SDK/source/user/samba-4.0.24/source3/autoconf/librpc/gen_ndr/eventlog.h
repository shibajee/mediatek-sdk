/* header auto-generated by pidl */

#ifndef _PIDL_HEADER_eventlog
#define _PIDL_HEADER_eventlog

#include <stdint.h>

#include "libcli/util/ntstatus.h"

#include "librpc/gen_ndr/lsa.h"
#include "librpc/gen_ndr/security.h"
#ifndef _HEADER_eventlog
#define _HEADER_eventlog

/* bitmap eventlogReadFlags */
#define EVENTLOG_SEQUENTIAL_READ ( 0x0001 )
#define EVENTLOG_SEEK_READ ( 0x0002 )
#define EVENTLOG_FORWARDS_READ ( 0x0004 )
#define EVENTLOG_BACKWARDS_READ ( 0x0008 )

enum eventlogEventTypes
#ifndef USE_UINT_ENUMS
 {
	EVENTLOG_SUCCESS=(int)(0x0000),
	EVENTLOG_ERROR_TYPE=(int)(0x0001),
	EVENTLOG_WARNING_TYPE=(int)(0x0002),
	EVENTLOG_INFORMATION_TYPE=(int)(0x0004),
	EVENTLOG_AUDIT_SUCCESS=(int)(0x0008),
	EVENTLOG_AUDIT_FAILURE=(int)(0x0010)
}
#else
 { __donnot_use_enum_eventlogEventTypes=0x7FFFFFFF}
#define EVENTLOG_SUCCESS ( 0x0000 )
#define EVENTLOG_ERROR_TYPE ( 0x0001 )
#define EVENTLOG_WARNING_TYPE ( 0x0002 )
#define EVENTLOG_INFORMATION_TYPE ( 0x0004 )
#define EVENTLOG_AUDIT_SUCCESS ( 0x0008 )
#define EVENTLOG_AUDIT_FAILURE ( 0x0010 )
#endif
;

struct eventlog_OpenUnknown0 {
	uint16_t unknown0;
	uint16_t unknown1;
};

struct eventlog_Record_tdb {
	uint32_t size;
	const char *reserved;/* [value("eLfL"),charset(DOS)] */
	uint32_t record_number;
	time_t time_generated;
	time_t time_written;
	uint32_t event_id;
	enum eventlogEventTypes event_type;
	uint16_t num_of_strings;/* [range(0,256)] */
	uint16_t event_category;
	uint16_t reserved_flags;
	uint32_t closing_record_number;
	uint32_t stringoffset;
	uint32_t sid_length;/* [value(sid.length)] */
	uint32_t sid_offset;
	uint32_t data_length;/* [value(data.length)] */
	uint32_t data_offset;
	uint32_t source_name_len;/* [value(2*strlen_m_term(source_name))] */
	const char * source_name;/* [flag(LIBNDR_FLAG_STR_NULLTERM|LIBNDR_FLAG_ALIGN2)] */
	uint32_t computer_name_len;/* [value(2*strlen_m_term(computer_name))] */
	const char * computer_name;/* [flag(LIBNDR_FLAG_STR_NULLTERM|LIBNDR_FLAG_ALIGN2)] */
	uint32_t sid_padding;
	DATA_BLOB sid;
	uint32_t strings_len;/* [value(2*ndr_size_string_array(strings,num_of_strings,LIBNDR_FLAG_STR_NULLTERM))] */
	const char * *strings;/* [flag(LIBNDR_FLAG_STR_NULLTERM|LIBNDR_FLAG_ALIGN2)] */
	DATA_BLOB data;
	uint32_t padding;
}/* [flag(LIBNDR_FLAG_NOALIGN|LIBNDR_PRINT_ARRAY_HEX),public] */;

enum EVENTLOG_HEADER_FLAGS
#ifndef USE_UINT_ENUMS
 {
	ELF_LOGFILE_HEADER_DIRTY=(int)(0x0001),
	ELF_LOGFILE_HEADER_WRAP=(int)(0x0002),
	ELF_LOGFILE_LOGFULL_WRITTEN=(int)(0x0004),
	ELF_LOGFILE_ARCHIVE_SET=(int)(0x0008)
}
#else
 { __donnot_use_enum_EVENTLOG_HEADER_FLAGS=0x7FFFFFFF}
#define ELF_LOGFILE_HEADER_DIRTY ( 0x0001 )
#define ELF_LOGFILE_HEADER_WRAP ( 0x0002 )
#define ELF_LOGFILE_LOGFULL_WRITTEN ( 0x0004 )
#define ELF_LOGFILE_ARCHIVE_SET ( 0x0008 )
#endif
;

struct EVENTLOGHEADER {
	uint32_t HeaderSize;/* [value(0x30)] */
	const char *Signature;/* [value("LfLe"),charset(DOS)] */
	uint32_t MajorVersion;/* [value] */
	uint32_t MinorVersion;/* [value] */
	uint32_t StartOffset;
	uint32_t EndOffset;
	uint32_t CurrentRecordNumber;
	uint32_t OldestRecordNumber;
	uint32_t MaxSize;
	enum EVENTLOG_HEADER_FLAGS Flags;
	uint32_t Retention;
	uint32_t EndHeaderSize;/* [value(0x30)] */
}/* [public] */;

struct EVENTLOGRECORD {
	uint32_t Length;
	const char *Reserved;/* [charset(DOS),value("LfLe")] */
	uint32_t RecordNumber;
	time_t TimeGenerated;
	time_t TimeWritten;
	uint32_t EventID;
	enum eventlogEventTypes EventType;
	uint16_t NumStrings;
	uint16_t EventCategory;
	uint16_t ReservedFlags;
	uint32_t ClosingRecordNumber;
	uint32_t StringOffset;/* [value(56+2*(strlen_m_term(SourceName)+strlen_m_term(Computername))+UserSidLength)] */
	uint32_t UserSidLength;/* [value(ndr_size_dom_sid0(&UserSid,ndr->flags))] */
	uint32_t UserSidOffset;/* [value(56+2*(strlen_m_term(SourceName)+strlen_m_term(Computername)))] */
	uint32_t DataLength;
	uint32_t DataOffset;/* [value(56+2*(strlen_m_term(SourceName)+strlen_m_term(Computername))+UserSidLength+(2*ndr_size_string_array(Strings,NumStrings,LIBNDR_FLAG_STR_NULLTERM)))] */
	const char * SourceName;/* [flag(LIBNDR_FLAG_STR_NULLTERM|LIBNDR_FLAG_ALIGN2)] */
	const char * Computername;/* [flag(LIBNDR_FLAG_STR_NULLTERM|LIBNDR_FLAG_ALIGN2)] */
	struct dom_sid0 UserSid;/* [subcontext_size(UserSidLength),flag(LIBNDR_FLAG_ALIGN4),subcontext(0)] */
	const char * *Strings;/* [flag(LIBNDR_FLAG_STR_NULLTERM|LIBNDR_FLAG_ALIGN2)] */
	uint8_t *Data;/* [flag(LIBNDR_PRINT_ARRAY_HEX)] */
	const char * Pad;/* [flag(LIBNDR_FLAG_STR_ASCII|LIBNDR_FLAG_STR_NULLTERM)] */
	uint32_t Length2;/* [value(Length)] */
}/* [gensize,public] */;

struct EVENTLOGEOF {
	uint32_t RecordSizeBeginning;/* [value(0x28)] */
	uint32_t One;/* [value(0x11111111)] */
	uint32_t Two;/* [value(0x22222222)] */
	uint32_t Three;/* [value(0x33333333)] */
	uint32_t Four;/* [value(0x44444444)] */
	uint32_t BeginRecord;
	uint32_t EndRecord;
	uint32_t CurrentRecordNumber;
	uint32_t OldestRecordNumber;
	uint32_t RecordSizeEnd;/* [value(0x28)] */
}/* [public] */;

struct EVENTLOG_EVT_FILE {
	struct EVENTLOGHEADER hdr;
	struct EVENTLOGRECORD *records;
	struct EVENTLOGEOF eof;
}/* [public] */;

struct EVENTLOG_FULL_INFORMATION {
	uint32_t full;
}/* [public] */;


struct eventlog_ClearEventLogW {
	struct {
		struct policy_handle *handle;/* [ref] */
		struct lsa_String *backupfile;/* [unique] */
	} in;

	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_BackupEventLogW {
	struct {
		struct policy_handle *handle;/* [ref] */
		struct lsa_String *backup_filename;/* [ref] */
	} in;

	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_CloseEventLog {
	struct {
		struct policy_handle *handle;/* [ref] */
	} in;

	struct {
		struct policy_handle *handle;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_DeregisterEventSource {
	struct {
		struct policy_handle *handle;/* [ref] */
	} in;

	struct {
		struct policy_handle *handle;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_GetNumRecords {
	struct {
		struct policy_handle *handle;/* [ref] */
	} in;

	struct {
		uint32_t *number;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_GetOldestRecord {
	struct {
		struct policy_handle *handle;/* [ref] */
	} in;

	struct {
		uint32_t *oldest_entry;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_ChangeNotify {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_OpenEventLogW {
	struct {
		struct eventlog_OpenUnknown0 *unknown0;/* [unique] */
		struct lsa_String *logname;/* [ref] */
		struct lsa_String *servername;/* [ref] */
		uint32_t major_version;
		uint32_t minor_version;
	} in;

	struct {
		struct policy_handle *handle;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_RegisterEventSourceW {
	struct {
		struct eventlog_OpenUnknown0 *unknown0;/* [unique] */
		struct lsa_String *module_name;/* [ref] */
		struct lsa_String *reg_module_name;/* [ref] */
		uint32_t major_version;
		uint32_t minor_version;
	} in;

	struct {
		struct policy_handle *log_handle;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_OpenBackupEventLogW {
	struct {
		struct eventlog_OpenUnknown0 *unknown0;/* [unique] */
		struct lsa_String *backup_logname;/* [ref] */
		uint32_t major_version;
		uint32_t minor_version;
	} in;

	struct {
		struct policy_handle *handle;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_ReadEventLogW {
	struct {
		struct policy_handle *handle;/* [ref] */
		uint32_t flags;
		uint32_t offset;
		uint32_t number_of_bytes;/* [range(0,0x7FFFF)] */
	} in;

	struct {
		uint8_t *data;/* [ref,size_is(number_of_bytes)] */
		uint32_t *sent_size;/* [ref] */
		uint32_t *real_size;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_ReportEventW {
	struct {
		struct policy_handle *handle;/* [ref] */
		time_t timestamp;
		enum eventlogEventTypes event_type;
		uint16_t event_category;
		uint32_t event_id;
		uint16_t num_of_strings;/* [range(0,256)] */
		uint32_t data_size;/* [range(0,0x3FFFF)] */
		struct lsa_String *servername;/* [ref] */
		struct dom_sid *user_sid;/* [unique] */
		struct lsa_String **strings;/* [size_is(num_of_strings),unique] */
		uint8_t *data;/* [unique,size_is(data_size)] */
		uint16_t flags;
		uint32_t *record_number;/* [unique] */
		time_t *time_written;/* [unique] */
	} in;

	struct {
		uint32_t *record_number;/* [unique] */
		time_t *time_written;/* [unique] */
		NTSTATUS result;
	} out;

};


struct eventlog_ClearEventLogA {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_BackupEventLogA {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_OpenEventLogA {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_RegisterEventSourceA {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_OpenBackupEventLogA {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_ReadEventLogA {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_ReportEventA {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_RegisterClusterSvc {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_DeregisterClusterSvc {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_WriteClusterEvents {
	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_GetLogInformation {
	struct {
		struct policy_handle *handle;/* [ref] */
		uint32_t level;
		uint32_t buf_size;/* [range(0,1024)] */
	} in;

	struct {
		uint8_t *buffer;/* [size_is(buf_size),ref] */
		uint32_t *bytes_needed;/* [ref] */
		NTSTATUS result;
	} out;

};


struct eventlog_FlushEventLog {
	struct {
		struct policy_handle *handle;/* [ref] */
	} in;

	struct {
		NTSTATUS result;
	} out;

};


struct eventlog_ReportEventAndSourceW {
	struct {
		struct policy_handle *handle;/* [ref] */
		time_t timestamp;
		enum eventlogEventTypes event_type;
		uint16_t event_category;
		uint32_t event_id;
		struct lsa_String *sourcename;/* [ref] */
		uint16_t num_of_strings;/* [range(0,256)] */
		uint32_t data_size;/* [range(0,0x3FFFF)] */
		struct lsa_String *servername;/* [ref] */
		struct dom_sid *user_sid;/* [unique] */
		struct lsa_String **strings;/* [size_is(num_of_strings),unique] */
		uint8_t *data;/* [unique,size_is(data_size)] */
		uint16_t flags;
		uint32_t *record_number;/* [unique] */
		time_t *time_written;/* [unique] */
	} in;

	struct {
		uint32_t *record_number;/* [unique] */
		time_t *time_written;/* [unique] */
		NTSTATUS result;
	} out;

};

#endif /* _HEADER_eventlog */
#endif /* _PIDL_HEADER_eventlog */
